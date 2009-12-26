/*
ncrobot
Natural Computing Robot
This app is based on the TestBed example application from Box2D.

Copyright 2009 Jelmer Cnossen, particle swarm optimization code by Johan de Ruiter
License (ZLIB):

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/
#pragma once

#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <cassert>


#define PI 3.141593f

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char uchar;

std::string SPrintf(const char *fmt, ...);

std::string ReadZStr (FILE*f);
void WriteZStr (FILE *f, const std::string& s);


class ContentException : public std::runtime_error {
public:
	ContentException(const std::string& msg) : std::runtime_error(msg.c_str())
	{ }
	virtual ~ContentException() throw() {}
};


/*
 * Portable definition for SNPRINTF, VSNPRINTF, STRCASECMP and STRNCASECMP
 */
#ifdef _MSC_VER
	#if _MSC_VER > 1310
		#define SNPRINTF _snprintf_s
		#define VSNPRINTF _vsnprintf_s
	#else
		#define SNPRINTF _snprintf
		#define VSNPRINTF _vsnprintf
	#endif
	#define STRCASECMP _stricmp
	#define STRNCASECMP _strnicmp
	#define ALLOCA(size) _alloca(size) // allocates memory on stack
#else
	#define STRCASECMP strcasecmp
	#define STRNCASECMP strncasecmp
	#define SNPRINTF snprintf
	#define VSNPRINTF vsnprintf
	#define ALLOCA(size) alloca(size)
#endif
#define ALLOCA_ARRAY(T, N) (new(ALLOCA(sizeof(T) * N)) T[N])



/*
 * HASH_SET defines a hash map template in a portable way.
 * Use #include HASH_MAP_H to include the header
 */
#ifdef _MSC_VER
	#define HASH_SET stdext::hash_set
	#define HASH_SET_H <hash_set>

	#define HASH_MAP stdext::hash_map
	#define HASH_MAP_H <hash_map>
#elif defined(__GNUC__)
	#define HASH_SET __gnu_cxrgb::hash_set
	#define HASH_SET_H <ext/hash_set>

	#define HASH_MAP __gnu_cxrgb::hash_map
	#define HASH_MAP_H <ext/hash_map>

	// This is needed as gnu doesn't offer specialization for other pointer types other than char*
	// (look in ext/hash_fun.h for the types supported out of the box)

/*	#include <ext/hash_fun.h>

	namespace __gnu_cxx {
		template<> struct hash<void*> {
			size_t operator() (const void* __s) const { return (size_t)(__s); }
		};

// 		template<> struct hash<String> {
// 			size_t operator() (const String& __s) const { return hash<const char*>()(__s.c_str()); }
// 		};
		template<> struct hash<std::string> {
			size_t operator() (const std::string& __s) const { return hash<const char*>()(__s.c_str()); }
		};
	}*/
#else
	#error Unsupported compiler
#endif


template<typename T> void SafeDelete(T& item) {
	if (item!=0) {
		delete item;
		item = 0;
	}
}

template<typename T> void SafeDeleteArray(T& item) {
	if (item!=0) {
		delete[] item;
		item = 0;
	}
}

// Delete all items in a container of pointers
template<typename T> void DeleteAll(T& items) {
	for(typename T::iterator i=items.begin();	i!=items.end();	++i) {
		delete (*i);
		*i = NULL;
	}
	items.clear();
}

#include "DebugTrace.h"
#include "Vector2.h"
#include "Matrix22.h"
