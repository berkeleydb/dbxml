//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#include "Timer.hpp"

#include <db.h>
#include <iostream>
#include <sstream>
#include "DbXmlInternal.hpp"

using namespace DbXml;

Timer::Timer(const char *name, const char *what, const char *whats)
	: name_(name),
	what_(what),
	whats_(whats),
	count_(0),
	start_secs(0), start_usecs(0),
	duration_secs(0), duration_usecs(0)
{}

Timer::~Timer()
{}

void Timer::start()
{
	DB_GETTIME(start_secs, start_usecs);
}

void Timer::stop()
{
	time_t stop_secs, stop_usecs;
	DB_GETTIME(stop_secs, stop_usecs);
	duration_secs += (stop_secs - start_secs);
	duration_usecs += (stop_usecs - start_usecs);
	// fixup for usec under/overflow
	if (duration_usecs < 0) {
		duration_secs -= 1;
		duration_usecs += USECS_PER_SEC;
	} else if (duration_usecs > USECS_PER_SEC) {
		duration_secs += 1;
		duration_usecs -= USECS_PER_SEC;
	}
	count_++;
}

void Timer::duration(time_t *seconds, time_t *microseconds) const
{
	*seconds = duration_secs;
	*microseconds = duration_usecs;
}

void Timer::reset()
{
	start_secs = 0;
	start_usecs = 0;
	duration_secs = 0;
	duration_usecs = 0;
}

double Timer::durationInSeconds() const
{
	time_t seconds, microseconds;
	duration(&seconds, &microseconds);
	return (double)seconds + ((double)microseconds / (double)USECS_PER_SEC);
}

double Timer::durationInUsecs() const
{
	time_t seconds, microseconds;
	duration(&seconds, &microseconds);
	return ((double)seconds * (double)USECS_PER_SEC) + (double)microseconds;
}

double Timer::rate(long count) const
{
	if (count == 0)
		count = count_;
	double duration = durationInSeconds();
	double rate = duration == 0 ? 0 : (count / duration);
	return rate;
}

std::string Timer::asString() const
{
	double duration = durationInSeconds();
	double average = count_ == 0 ? 0 : (duration / count_);
	double rate = average == 0 ? 0 : (1 / average);
	std::ostringstream s;
	s << (name_ == 0 ? "noname" : name_) << " Timer: ";
	s << count_ << " " << (whats_ != 0 ? whats_ : "") << " in " << duration << " seconds, ";
	s << average << " seconds per " << (what_ != 0 ? what_ : "") << ", ";
	s << rate << " " << whats_ << " per second.";
	return s.str();
}
