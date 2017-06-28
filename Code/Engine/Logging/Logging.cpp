#include "Logging.h"
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include "../Asserts/Asserts.h"

namespace
{
	bool OutputMessage( const char* const i_message, va_list io_insertions );
}

namespace
{
	class cLogging
	{
	public:
		bool OutputMessage( const std::string& i_message );
		void FlushLog();
		bool InitializeIfNecesary( const char* const i_pathToLogTo = NULL );
		static void CleanUp();
		~cLogging();

	private:

		std::ofstream m_outputStream;
	};
}

namespace
{
	std::string s_path_log;
	cLogging s_logger;
	bool s_hasTheLoggerBeenDestroyed = false;
	bool s_hasTheLogFileAlreadyBeenWrittenTo = false;
	bool s_cleanUpResult = false;
}

bool eae6320::Logging::OutputMessage( const char* const i_message, ... )
{
	bool result;
	{
		va_list insertions;
		va_start( insertions, i_message );
		result = ::OutputMessage( i_message, insertions );
		va_end( insertions );
	}
#ifdef EAE6320_LOGGING_FLUSHBUFFERAFTEREVERYMESSAGE
	s_logger.FlushLog();
#endif
	return result;
}

bool eae6320::Logging::OutputError( const char* const i_errorMessage, ... )
{
	bool result;
	{
		va_list insertions;
		va_start( insertions, i_errorMessage );
		result = ::OutputMessage( i_errorMessage, insertions );
		va_end( insertions );
	}
	s_logger.FlushLog();
	return result;
}

bool eae6320::Logging::Initialize( const char* const i_pathToLogTo )
{
	return s_logger.InitializeIfNecesary( i_pathToLogTo );
}

bool eae6320::Logging::CleanUp()
{
	cLogging::CleanUp();
	return s_cleanUpResult;
}

namespace
{
	bool OutputMessage( const char* const i_message, va_list io_insertions )
	{
		const size_t bufferSize = 512;
		char buffer[bufferSize];
		const int formattingResult = vsnprintf( buffer, bufferSize, i_message, io_insertions );
		if ( formattingResult >= 0 )
		{
			if ( formattingResult < bufferSize )
			{
				return s_logger.OutputMessage( buffer );
			}
			else
			{
				EAE6320_ASSERTF( false, "The internal logging buffer of size %u was not big enough to hold the formatted message of length %i",
					bufferSize, formattingResult + 1 );
				std::ostringstream errorMessage;
				errorMessage << "FORMATTING ERROR! (The internal logging buffer of size " << bufferSize
					<< " was not big enough to hold the formatted message of length " << ( formattingResult + 1 ) << ".)"
					" Cut-off message is:\n\t" << buffer;
				s_logger.OutputMessage( errorMessage.str().c_str() );
				return false;
			}
		}
		else
		{
			EAE6320_ASSERTF( false, "An encoding error occurred while logging the message \"%s\"", i_message );
			std::ostringstream errorMessage;
			errorMessage << "ENCODING ERROR! Unformatted message was:\n\t" << i_message;
			s_logger.OutputMessage( errorMessage.str().c_str() );
			return false;
		}
	}
}

namespace
{
	bool cLogging::OutputMessage( const std::string& i_message )
	{
		if ( InitializeIfNecesary() )
		{
			m_outputStream << i_message << "\n";

			return true;
		}
		else
		{
			return false;
		}
	}

	inline void cLogging::FlushLog()
	{
		m_outputStream.flush();
	}

	bool cLogging::InitializeIfNecesary( const char* const i_pathToLogTo )
	{
		if ( !s_hasTheLoggerBeenDestroyed )
		{
			if ( m_outputStream.is_open() )
			{
				if ( !i_pathToLogTo )
				{
					return true;
				}
				else
				{
					const bool arePathsEqual = s_path_log.compare( i_pathToLogTo ) == 0;
					EAE6320_ASSERTF( arePathsEqual, "The log file \"%s\" is already open,"
						" but an attempt is being made to initialize logging with the different path \"%s\"",
						s_path_log.c_str(), i_pathToLogTo );
					eae6320::Logging::OutputError( "Error! An attempt was made to initialize logging with the path \"%s\""
						" after the current log file had already been opened with this path", i_pathToLogTo );
					return arePathsEqual;
				}
			}
			else
			{
				if ( !s_hasTheLogFileAlreadyBeenWrittenTo )
				{
					if ( i_pathToLogTo )
					{
						if ( i_pathToLogTo[0] == '\0' )
						{
							EAE6320_ASSERT( "An attempt is being made to initialize logging with an empty path" );
							return false;
						}
						if ( !s_path_log.empty() )
						{
							const bool arePathsEqual = s_path_log.compare( i_pathToLogTo ) == 0;
							EAE6320_ASSERTF( arePathsEqual, "An attempt to initialize logging with a log file path of \"%s\" has already been made,"
								" but another attempt to initialize it is being made with the different path \"%s\"",
								s_path_log.c_str(), i_pathToLogTo );
						}
						s_path_log = i_pathToLogTo;
					}
					else
					{
						s_path_log = EAE6320_LOGGING_DEFAULTPATH;
					}
					EAE6320_ASSERTF( !s_path_log.empty(), "The path to log to is empty" );
					m_outputStream.open( s_path_log.c_str() );
					if ( m_outputStream.is_open() )
					{
						s_hasTheLogFileAlreadyBeenWrittenTo = true;
						eae6320::Logging::OutputMessage( "Opened log file \"%s\"", s_path_log.c_str() );
						FlushLog();
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					if ( i_pathToLogTo )
					{
						const bool arePathsEqual = s_path_log.compare( i_pathToLogTo ) == 0;
						EAE6320_ASSERTF( arePathsEqual, "The log file \"%s\" has already been written to,"
							" but an attempt is being made to initialize logging with the different path \"%s\"",
							s_path_log.c_str(), i_pathToLogTo );
						if ( !arePathsEqual )
						{
							return false;
						}
					}
					m_outputStream.open( s_path_log.c_str(), std::ofstream::app );
					const bool result = m_outputStream.is_open();
					EAE6320_ASSERT( result );
					if ( result )
					{
						eae6320::Logging::OutputMessage( "Re-opened log file" );
						FlushLog();
					}
					return result;
				}
			}
		}
		else
		{
			{
				new ( this ) cLogging;
				{
					int result = atexit( CleanUp );
					if ( result == 0 )
					{
						s_hasTheLoggerBeenDestroyed = false;
					}
					else
					{
						EAE6320_ASSERTF( false, "Calling atexit() to register logging clean up on a revived logger failed with a return value of %i", result );
						CleanUp();
						return false;
					}
				}
			}
			m_outputStream.open( s_path_log.c_str(), std::ofstream::app );
			const bool result = m_outputStream.is_open();
			EAE6320_ASSERT( result );
			if ( result )
			{
				eae6320::Logging::OutputMessage( "Re-opened log file after it had been destroyed" );
				FlushLog();
			}
			return result;
		}
	}

	void cLogging::CleanUp()
	{
		if ( s_logger.m_outputStream.is_open() )
		{
			{
				eae6320::Logging::OutputMessage( "Closing log file" );
				s_logger.FlushLog();
			}
			s_logger.m_outputStream.close();
			s_cleanUpResult = !s_logger.m_outputStream.is_open();
			EAE6320_ASSERTF( s_cleanUpResult, "Log file wasn't closed" );
			if ( !s_cleanUpResult )
			{
				eae6320::Logging::OutputError( "Error: Log file did not close" );
			}
			return;
		}

		s_cleanUpResult = true;
	}

	cLogging::~cLogging()
	{
		s_hasTheLoggerBeenDestroyed = true;
		CleanUp();
	}
}
