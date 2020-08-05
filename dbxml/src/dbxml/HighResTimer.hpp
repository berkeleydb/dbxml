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

class DBXML_EXPORT HighResTimer
{
public:
	HighResTimer(const char *name = 0, const char *what = 0, const char *whats = 0);
	virtual ~HighResTimer();
	void start();
	void stop();
	void duration(time_t *seconds, time_t *microseconds) const;
	double durationInSeconds() const;
	void reset();
	std::string asString() const;
	long count() const
	{
		return count_;
	}
	double rate() const;

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

