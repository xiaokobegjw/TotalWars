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

#ifdef _MSC_VER

const char *Win32ExceptionCodeToString(DWORD exception_code)
{
	switch (exception_code) {
	case EXCEPTION_ACCESS_VIOLATION:
		return "Access violation";
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return "Misaligned data access";
	case EXCEPTION_BREAKPOINT:
		return "Breakpoint reached";
	case EXCEPTION_SINGLE_STEP:
		return "Single debug step";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return "Array access out of bounds";
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return "Denormal floating point operand";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return "Floating point division by zero";
	case EXCEPTION_FLT_INEXACT_RESULT:
		return "Inaccurate floating point result";
	case EXCEPTION_FLT_INVALID_OPERATION:
		return "Invalid floating point operation";
	case EXCEPTION_FLT_OVERFLOW:
		return "Floating point exponent overflow";
	case EXCEPTION_FLT_STACK_CHECK:
		return "Floating point stack overflow or underflow";
	case EXCEPTION_FLT_UNDERFLOW:
		return "Floating point exponent underflow";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return "Integer division by zero";
	case EXCEPTION_INT_OVERFLOW:
		return "Integer overflow";
	case EXCEPTION_PRIV_INSTRUCTION:
		return "Privileged instruction executed";
	case EXCEPTION_IN_PAGE_ERROR:
		return "Could not access or load page";
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return "Illegal instruction encountered";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return "Attempted to continue after fatal exception";
	case EXCEPTION_STACK_OVERFLOW:
		return "Stack overflow";
	case EXCEPTION_INVALID_DISPOSITION:
		return "Invalid disposition returned to the exception dispatcher";
	case EXCEPTION_GUARD_PAGE:
		return "Attempted guard page access";
	case EXCEPTION_INVALID_HANDLE:
		return "Invalid handle";
	}

	return "Unknown exception";
}

long WINAPI Win32ExceptionHandler(struct _EXCEPTION_POINTERS *pExceptInfo)
{
	return 0;
}

#endif

void debug_set_exception_handler()
{
#ifdef _MSC_VER
	SetUnhandledExceptionFilter(Win32ExceptionHandler);
#endif
}

