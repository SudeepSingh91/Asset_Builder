#ifndef LOGGING_H
#define LOGGING_H

#include "Configuration.h"

#ifndef NULL
	#define NULL 0
#endif

namespace Engine
{
	namespace Logging
	{
		bool OutputMessage( const char* const i_message, ... );
		bool OutputError(const char* const i_errorMessage, ...);
		bool Initialize( const char* const i_pathToLogTo = NULL );
		bool CleanUp();
	}
}

#endif
