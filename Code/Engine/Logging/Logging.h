#ifndef EAE6320_LOGGING_H
#define EAE6320_LOGGING_H

#include "Configuration.h"

#ifndef NULL
	#define NULL 0
#endif

namespace eae6320
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
