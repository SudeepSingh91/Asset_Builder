#include "../cbApplication.h"
#include <cstdlib>
#include "../../Asserts/Asserts.h"
#include "../../Graphics/Graphics.h"
#include "../../Logging/Logging.h"
#include "../../UserOutput/UserOutput.h"
#include "../../UserSettings/UserSettings.h"
#include "../../Windows/Functions.h"

namespace
{
	bool CreateMainWindow( const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_windowName, const ATOM i_windowClass,
		const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight,
		Engine::Application::cbApplication& io_application, HWND& o_window );
	bool CreateMainWindowClass( const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_mainWindowClassName,
		WNDPROC fOnMessageReceivedFromWindows, ATOM& o_mainWindowClass,
		const WORD* const i_iconId_large = NULL, const WORD* const i_iconId_small = NULL );
	bool FreeMainWindow( HWND& io_window );
	bool FreeMainWindowClass( const HINSTANCE i_thisInstanceOfTheApplication, ATOM& io_mainWindowClass );
}

bool Engine::Application::cbApplication::GetResolution( unsigned int& o_width, unsigned int& o_height ) const
{
	if ( ( m_resolutionWidth != 0 ) && ( m_resolutionHeight != 0 ) )
	{
		o_width = m_resolutionWidth;
		o_height = m_resolutionHeight;
		return true;
	}
	else
	{
		return false;
	}
}

Engine::Application::cbApplication* Engine::Application::cbApplication::GetApplicationFromWindow( const HWND i_window )
{
	const LONG_PTR userData = GetWindowLongPtr( i_window, GWLP_USERDATA );
	cbApplication* const application = reinterpret_cast<cbApplication*>( userData );
	ASSERT( application != NULL );
	ASSERT( application->m_mainWindow == i_window );
	return application;
}

bool Engine::Application::cbApplication::WaitForApplicationToFinish( int& o_exitCode )
{
	MSG message = { 0 };
	do
	{
		bool hasWindowsSentAMessage;
		{
			HWND getMessagesFromAnyWindowBelongingToTheCurrentThread = NULL;
			unsigned int getAllMessageTypes = 0;
			unsigned int ifAMessageExistsRemoveItFromTheQueue = PM_REMOVE;
			hasWindowsSentAMessage = PeekMessage( &message, getMessagesFromAnyWindowBelongingToTheCurrentThread,
				getAllMessageTypes, getAllMessageTypes, ifAMessageExistsRemoveItFromTheQueue ) == TRUE;
		}
		if ( !hasWindowsSentAMessage )
		{
			OnNewFrame();
		}
		else
		{
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
	} while ( message.message != WM_QUIT );

	o_exitCode = static_cast<int>( message.wParam );

	return true;
}

LRESULT CALLBACK Engine::Application::cbApplication::OnMessageReceivedFromWindows( HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam )
{
	switch( i_message )
	{
	case WM_CHAR:
		{
			if ( i_wParam == VK_ESCAPE )
			{
				int result;
				{
					const char* const caption = "Exit Application?";
					const char* const message = "Are you sure you want to quit?";
					result = MessageBox( i_window, message, caption, MB_YESNO | MB_ICONQUESTION );
				}
				if ( result == IDYES )
				{
					{
						const int exitCode = EXIT_SUCCESS;
						PostQuitMessage( exitCode );	
					}
					return 0;
				}
			}
		}
		break;
	case WM_CREATE:
		{
			const CREATESTRUCT& creationData = *reinterpret_cast<CREATESTRUCT*>( i_lParam );
			Engine::Application::cbApplication* application =
				reinterpret_cast<Engine::Application::cbApplication*>( creationData.lpCreateParams );
			ASSERT( application != NULL );
			application->m_mainWindow = i_window;
			{
				SetLastError( ERROR_SUCCESS );
				const LONG_PTR previousUserData = SetWindowLongPtr( i_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( application ) );
				if ( previousUserData == NULL )
				{
					DWORD errorCode;
					const std::string errorMessage = Windows::GetLastSystemError( &errorCode );
					if ( errorCode != ERROR_SUCCESS )
					{
						ASSERTF( "Couldn't set main window user data: %s", errorMessage.c_str() );
						Logging::OutputError( "Windows failed to set the main window's user data: %s", errorMessage.c_str() );
						application->m_mainWindow = NULL;
						return -1;
					}
				}
			}
			return 0;
		}
		break;
	case WM_NCDESTROY:
		{
			{
				cbApplication* const application = GetApplicationFromWindow( i_window );
				if ( application != NULL )
				{
					application->m_mainWindow = NULL;
				}
			}
			const int exitCode = EXIT_SUCCESS;
			PostQuitMessage( exitCode );	
			return 0;
		}
		break;
	}
	return DefWindowProc( i_window, i_message, i_wParam, i_lParam );
}

bool Engine::Application::cbApplication::Initialize_base( const sEntryPointParameters& i_entryPointParameters )
{
	bool wereThereErrors = false;
	m_thisInstanceOfTheApplication = i_entryPointParameters.applicationInstance;

	if ( !CreateMainWindowClass( m_thisInstanceOfTheApplication, GetMainWindowClassName(),
		OnMessageReceivedFromWindows, m_mainWindowClass,
		GetLargeIconId(), GetSmallIconId() ) )
	{
		ASSERT( false );
		wereThereErrors = true;
		goto OnExit;
	}
	{
		HWND hWindow;
		const unsigned int desiredResolutionWidth = UserSettings::GetResolutionWidth();
		const unsigned int desiredResolutionHeight = UserSettings::GetResolutionHeight();
		if ( CreateMainWindow( m_thisInstanceOfTheApplication, GetMainWindowName(), m_mainWindowClass,
			desiredResolutionWidth, desiredResolutionHeight,
			*this, hWindow ) )
		{
			ASSERT( hWindow == m_mainWindow );
			m_resolutionWidth = desiredResolutionWidth;
			m_resolutionHeight = desiredResolutionHeight;
		}
		else
		{
			ASSERT( false );
			wereThereErrors = true;
			goto OnExit;
		}
	}
	{
		const BOOL wasWindowPreviouslyVisible = ShowWindow( m_mainWindow, i_entryPointParameters.initialWindowDisplayState );
	}

OnExit:

	return !wereThereErrors;
}

bool Engine::Application::cbApplication::PopulateGraphicsInitializationParameters( Graphics::sInitializationParameters& o_initializationParameters )
{
	ASSERT( m_mainWindow != NULL );
	o_initializationParameters.mainWindow = m_mainWindow;
#if defined( PLATFORM_D3D )
	o_initializationParameters.resolutionWidth = m_resolutionWidth;
	o_initializationParameters.resolutionHeight = m_resolutionHeight;
#elif defined( PLATFORM_GL )
	o_initializationParameters.thisInstanceOfTheApplication = m_thisInstanceOfTheApplication;
#endif
	return true;
}

bool Engine::Application::cbApplication::PopulateUserOutputInitializationParameters( UserOutput::sInitializationParameters& o_initializationParameters )
{
	ASSERT( m_mainWindow != NULL );
	o_initializationParameters.mainWindow = m_mainWindow;
	return true;
}

bool Engine::Application::cbApplication::CleanUp_base()
{
	bool wereThereErrors = false;

	if ( m_mainWindow )
	{
		if ( !FreeMainWindow( m_mainWindow ) )
		{
			ASSERT( false );
			wereThereErrors = true;
		}
	}
	if ( m_mainWindowClass )
	{
		if ( !FreeMainWindowClass( m_thisInstanceOfTheApplication, m_mainWindowClass ) )
		{
			ASSERT( false );
			wereThereErrors = true;
		}
	}

	return !wereThereErrors;
}

Engine::Application::cbApplication::cbApplication()
	:
	m_mainWindow( NULL ), m_mainWindowClass( NULL ), m_thisInstanceOfTheApplication( NULL ),
	m_resolutionWidth( 0 ), m_resolutionHeight( 0 )
{

}

namespace
{
	bool CreateMainWindow( const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_windowName, const ATOM i_windowClass,
		const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight,
		Engine::Application::cbApplication& io_application, HWND& o_window )
	{
		{
			const DWORD windowStyle =
				WS_OVERLAPPED
				| WS_CAPTION
				| WS_MINIMIZEBOX
				| WS_SYSMENU;
			const DWORD windowStyle_extended =
				WS_EX_OVERLAPPEDWINDOW;
			const int width = CW_USEDEFAULT;
			const int height = CW_USEDEFAULT;
			const int position_x = CW_USEDEFAULT;
			const int position_y = CW_USEDEFAULT;
			const HWND hParent = NULL;
			const HMENU hMenu = NULL;
			void* const userData = &io_application;

			o_window = CreateWindowEx( windowStyle_extended, MAKEINTATOM( i_windowClass ), i_windowName, windowStyle,
				position_x, position_y, width, height,
				hParent, hMenu, i_thisInstanceOfTheApplication, userData );
			if ( o_window != NULL )
			{
				Engine::Logging::OutputMessage( "Created main window \"%s\"", i_windowName );
			}
			else
			{
				const std::string windowsErrorMessage = Engine::Windows::GetLastSystemError();
				ASSERTF( false, "Main window wasn't created: %s", windowsErrorMessage.c_str() );
				Engine::Logging::OutputError( "Windows failed to create the main window: %s", windowsErrorMessage.c_str() );
				return false;
			}
		}

		{
			RECT windowCoordinates;
			struct
			{
				long width;
				long height;
			} nonClientAreaSize;
			{
				if ( GetWindowRect( o_window, &windowCoordinates ) == FALSE )
				{
					const std::string windowsErrorMessage = Engine::Windows::GetLastSystemError();
					ASSERTF( false, "Couldn't get coordinates of the main window: %s", windowsErrorMessage.c_str() );
					Engine::Logging::OutputError( "Windows failed to get the coordinates of the main window: %s", windowsErrorMessage.c_str() );
					goto OnError;
				}
				RECT clientDimensions;
				if ( GetClientRect( o_window, &clientDimensions ) == FALSE )
				{
					const std::string windowsErrorMessage = Engine::Windows::GetLastSystemError();
					ASSERTF( false, "Couldn't get the main window's client dimensions: %s", windowsErrorMessage.c_str() );
					Engine::Logging::OutputError( "Windows failed to get the dimensions of the main window's client area:: %s",
						windowsErrorMessage.c_str() );
					goto OnError;
				}
				nonClientAreaSize.width = ( windowCoordinates.right - windowCoordinates.left ) - clientDimensions.right;
				nonClientAreaSize.height = ( windowCoordinates.bottom - windowCoordinates.top ) - clientDimensions.bottom;
			}
			{
				const int desiredWidth_window = i_resolutionWidth + nonClientAreaSize.width;
				const int desiredHeight_window = i_resolutionHeight + nonClientAreaSize.height;
				const BOOL redrawTheWindowAfterItsBeenResized = TRUE;
				if ( MoveWindow( o_window,
					windowCoordinates.left, windowCoordinates.top, desiredWidth_window, desiredHeight_window,
					redrawTheWindowAfterItsBeenResized ) != FALSE )
				{
					Engine::Logging::OutputMessage( "Set main window resolution to %u x %u",
						i_resolutionWidth, i_resolutionHeight );
				}
				else
				{
					const std::string windowsErrorMessage = Engine::Windows::GetLastSystemError();
					ASSERTF( false, "Couldn't resize the main window to &i x &i: %s",
						desiredWidth_window, desiredHeight_window, windowsErrorMessage.c_str() );
					Engine::Logging::OutputError( "Windows failed to resize main window to &i x &i"
						" (based on a desired resolution of %u x %u): %s",
						desiredWidth_window, desiredHeight_window, i_resolutionWidth, i_resolutionHeight, windowsErrorMessage.c_str() );
					goto OnError;
				}
			}
		}

		return true;

	OnError:

		if ( o_window != NULL )
		{
			FreeMainWindow( o_window );
		}

		return false;
	}

	bool CreateMainWindowClass( const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_mainWindowClassName,
		WNDPROC fOnMessageReceivedFromWindows, ATOM& o_mainWindowClass,
		const WORD* const i_iconId_large, const WORD* const i_iconId_small )
	{
		WNDCLASSEX wndClassEx = { 0 };
		{
			wndClassEx.cbSize = sizeof( WNDCLASSEX );
			wndClassEx.hInstance = i_thisInstanceOfTheApplication;
			wndClassEx.style = 0;
			wndClassEx.lpfnWndProc = fOnMessageReceivedFromWindows;
			wndClassEx.cbClsExtra = 0;
			wndClassEx.cbWndExtra = 0;
			if ( i_iconId_large )
			{
				wndClassEx.hIcon = LoadIcon( i_thisInstanceOfTheApplication, MAKEINTRESOURCE( *i_iconId_large ) );
			}
			else
			{
				wndClassEx.hIcon = LoadIcon( NULL, IDI_APPLICATION );
			}
			if ( i_iconId_small != 0 )
			{
				wndClassEx.hIconSm = LoadIcon( i_thisInstanceOfTheApplication, MAKEINTRESOURCE( *i_iconId_small ) );
			}
			else
			{
				wndClassEx.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
			}
			wndClassEx.hCursor = LoadCursor( NULL, IDC_ARROW );
			wndClassEx.hbrBackground = reinterpret_cast<HBRUSH>( IntToPtr( COLOR_BACKGROUND + 1 ) );
			wndClassEx.lpszMenuName = NULL;
			wndClassEx.lpszClassName = i_mainWindowClassName;
		}
		o_mainWindowClass = RegisterClassEx( &wndClassEx );
		if ( o_mainWindowClass != NULL )
		{
			Engine::Logging::OutputMessage( "Registered main window class \"%s\"", i_mainWindowClassName );
			return true;
		}
		else
		{
			const std::string windowsErrorMessage = Engine::Windows::GetLastSystemError();
			ASSERTF( false, "Main window class registration failed: %s", windowsErrorMessage.c_str() );
			Engine::Logging::OutputError( "Windows failed to register the main window class: %s", windowsErrorMessage.c_str() );
			return false;
		}
	}

	bool FreeMainWindow( HWND& io_window )
	{
		if ( DestroyWindow( io_window ) != FALSE )
		{
			Engine::Logging::OutputMessage( "Destroyed main window" );
			ASSERT( io_window == NULL );
			io_window = NULL;
			return true;
		}
		else
		{
			const std::string windowsErrorMessage = Engine::Windows::GetLastSystemError();
			ASSERTF( false, "Main window wasn't destroyed: %s", windowsErrorMessage.c_str() );
			Engine::Logging::OutputError( "Windows failed to destroy the main window: %s", windowsErrorMessage.c_str() );
			return false;
		}
	}

	bool FreeMainWindowClass( const HINSTANCE i_thisInstanceOfTheApplication, ATOM& io_mainWindowClass  )
	{
		if ( UnregisterClass( MAKEINTATOM( io_mainWindowClass ), i_thisInstanceOfTheApplication ) != FALSE )
		{
			Engine::Logging::OutputMessage( "Unregistered main window class" );
			io_mainWindowClass = NULL;
			return true;
		}
		else
		{
			const std::string windowsErrorMessage = Engine::Windows::GetLastSystemError();
			ASSERTF( false, "Main window class wasn't unregistered: %s", windowsErrorMessage.c_str() );
			Engine::Logging::OutputError( "Windows failed to unregister the main window class: %s", windowsErrorMessage.c_str() );
			return false;
		}
	}
}
