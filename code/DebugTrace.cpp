#include "StdIncl.h"
#include "DebugTrace.h"
#include <cstdarg>

#if defined(_MSC_VER) && defined(_DEBUG)
#define DEBUGTRACE
#endif

#ifdef DEBUGTRACE
	#include <windows.h>
#endif


void d_trace(const char *fmt, ...)
{
	static char buf[256];

	va_list ap;
	va_start(ap,fmt);
	VSNPRINTF (buf,256,fmt,ap);
	va_end(ap);

	d_puts (buf);
}

/*
d_puts - writes a string of any length to the debug output
	(as opposed to d_trace, which is limited by the buffer)
*/
void d_puts (const char *buf)
{
#ifdef DEBUGTRACE
	OutputDebugStringA(buf);
#endif

#ifdef WIN32
	printf (buf);
#endif
}



void d_assert(char *str, char *file, int line)
{
	std::string msg = SPrintf("Assertion \"%s\" failed at line %d in \'%s\'\n", str, line, file);
	d_trace (msg.c_str());

	throw std::runtime_error(msg.c_str());
}
