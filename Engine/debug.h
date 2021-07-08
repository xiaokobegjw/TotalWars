#pragma once

#include <iostream>
#include <exception>
#include <cassert>
#include "utils/time_utils.h"
#include "log.h"

#ifdef _WIN32
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501
	#endif
	#include <windows.h>
	#ifdef _MSC_VER
		#include <eh.h>
	#endif
	#define NORETURN __declspec(noreturn)
	#define FUNCTION_NAME __FUNCTION__
#else
	#define NORETURN __attribute__ ((__noreturn__))
	#define FUNCTION_NAME __PRETTY_FUNCTION__
#endif

// Whether to catch all std::exceptions.
// When "catching", the program will abort with an error message.
// In debug mode, leave these for the debugger and don't catch them.
#ifdef NDEBUG
	#define CATCH_UNHANDLED_EXCEPTIONS 1
#else
	#define CATCH_UNHANDLED_EXCEPTIONS 0
#endif

/* Abort program execution immediately
 */
NORETURN extern void fatal_error_fn(
		const char *msg, const char *file,
		unsigned int line, const char *function);

#define FATAL_ERROR(msg) \
	fatal_error_fn((msg), __FILE__, __LINE__, FUNCTION_NAME)

#define FATAL_ERROR_IF(expr, msg) \
	((expr) \
	? fatal_error_fn((msg), __FILE__, __LINE__, FUNCTION_NAME) \
	: (void)(0))

/*
	sanity_check()
	Equivalent to assert() but persists in Release builds (i.e. when NDEBUG is
	defined)
*/

NORETURN extern void sanity_check_fn(
		const char *assertion, const char *file,
		unsigned int line, const char *function);

#define SANITY_CHECK(expr) \
	((expr) \
	? (void)(0) \
	: sanity_check_fn(#expr, __FILE__, __LINE__, FUNCTION_NAME))

#define sanity_check(expr) SANITY_CHECK(expr)


void debug_set_exception_handler();

/*
	These should be put into every thread
*/

#if CATCH_UNHANDLED_EXCEPTIONS == 1
	#define BEGIN_DEBUG_EXCEPTION_HANDLER try {
	#define END_DEBUG_EXCEPTION_HANDLER                        \
		} catch (std::exception &e) {                          \
			errorstream << "An unhandled exception occurred: " \
				<< e.what() << std::endl;                      \
			FATAL_ERROR(e.what());                             \
		}
#else
	// Dummy ones
	#define BEGIN_DEBUG_EXCEPTION_HANDLER
	#define END_DEBUG_EXCEPTION_HANDLER
#endif
