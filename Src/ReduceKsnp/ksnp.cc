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
 * File:        ksnn.cc
 * Description: Kanellakis-Smolka Naive algorithm for multiple symbols
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#include "TvtCLP.hh"
#include "LogWrite.hh"

#include "algo.hh"
#include "clp.hh"
#include "global.hh"
#include "reader.hh"
#include "timing.hh"
#include "writer.hh"
#ifdef TUTNEW
#include <tutnew>
#endif

int
main(int argc, char *argv[])
{
	long u, s, t;

#ifndef NO_EXCEPTIONS
    try {
#endif
	CLP clp("Run Kanellakis-Smolka Naive algorithm for strong "
		"bisimulation reduction");
	if (!clp.parseCommandLine(argc, argv)) { return 1; }
	std::ostream& log = LogWrite::getOStream();
	init_vars();

	record_init_time();
	read_LSTS(clp.getInputStream());
	if (print_info) {
		get_time(&u, &s, &t);
		log << "  reading time: " << u << " " << s << " " << t << std::endl;
	}

	if (trans != 0) {
		ksnaivep();
	}
	if (print_info) {
		get_time(&u, &s, &t);
		log << "  processing time: " << u << " " << s << " " << t << std::endl;
	}

	write_LSTS(clp.getOutputStream());
	if (print_info) {
		get_time(&u, &s, &t);
		log << "  writing time: " << u << " " << s << " " << t << std::endl;
	}

	if (print_info) {
		get_total_time(&u, &s, &t);
		log << "  total time: " << u << " " << s << " " << t << std::endl;
		log << "  static memory: " << static_memory << " bytes" << std::endl;
		log << "  dynamic memory: " << dynamic_memory << " bytes" << std::endl;
		log << "  size before: S " << N << " T " << M << " A " << K << std::endl;
		log << "  size after: S " << final_block_count << " T " << final_M
			<< " A " << K << std::endl;
	}
#ifndef NO_EXCEPTIONS
    } catch(...) { return 1; }
#endif
	return 0;
}

/* vim: set tabstop=4 shiftwidth=4: */
