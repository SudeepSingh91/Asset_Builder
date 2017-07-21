#ifndef USEROUTPUT_H
#define USEROUTPUT_H

#if defined( PLATFORM_WINDOWS )
	#include "../Windows/Includes.h"
#endif

namespace Engine
{
	namespace UserOutput
	{
		void Print( const char* const i_message, ... );

		struct sInitializationParameters
		{
#if defined( PLATFORM_WINDOWS )
			HWND mainWindow;
#endif
		};

		bool Initialize( const sInitializationParameters& i_initializationParameters );
		bool CleanUp();
	}
}

#endif
