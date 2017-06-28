#include "../Time.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Windows/Includes.h"
#include "../../Windows/Functions.h"

namespace
{
	bool s_isInitialized = false;

	double s_secondsPerTick = 0.0;
	LARGE_INTEGER s_totalTicksElapsed_atInitializion = { 0 };
	LARGE_INTEGER s_totalTicksElapsed_duringRun = { 0 };
	LARGE_INTEGER s_totalTicksElapsed_previousFrame = { 0 };
}

namespace
{
	bool InitializeIfNecessary();
}

float eae6320::Time::GetElapsedSecondCount_total()
{
	InitializeIfNecessary();
	return static_cast<float>( static_cast<double>( s_totalTicksElapsed_duringRun.QuadPart ) * s_secondsPerTick );
}

float eae6320::Time::GetElapsedSecondCount_duringPreviousFrame()
{
	InitializeIfNecessary();
	return static_cast<float>(
		static_cast<double>( s_totalTicksElapsed_duringRun.QuadPart - s_totalTicksElapsed_previousFrame.QuadPart )
		* s_secondsPerTick );
}

void eae6320::Time::OnNewFrame()
{
	InitializeIfNecessary();

	{
		s_totalTicksElapsed_previousFrame = s_totalTicksElapsed_duringRun;
	}
	{
		LARGE_INTEGER totalCountsElapsed;
		const BOOL result = QueryPerformanceCounter( &totalCountsElapsed );
		EAE6320_ASSERTF( result != FALSE, "QueryPerformanceCounter() failed" );
		s_totalTicksElapsed_duringRun.QuadPart = totalCountsElapsed.QuadPart - s_totalTicksElapsed_atInitializion.QuadPart;
	}
}

bool eae6320::Time::Initialize()
{
	bool wereThereErrors = false;

	if ( !s_isInitialized )
	{
		{
			LARGE_INTEGER ticksPerSecond;
			if ( QueryPerformanceFrequency( &ticksPerSecond ) != FALSE )
			{
				if ( ticksPerSecond.QuadPart != 0 )
				{
					s_secondsPerTick = 1.0 / static_cast<double>( ticksPerSecond.QuadPart );
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERT( false );
					Logging::OutputMessage( "This hardware doesn't support high resolution performance counters!" );
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				const std::string windowsErrorMessage = Windows::GetLastSystemError();
				EAE6320_ASSERTF( false, windowsErrorMessage.c_str() );
				Logging::OutputMessage( "Windows failed to query performance frequency: %s", windowsErrorMessage.c_str() );
				goto OnExit;
			}
		}
		if ( QueryPerformanceCounter( &s_totalTicksElapsed_atInitializion ) == FALSE )
		{
			wereThereErrors = true;
			const std::string windowsErrorMessage = Windows::GetLastSystemError();
			EAE6320_ASSERTF( false, windowsErrorMessage.c_str() );
			Logging::OutputMessage( "Windows failed to query the performance counter: %s", windowsErrorMessage.c_str() );
			goto OnExit;
		}

		Logging::OutputMessage( "Initialized time" );
		s_isInitialized = true;
	}
	else
	{
		EAE6320_ASSERTF( false, "Time has already been initialized" );
		goto OnExit;
	}

OnExit:

	return !wereThereErrors;
}

bool eae6320::Time::CleanUp()
{
	return true;
}

namespace
{
	bool InitializeIfNecessary()
	{
		EAE6320_ASSERTF( s_isInitialized, "Time being used but was never explicitly initialized" );
		return s_isInitialized ? true : eae6320::Time::Initialize();
	}
}
