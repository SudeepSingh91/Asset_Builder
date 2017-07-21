#include "cbApplication.h"
#include <cstdlib>
#include "../Asserts/Asserts.h"
#include "../Graphics/Graphics.h"
#include "../Logging/Logging.h"
#include "../Time/Time.h"
#include "../UserOutput/UserOutput.h"

int Engine::Application::cbApplication::ParseEntryPointParametersAndRun( const sEntryPointParameters& i_entryPointParameters )
{
	int exitCode = EXIT_SUCCESS;

	if ( Initialize_all( i_entryPointParameters ) )
	{
		Logging::OutputMessage( "The application was successfully initialized" );
	}
	else
	{
		exitCode = EXIT_FAILURE;
		ASSERT( false );
		Logging::OutputError( "Application initialization failed!" );
		UserOutput::Print( "Initialization failed! (Check the log file for details.) This program will now exit." );
		goto OnExit;
	}

	if ( !WaitForApplicationToFinish( exitCode ) )
	{
		ASSERT( false );
		UserOutput::Print( "The application encountered an error and will now exit" );
		goto OnExit;
	}

OnExit:

	if ( !CleanUp_all() )
	{
		ASSERT( false );
		if ( exitCode == EXIT_SUCCESS )
		{
			exitCode = EXIT_FAILURE;
		}
	}

	return exitCode;
}

bool Engine::Application::cbApplication::Initialize_all( const sEntryPointParameters& i_entryPointParameters )
{
	if ( !Logging::Initialize( GetPathToLogTo() ) )
	{
		ASSERT( false );
		return false;
	}
	if ( !Initialize_base( i_entryPointParameters ) )
	{
		ASSERT( false );
		return false;
	}
	if ( !Initialize_engine() )
	{
		ASSERT( false );
		return false;	
	}
	if ( !Initialize() )
	{
		ASSERT( false );
		return false;
	}

	return true;
}

bool Engine::Application::cbApplication::Initialize_engine()
{
	{
		UserOutput::sInitializationParameters initializationParameters;
		if ( PopulateUserOutputInitializationParameters( initializationParameters ) )
		{
			if ( !UserOutput::Initialize( initializationParameters ) )
			{
				ASSERT( false );
				return false;
			}
		}
		else
		{
			ASSERT( false );
			return false;
		}
	}
	if ( !Time::Initialize() )
	{
		ASSERT( false );
		return false;
	}
	{
		Graphics::GraphicsData* m_graphicsData = Graphics::GraphicsData::GetGraphicsInst();
		Graphics::sInitializationParameters initializationParameters;
		if ( PopulateGraphicsInitializationParameters( initializationParameters ) )
		{
			if ( !m_graphicsData->Initialize( initializationParameters ) )
			{
				ASSERT( false );
				return false;
			}
		}
		else
		{
			ASSERT( false );
			return false;
		}
	}

	return true;
}

bool Engine::Application::cbApplication::CleanUp_all()
{
	bool wereThereErrors = false;

	if ( !CleanUp() )
	{
		wereThereErrors = true;
		ASSERT( false );
	}
	if ( !CleanUp_engine() )
	{
		wereThereErrors = true;
		ASSERT( false );
	}
	if ( !CleanUp_base() )
	{
		wereThereErrors = true;
		ASSERT( false );
	}
	if ( !Logging::CleanUp() )
	{
		wereThereErrors = true;
		ASSERT( false );
	}

	return !wereThereErrors;
}

bool Engine::Application::cbApplication::CleanUp_engine()
{
	bool wereThereErrors = false;
	Graphics::GraphicsData* m_graphicsData = Graphics::GraphicsData::GetGraphicsInst();

	if ( !m_graphicsData->CleanUp() )
	{
		wereThereErrors = true;
		ASSERT( false );
	}
	if ( !Time::CleanUp() )
	{
		wereThereErrors = true;
		ASSERT( false );
	}
	if ( !UserOutput::CleanUp() )
	{
		wereThereErrors = true;
		ASSERT( false );
	}

	return !wereThereErrors;
}

Engine::Application::cbApplication::~cbApplication()
{
	CleanUp_base();
}

void Engine::Application::cbApplication::OnNewFrame()
{
	Time::OnNewFrame();
	DrawMesh();
}
