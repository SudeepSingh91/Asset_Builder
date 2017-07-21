/*
	This file can be included by a project that wants to use MCPP

	Using this intermediate file to store the version number
	means that no other source files will have to change
	if you ever change to a new version.
*/

#ifndef MCPPINCLUDES_H
#define MCPPINCLUDES_H

// Windows and mcpp both define a global "OUT".
// This is an example of why C++ namespaces are a good thing.
#if defined( PLATFORM_WINDOWS ) && defined( OUT )
	#define WINDOWS_OUT
	#undef OUT
#endif

extern "C"
{
	#include "2.7.2/src/mcpp_lib.h"
}

namespace Engine
{
	namespace mcpp
	{
		namespace OUTDEST
		{
			enum eOUTDEST
			{
				Out = OUT,
				Err = ERR,
				Dbg = DBG,
			};
		}
	}
}

#ifdef WINDOWS_OUT
	#define OUT
	#undef WINDOWS_OUT
#endif

#endif	
