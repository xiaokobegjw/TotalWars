#include "debug.h"
#include "exceptions.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <sstream>
#include <thread>
#include "threading/mutex_auto_lock.h"

#ifdef _MSC_VER
	#include <dbghelp.h>
	#include "filesys.h"
#endif

#if USE_CURSES
	#include "terminal_chat_console.h"
#endif

/*
	Assert
*/

void sanity_check_fn(const char *assertion, const char *file,
		unsigned int line, const char *function)
{
#if USE_CURSES
	g_term_console.stopAndWaitforThread();
#endif

	errorstream << std::endl << "In thread " << std::hex
		<< std::this_thread::get_id() << ":" << std::endl;
	errorstream << file << ":" << line << ": " << function
		<< ": An engine assumption '" << assertion << "' failed." << std::endl;

	abort();
}

void fatal_error_fn(const char *msg, const char *file,
		unsigned int line, const char *function)
{
#if USE_CURSES
	g_term_console.stopAndWaitforThread();
#endif

	errorstream << std::endl << "In thread " << std::hex
		<< std::this_thread::get_id() << ":" << std::endl;
	errorstream << file << ":" << line << ": " << function
		<< ": A fatal error occurred: " << msg << std::endl;

	abort();
}