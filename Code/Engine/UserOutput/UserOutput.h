#ifndef EAE6320_USEROUTPUT_H
#define EAE6320_USEROUTPUT_H

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "../Windows/Includes.h"
#endif

namespace eae6320
{
	namespace UserOutput
	{
		void Print( const char* const i_message, ... );

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow;
#endif
		};

		bool Initialize( const sInitializationParameters& i_initializationParameters );
		bool CleanUp();
	}
}

#endif
