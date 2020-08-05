/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000,2009 Oracle.  All rights reserved.
 *
 */

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif
	
void dbxml_util_siginit();
int dbxml_util_interrupted();
void dbxml_util_reset_interrupted();
void dbxml_util_sigresend();
void dbxml_set_signal_listener(void (*listener)(int));

#ifdef __cplusplus
}

namespace DbXml {
	// abstract class to call when a signal happens
	class SignalListener {
	public:
		virtual ~SignalListener() {}
		virtual void signal(int signo) = 0;
	};

	// class to wrap above interfaces
	class SigBlock {
	public:
		SigBlock();
		~SigBlock();
		bool isInterrupted() const;
    void reset();
		void setListener(SignalListener *listener);
	};
}

#endif

#endif
