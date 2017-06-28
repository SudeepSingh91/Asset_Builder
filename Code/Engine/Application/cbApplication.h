#ifndef EAE6320_APPLICATION_CBAPPLICATION_H
#define EAE6320_APPLICATION_CBAPPLICATION_H

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "../Windows/Includes.h"
#endif

namespace eae6320
{
	namespace Graphics
	{
		struct sInitializationParameters;
	}
	namespace UserOutput
	{
		struct sInitializationParameters;
	}
}

namespace eae6320
{
	namespace Application
	{
		template<class tApplication>
		int Run(
#if defined( EAE6320_PLATFORM_WINDOWS )
			 const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_commandLineArguments, const int i_initialWindowDisplayState
#endif
		)
		{
			tApplication::sEntryPointParameters entryPointParameters =
			{
#if defined( EAE6320_PLATFORM_WINDOWS )
				i_thisInstanceOfTheApplication, i_commandLineArguments, i_initialWindowDisplayState
#endif
			};
			tApplication newApplicationInstance;
			return newApplicationInstance.ParseEntryPointParametersAndRun( entryPointParameters );
		}

		class cbApplication
		{
		public:
			struct sEntryPointParameters
			{
#if defined( EAE6320_PLATFORM_WINDOWS )
				const HINSTANCE applicationInstance;
				const char* const commandLineArguments;
				const int initialWindowDisplayState;
#endif
			};

			int ParseEntryPointParametersAndRun( const sEntryPointParameters& i_entryPointParameters );
			bool GetResolution( unsigned int& o_width, unsigned int& o_height ) const;
			cbApplication();
			virtual ~cbApplication() = 0;

		private:
			virtual const char* GetPathToLogTo() const { return NULL; }	

			virtual bool Initialize() = 0;
			virtual void DrawMesh() = 0;
			virtual bool CleanUp() = 0;

		private:

			void OnNewFrame();
			bool WaitForApplicationToFinish( int& o_exitCode );

			bool Initialize_all( const sEntryPointParameters& i_entryPointParameters );	
				bool Initialize_base( const sEntryPointParameters& i_entryPointParameters );
				bool Initialize_engine();

			bool PopulateGraphicsInitializationParameters( Graphics::sInitializationParameters& o_initializationParameters );
			bool PopulateUserOutputInitializationParameters( UserOutput::sInitializationParameters& o_initializationParameters );

			bool CleanUp_all();	
				bool CleanUp_base();	
				bool CleanUp_engine();	

#if defined( EAE6320_PLATFORM_WINDOWS )
			#include "Windows/cbApplication.win.h"
#endif
		};
	}
}

#endif	
