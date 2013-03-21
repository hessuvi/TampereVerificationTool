/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright © 2001 Nokia and others. All Rights Reserved.

Contributor(s): Jacobus Geldenhuys.
*/

/* ------------------------------------------------------------------------
 * File:        timing.hh
 * Description: Functions for collecting timing information.
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#include "timing.hh"
#include <sys/resource.h>
#ifdef TUTNEW
#include <tutnew>
#endif

long ru_usr_time0;
long ru_sys_time0;
long ru_usr_time1;
long ru_sys_time1;
long ru_usr_time2;
long ru_sys_time2;

void
record_init_time()
{
	struct rusage ru;

	(void) getrusage(RUSAGE_SELF, &ru);
	ru_usr_time0 = ru.ru_utime.tv_sec * 1000 + ru.ru_utime.tv_usec / 1000;
	ru_sys_time0 = ru.ru_stime.tv_sec * 1000 + ru.ru_stime.tv_usec / 1000;
	ru_usr_time1 = ru_usr_time0;
	ru_sys_time1 = ru_sys_time0;
}

void
get_time(long* ut, long* st, long* tt)
{
	struct rusage ru;

	(void) getrusage(RUSAGE_SELF, &ru);
	ru_usr_time2 = ru.ru_utime.tv_sec * 1000 + ru.ru_utime.tv_usec / 1000;
	ru_sys_time2 = ru.ru_stime.tv_sec * 1000 + ru.ru_stime.tv_usec / 1000;
	*ut = ru_usr_time2 - ru_usr_time1;
	*st = ru_sys_time2 - ru_sys_time1;
	*tt = (ru_usr_time2 + ru_sys_time2) - (ru_usr_time1 + ru_sys_time1);
	ru_usr_time1 = ru_usr_time2;
	ru_sys_time1 = ru_sys_time2;
}

void
get_total_time(long* ut, long* st, long* tt)
{
	ru_usr_time1 = ru_usr_time0;
	ru_sys_time1 = ru_sys_time0;
	get_time(ut, st, tt);
}

/* vim: set tabstop=4 shiftwidth=4: */
