#include "cbApplication.h"
#include <cstdlib>
#include "../Asserts/Asserts.h"
#include "../Graphics/Graphics.h"
#include "../Logging/Logging.h"
#include "../Time/Time.h"
#include "../UserOutput/UserOutput.h"

int eae6320::Application::cbApplication::ParseEntryPointParametersAndRun( const sEntryPointParameters& i_entryPointParameters )
{
	int exitCode = EXIT_SUCCESS;

	if ( Initialize_all( i_entryPointParameters ) )
	{
		Logging::OutputMessage( "The application was successfully initialized" );
	}
	else
	{
		exitCode = EXIT_FAILURE;
		EAE6320_ASSERT( false );
		Logging::OutputError( "Application initialization failed!" );
		UserOutput::Print( "Initialization failed! (Check the log file for details.) This program will now exit." );
		goto OnExit;
	}

	if ( !WaitForApplicationToFinish( exitCode ) )
	{
		EAE6320_ASSERT( false );
		UserOutput::Print( "The application encountered an error and will now exit" );
		goto OnExit;
	}

OnExit:

	if ( !CleanUp_all() )
	{
		EAE6320_ASSERT( false );
		if ( exitCode == EXIT_SUCCESS )
		{
			exitCode = EXIT_FAILURE;
		}
	}

	return exitCode;
}

bool eae6320::Application::cbApplication::Initialize_all( const sEntryPointParameters& i_entryPointParameters )
{
	if ( !Logging::Initialize( GetPathToLogTo() ) )
	{
		EAE6320_ASSERT( false );
		return false;
	}
	if ( !Initialize_base( i_entryPointParameters ) )
	{
		EAE6320_ASSERT( false );
		return false;
	}
	if ( !Initialize_engine() )
	{
		EAE6320_ASSERT( false );
		return false;	
	}
	if ( !Initialize() )
	{
		EAE6320_ASSERT( false );
		return false;
	}

	return true;
}

bool eae6320::Application::cbApplication::Initialize_engine()
{
	{
		UserOutput::sInitializationParameters initializationParameters;
		if ( PopulateUserOutputInitializationParameters( initializationParameters ) )
		{
			if ( !UserOutput::Initialize( initializationParameters ) )
			{
				EAE6320_ASSERT( false );
				return false;
			}
		}
		else
		{
			EAE6320_ASSERT( false );
			return false;
		}
	}
	if ( !Time::Initialize() )
	{
		EAE6320_ASSERT( false );
		return false;
	}
	{
		Graphics::sInitializationParameters initializationParameters;
		if ( PopulateGraphicsInitializationParameters( initializationParameters ) )
		{
			if ( !Graphics::Initialize( initializationParameters ) )
			{
				EAE6320_ASSERT( false );
				return false;
			}
		}
		else
		{
			EAE6320_ASSERT( false );
			return false;
		}
	}

	return true;
}

bool eae6320::Application::cbApplication::CleanUp_all()
{
	bool wereThereErrors = false;

	if ( !CleanUp() )
	{
		wereThereErrors = true;
		EAE6320_ASSERT( false );
	}
	if ( !CleanUp_engine() )
	{
		wereThereErrors = true;
		EAE6320_ASSERT( false );
	}
	if ( !CleanUp_base() )
	{
		wereThereErrors = true;
		EAE6320_ASSERT( false );
	}
	if ( !Logging::CleanUp() )
	{
		wereThereErrors = true;
		EAE6320_ASSERT( false );
	}

	return !wereThereErrors;
}

bool eae6320::Application::cbApplication::CleanUp_engine()
{
	bool wereThereErrors = false;

	if ( !Graphics::CleanUp() )
	{
		wereThereErrors = true;
		EAE6320_ASSERT( false );
	}
	if ( !Time::CleanUp() )
	{
		wereThereErrors = true;
		EAE6320_ASSERT( false );
	}
	if ( !UserOutput::CleanUp() )
	{
		wereThereErrors = true;
		EAE6320_ASSERT( false );
	}

	return !wereThereErrors;
}

eae6320::Application::cbApplication::~cbApplication()
{
	CleanUp_base();
}

void eae6320::Application::cbApplication::OnNewFrame()
{
	Time::OnNewFrame();
	DrawMesh();
	Graphics::RenderFrame();
}
