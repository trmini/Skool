/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <unistd.h>

#ifdef __linux__
#include <sched.h>
#ifndef SCHED_IDLE
#define SCHED_IDLE			3
#endif
static int use_yield = 0;
#endif

#ifdef __CYGWIN32__
extern int nice(int);
#endif

#include "params.h"
#include "config.h"
#include "options.h"

void idle_init()
{
#ifdef __linux__
	struct sched_param param = {0};
#endif

	if (!cfg_get_bool(SECTION_OPTIONS, NULL, "Idle")) return;
	if (options.flags & FLG_STDOUT) return;

	nice(20);

#ifdef __linux__
	use_yield = sched_setscheduler(getpid(), SCHED_IDLE, &param) ? 1 : 2;
#endif
}

void idle_yield(int always)
{
#ifdef __linux__
	if (use_yield)
	if (use_yield == 1 || always) sched_yield();
#endif
}
