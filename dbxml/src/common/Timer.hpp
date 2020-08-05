//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#ifndef __TIMER_H
#define	__TIMER_H

#include <string>
#include <db.h>

namespace DbXml
{

class Timer
{
public:
	Timer(const char *name = 0, const char *what = 0,
	      const char *whats = 0);
	virtual ~Timer();
	void start();
	void stop();
	void duration(time_t *seconds, time_t *microseconds) const;
	double durationInSeconds() const;
	double durationInUsecs() const;
	void reset();
	std::string asString() const;
	long count() const {
		return count_;
	}
	double rate(long count = 0) const; // if 0, use count_
private:
	const char *name_;
	const char *what_;
	const char *whats_;
	long count_;
	time_t start_secs, start_usecs;
	time_t duration_secs, duration_usecs;
};

}

#endif

