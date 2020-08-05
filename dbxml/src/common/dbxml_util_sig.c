/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000,2009 Oracle.  All rights reserved.
 *
 */

/*
 * This file is virtually identical to db/common/util_sig.c
 * It is intended to be used by common BDB XML utilities to
 * handle signals as necessary.
 * It's intended usage is to hold off important signals across
 * critical sections. The application can then check to see if
 * an interrupt happened in locations of its own choosing.
 *
 * Application usage pattern:
 *   __db_util_siginit() // initialize
 *   while (true) {
 *     do stuff
 *     if (__db_util_interrupted())
 *       break;
 *   }
 *   app-specific cleanup
 *   __db_util_sigresend();
 *   exit
 */

#include "dbxml_config.h"

#include <sys/types.h>
#include <signal.h>

static int	interrupt = 0;
static void (*signal_listener)(int) = 0;
static void	set_signal(int, int);
static void	signal_handler(int);

/*
 * signal_handler --
 *	Interrupt signal handler.
 */
static void
signal_handler(signo)
	int signo;
{
#ifndef HAVE_SIGACTION
	/* Assume signal() is unreliable and reset it, first thing. */
	set_signal(signo, 0);
#endif
	/* Some systems don't pass in the correct signal value -- check. */
	if ((interrupt = signo) == 0)
		interrupt = SIGINT;

  if(signal_listener != 0)
    (*signal_listener)(signo);
}

/*
 * set_signal
 */
static void
set_signal(s, is_dflt)
	int s, is_dflt;
{
	/*
	 * Use sigaction if it's available, otherwise use signal().
	 */
#ifdef HAVE_SIGACTION
	struct sigaction sa, osa;

	sa.sa_handler = is_dflt ? SIG_DFL : signal_handler;
	(void)sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	(void)sigaction(s, &sa, &osa);
#else
	(void)signal(s, is_dflt ? SIG_DFL : signal_handler);
#endif
}

void
dbxml_set_signal_listener(listener)
     void (*listener)(int);
{
  signal_listener = listener;
}

/*
 * __db_util_siginit --
 *
 * PUBLIC: void __db_util_siginit __P((void));
 */
void
dbxml_util_siginit()
{
	/*
	 * Initialize the set of signals for which we want to clean up.
	 * Generally, we try not to leave the shared regions locked if
	 * we can.
	 */
#ifdef SIGHUP
	set_signal(SIGHUP, 0);
#endif
#ifdef SIGINT
	set_signal(SIGINT, 0);
#endif
#ifdef SIGPIPE
	set_signal(SIGPIPE, 0);
#endif
#ifdef SIGTERM
	set_signal(SIGTERM, 0);
#endif
}

/*
 * __db_util_interrupted --
 *	Return if interrupted.
 *
 * PUBLIC: int __db_util_interrupted __P((void));
 */
int
dbxml_util_interrupted()
{
	return (interrupt != 0);
}

void
dbxml_util_reset_interrupted()
{
	interrupt = 0;
}

/*
 * __db_util_sigresend --
 *
 * PUBLIC: void __db_util_sigresend __P((void));
 */
void
dbxml_util_sigresend()
{
	/* Resend any caught signal. */
	if (interrupt != 0) {
		set_signal(interrupt, 1);

		(void)raise(interrupt);
		/* NOTREACHED */
	}
}
