//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __BUFFER_HPP
#define	__BUFFER_HPP

#include <string>
#include "DbXmlInternal.hpp"

namespace DbXml
{

class Buffer
{
public:
	/// Default constructor.  Use Set to construct the buffer.
	Buffer();
	/// Construct from a buffer and size. Buffer copies this into its own memory.
	Buffer(const void *p, size_t size);
	/// Construct from a Buffer, with some extra space. Buffer copies this into its own memory.
	Buffer(const Buffer& buffer, size_t extra = 0);
	/// Construct from a buffer and size.  Buffer points to this memory. If wrapper is true, buffer can't expand. Buffer won't deallocate this.
	Buffer(const void *p, size_t size, bool wrapper);
	/// Default destructor
	~Buffer();

	/// Assignment operator
	Buffer& operator= (const Buffer& buffer);
	/// Set the buffer contents from a buffer and size.  Used for delayed construction.
	void set(void *p, size_t nSize);

	/// Get the size of the buffer in bytes.
	size_t getSize() const;
	/// Get the current cursor position as a bytes offset.
	size_t getCursorPosition() const;
	/// Set the buffer cursor to the start of the buffer.
	void resetCursorPosition();
	/// Returns the amount of buffer used. The highest position reached by the cursor.
	size_t getOccupancy() const;
	// Set the buffer occupancy to zero.
	void resetOccupancy();
	// Set the buffer occupancy, must not be bigger than the buffer size
	void setOccupancy(size_t size);
	/// Return the buffer to its before use state. (Cursor==0 && Occupancy==0)
	void reset();
	/// Get a pointer to the internal buffer
	void* getBuffer(size_t offset = 0) const;
	/// Get a pointer to the internal buffer and donate it to the caller
	void* donateBuffer(size_t offset = 0);
	/// Get a pointer to the cursor position
	void* getCursor() const;
	/// How much data is remaining between the cursor and the occupancy.
	size_t getRemaining() const;

	/// Seek n bytes into buffer p from the current position.
	size_t seek(size_t n) { return readSeek(0, n); }
	/// Read n bytes into buffer p from the current position.
	size_t read(void *p, size_t n) { return readSeek(p, n); }
	/// Write n bytes from buffer p to the current position.
	size_t write(const void *p, size_t n);

	/// Reserve a space of size n in the buffer, returning the offset to it
	size_t reserve(size_t &offset, size_t n);
	/// Read n bytes into buffer p from the given offset
	size_t read(size_t offset, void* p, size_t n);
	/// Write n bytes from buffer p to the given offset
	size_t write(size_t offset, const void *p, size_t n);
	/// Write n bytes from buffer p onto the end of the occupied buffer
	size_t append(const void *p, size_t n);

	/// Comparison operator
	int operator==(const Buffer& buffer) const;
	int operator!=(const Buffer& buffer) const;
	bool operator<(const Buffer& buffer) const;

	/// Debug method to get a text representation of the buffer
	std::string asString(bool textOnly = false) const;
	std::string asStringBrief() const;

private:
	void deleteBuffer();
	void expandBuffer(size_t amount);
	size_t readSeek(void *p, size_t n);

	size_t bufferSize_;
	void* pBuffer_;
	void* pCursor_;
	void* pOccupancy_;
	bool ownsMemory_;
};

inline size_t Buffer::getSize() const
{
	return bufferSize_;
}

inline void Buffer::resetCursorPosition()
{
	pCursor_ = pBuffer_;
}

inline void* Buffer::getBuffer(size_t offset) const
{
	return (void*)((char*)pBuffer_ + offset);
}

// this will only work if the caller is using
// ::free to free the memory
inline void* Buffer::donateBuffer(size_t offset)
{
	DBXML_ASSERT(ownsMemory_ && (offset == 0));
	void *ret = (void*)((char*)pBuffer_ + offset);
	ownsMemory_ = false;
	return ret;
}

inline void* Buffer::getCursor() const
{
	return pCursor_;
}

inline int Buffer::operator!=(const Buffer& buffer) const
{
	return !operator==(buffer);
}

inline void Buffer::resetOccupancy()
{
	pOccupancy_ = pBuffer_;
}

inline void Buffer::reset()
{
	resetCursorPosition();
	resetOccupancy();
}

inline size_t Buffer::getRemaining() const
{
	return static_cast<char*>(pOccupancy_) - static_cast<char*>(pCursor_);
}

}

#endif

