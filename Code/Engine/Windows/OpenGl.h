#ifndef EAE6320_WINDOWS_OPENGL_H
#define EAE6320_WINDOWS_OPENGL_H

#include "Includes.h"
#include <string>

namespace eae6320
{
	namespace Windows
	{
		namespace OpenGl
		{
			struct sHiddenWindowInfo
			{
				HWND window;
				ATOM windowClass;
				HDC deviceContext;
				HGLRC openGlRenderingContext;

				sHiddenWindowInfo() : window( NULL ), windowClass( NULL ), deviceContext( NULL ), openGlRenderingContext( NULL ) {}
			};

			bool CreateHiddenContextWindow( HINSTANCE& io_hInstance, sHiddenWindowInfo& o_info, std::string* const o_errorMessage = NULL );
			bool FreeHiddenContextWindow( HINSTANCE& i_hInstance, sHiddenWindowInfo& io_info, std::string* const o_errorMessage = NULL );
		}
	}
}

#endif
