//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSHANDLERBASE_HPP
#define __DBXMLNSHANDLERBASE_HPP

/*
 * NsHandlerBase
 *
 * A base class for handling NsEvent input, used for
 * creation of Node Storage nodes, as well as
 * "transient" parsing for whole-document storage
 * input and querying.
 *
 * It has no virtual behavior.
 */

#include "NsTypes.hpp"
#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "NsNid.hpp"
#include "../BulkPut.hpp"

#ifndef DBXML_BULK
#include <vector>
#endif

namespace DbXml
{

class EventWriter;
struct nsTextList;
	
#ifndef DBXML_BULK
/*
	Keeps track of the predicted size of a node and the eventual real size of that node
	so a more accurate prediction can be made.  Also tracks the error in predicting.
*/
class SizePredictionStatistics {
public:
	SizePredictionStatistics();
	int64_t getPrediction() const;
	int64_t getAverage() const;
	double getPercentUnder() const;
	int64_t getAverageUnder() const;
	double getPercentOver() const;
	int64_t getAverageOver() const;
	void increment(int64_t error, int64_t correctPrediction);
	void reset();
private:
	int64_t _count;
	int64_t _undercount;
	int64_t _under;
	int64_t _overcount;
	int64_t _over;
	int64_t _prediction;
};
#endif
	
class NsHandlerBase {
public:
	NsHandlerBase(NsDocument *doc,
		      EventWriter *writer);
	~NsHandlerBase();

	// Configuration
	void setBaseEventWriter(EventWriter *writer) {
		_ewriter = writer;
	}
	EventWriter *getBaseEventWriter() const {
		return _ewriter;
	}

	// Explicit end call for partial documents
	void end(void);

	void nextId(NsFullNid *id);
	void incDepth() { ++_depth; }
	void decDepth() { --_depth; }

protected:
	void addText(const xmlbyte_t *text, size_t len, uint32_t textType,
		     bool isDonated = false);
	void addPI(const xmlbyte_t *target, const xmlbyte_t *data,
		   bool isDonated, size_t len);
	void startElem(NsNode *node,
		       const xmlbyte_t *baseName,
		       const char *uri, // may be null
		       const char *prefix,
		       bool isDonated);
	void endDoc(void);
	void endElem(void);
#ifndef DBXML_BULK
	size_t gatherStatistics(const NsNode *node);
#endif
protected:
	// parse state
	NsDocument *_doc;
	EventWriter *_ewriter;
	NsNode* _current;
	NsNode* _previous;
	uint32_t _depth;
	bool _lastWasText;
	struct nsTextList *_textList;
#ifndef DBXML_BULK
	/* Berkeley DB behaves the best space wise when records are inserted in key order, or
	 * in XML's specific case, document order.  However, the only way to insert them in 
	 * document order is to insert a fake node when the first element child of the node is
	 * discovered, and then insert the real node after it is fully explored.  The size of the
	 * fake node is a guess, and the SizePredictionStatistics class records how close that
	 * guess is to the real size.
	 */
	std::vector<size_t> _sizePredictions;
	SizePredictionStatistics _stats;
#endif
	// nid
	NsNidGen _nidGen;
};

}

#endif
