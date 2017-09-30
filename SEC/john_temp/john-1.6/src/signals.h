/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Signal handling.
 */

#ifndef _JOHN_SIGNALS_H
#define _JOHN_SIGNALS_H

#include "arch.h"

/*
 * Event flags.
 *
 * Why aren't these put into a bitmask? The reason is that it's not possible
 * to clear individual flags in a bitmask without a race condition on RISC,
 * or having to block the signals.
 */
extern int event_pending;	/* An event is pending */
extern int event_abort;		/* Abort requested */
extern int event_save;		/* Time to save the crash recovery file */
extern int event_status;	/* Status display requested */

#if !OS_TIMER
/*
 * Timer emulation for systems with no setitimer(2).
 */
#include <sys/times.h>
extern void sig_timer_emu_init(clock_t interval);
extern void sig_timer_emu_tick();
#endif

/*
 * Installs the signal handlers.
 */
extern void sig_init(void (*yield)(int always));

/*
 * Terminates the process if event_abort is set.
 */
extern void check_abort();

#endif
