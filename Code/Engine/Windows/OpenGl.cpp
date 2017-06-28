#include "OpenGl.h"
#include "Functions.h"
#include <sstream>
#include "../Asserts/Asserts.h"

namespace
{
	const char* const s_hiddenWindowClass_name = "EAE6320 Hidden OpenGL Context Window Class";
}

bool eae6320::Windows::OpenGl::CreateHiddenContextWindow( HINSTANCE& io_hInstance, sHiddenWindowInfo& o_info, std::string* const o_errorMessage )
{
	bool wereThereErrors = false;

	{
		if ( io_hInstance == NULL )
		{
			const DWORD flags =
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
				| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT
				;
			static const char* hackyWayOfGettingAnAddressInsideThisModule = "";
			if ( GetModuleHandleEx( flags, hackyWayOfGettingAnAddressInsideThisModule, &io_hInstance ) == FALSE )
			{
				wereThereErrors = true;
				if ( o_errorMessage )
				{
					const std::string windowsErrorMessage = GetLastSystemError();
					std::ostringstream errorMessage;
					errorMessage << "Windows failed to get the current instance handle: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		{
			WNDCLASSEX wndClassEx = { 0 };
			{
				wndClassEx.cbSize = sizeof( WNDCLASSEX );
				wndClassEx.lpfnWndProc = DefWindowProc;
				wndClassEx.hInstance = io_hInstance;
				wndClassEx.lpszClassName = s_hiddenWindowClass_name;
			}
			o_info.windowClass = RegisterClassEx( &wndClassEx );
			if ( o_info.windowClass == NULL )
			{
				wereThereErrors = true;
				if ( o_errorMessage )
				{
					const std::string windowsErrorMessage = GetLastSystemError();
					std::ostringstream errorMessage;
					errorMessage << "Windows failed to register the hidden OpenGL context window's class: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		{
			const char* const windowName = "EAE6320 Hidden OpenGL Context Window";
			const DWORD windowStyle = WS_POPUP
				| WS_MINIMIZE;	
			const DWORD windowStyle_extended = 0;
			const int position = CW_USEDEFAULT;
			const int dimension = CW_USEDEFAULT;
			const HWND hParent = NULL;
			const HMENU hMenu = NULL;
			void* userData = NULL;
			o_info.window = CreateWindowEx( windowStyle_extended, s_hiddenWindowClass_name, windowName, windowStyle,
				position, position, dimension, dimension,
				hParent, hMenu, io_hInstance, userData );
			if ( o_info.window == NULL )
			{
				wereThereErrors = true;
				if ( o_errorMessage )
				{
					const std::string windowsErrorMessage = GetLastSystemError();
					std::ostringstream errorMessage;
					errorMessage << "Windows failed to create the hidden OpenGL context window: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
	}
	{
		o_info.deviceContext = GetDC( o_info.window );
		if ( o_info.deviceContext == NULL )
		{
			wereThereErrors = true;
			if ( o_errorMessage )
			{
				*o_errorMessage = "Windows failed to get the hidden OpenGL device context";
			}
			goto OnExit;
		}
	}
	{
		PIXELFORMATDESCRIPTOR desiredPixelFormat = { 0 };
		{
			desiredPixelFormat.nSize = sizeof( PIXELFORMATDESCRIPTOR );
			desiredPixelFormat.nVersion = 1;

			desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL
				| PFD_DEPTH_DONTCARE | PFD_DOUBLEBUFFER_DONTCARE | PFD_STEREO_DONTCARE;
			desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
			desiredPixelFormat.cColorBits = 24;
			desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
		}
		int pixelFormatId;
		{
			pixelFormatId = ChoosePixelFormat( o_info.deviceContext, &desiredPixelFormat );
			if ( pixelFormatId == 0 )
			{
				wereThereErrors = true;
				if ( o_errorMessage )
				{
					const std::string windowsErrorMessage = GetLastSystemError();
					std::ostringstream errorMessage;
					errorMessage << "Windows couldn't choose the closest pixel format"
						" for the hidden OpenGL context window: "
						<< windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		if ( SetPixelFormat( o_info.deviceContext, pixelFormatId, &desiredPixelFormat ) == FALSE )
		{
			wereThereErrors = true;
			if ( o_errorMessage )
			{
				const std::string windowsErrorMessage = GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "Windows couldn't set the desired pixel format " << pixelFormatId
					<< " for the hidden OpenGL context window: "
					<< windowsErrorMessage;
				*o_errorMessage = errorMessage.str();
			}
			goto OnExit;
		}
	}
	o_info.openGlRenderingContext = wglCreateContext( o_info.deviceContext );
	if ( o_info.openGlRenderingContext == NULL )
	{
		wereThereErrors = true;
		if ( o_errorMessage )
		{
			const std::string windowsErrorMessage = GetLastSystemError();
			std::ostringstream errorMessage;
			errorMessage << "Windows failed to create a hidden OpenGL rendering context: "
				<< windowsErrorMessage;
			*o_errorMessage = errorMessage.str();
		}
		goto OnExit;
	}
	if ( wglMakeCurrent( o_info.deviceContext, o_info.openGlRenderingContext ) == FALSE )
	{
		wereThereErrors = true;
		if ( o_errorMessage )
		{
			const std::string windowsErrorMessage = GetLastSystemError();
			std::ostringstream errorMessage;
			errorMessage << "Windows failed to set the current OpenGL rendering context"
				" for the hidden window: "
				<< windowsErrorMessage;
			*o_errorMessage = errorMessage.str();
		}
		goto OnExit;
	}

OnExit:

	return !wereThereErrors;
}

bool eae6320::Windows::OpenGl::FreeHiddenContextWindow( HINSTANCE& i_hInstance, sHiddenWindowInfo& io_info, std::string* const o_errorMessage )
{
	bool wereThereErrors = false;

	if ( io_info.openGlRenderingContext != NULL )
	{
		EAE6320_ASSERTF( io_info.deviceContext != NULL, "If a rendering context exists then a device context should also" );
		if ( wglGetCurrentContext() == io_info.openGlRenderingContext )
		{
			if ( wglMakeCurrent( io_info.deviceContext, NULL ) == FALSE )
			{
				wereThereErrors = true;
				EAE6320_ASSERT( false );
				if ( o_errorMessage )
				{
					const std::string windowsError = GetLastSystemError();
					std::ostringstream errorMessage;
					errorMessage << "\nWindows failed to unset the current hidden OpenGL device context: " << windowsError;
					*o_errorMessage += errorMessage.str();
				}
			}
		}
		if ( wglDeleteContext( io_info.openGlRenderingContext ) == FALSE )
		{
			wereThereErrors = true;
			EAE6320_ASSERT( false );
			if ( o_errorMessage )
			{
				const std::string windowsError = GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "\nWindows failed to delete the hidden OpenGL rendering context: " << windowsError;
				*o_errorMessage += errorMessage.str();
			}
		}
		io_info.openGlRenderingContext = NULL;
	}
	if ( io_info.deviceContext != NULL )
	{
		if ( io_info.window != NULL )
		{
			ReleaseDC( io_info.window, io_info.deviceContext );
			io_info.deviceContext = NULL;
		}
		else
		{
			EAE6320_ASSERTF( io_info.window != NULL, "A window handle is required to release a device context" );
		}
	}
	if ( io_info.window != NULL )
	{
		EAE6320_ASSERTF( io_info.windowClass != NULL, "If a window was created then a window class should also be provided" );
		if ( DestroyWindow( io_info.window ) == FALSE )
		{
			wereThereErrors = true;
			EAE6320_ASSERT( false );
			if ( o_errorMessage )
			{
				const std::string windowsErrorMessage = GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "\nWindows failed to destroy the hidden OpenGL context window: " << windowsErrorMessage;
				*o_errorMessage += errorMessage.str();
			}			
		}
		io_info.window = NULL;
	}
	if ( io_info.windowClass != NULL )
	{
		EAE6320_ASSERTF( i_hInstance != NULL, "The provided instance handle is NULL" );
		if ( UnregisterClass( s_hiddenWindowClass_name, i_hInstance ) == FALSE )
		{
			EAE6320_ASSERT( false );
			wereThereErrors = true;
			if ( o_errorMessage )
			{
				const std::string windowsErrorMessage = GetLastSystemError();
				std::ostringstream errorMessage;
				errorMessage << "\nWindows failed to unregister the hidden OpenGL context window's class \""
					<< io_info.windowClass << "\": " << windowsErrorMessage;
				*o_errorMessage += errorMessage.str();
			}
		}
		io_info.windowClass = NULL;
	}

	return !wereThereErrors;
}
