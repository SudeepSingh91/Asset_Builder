#include "../UserOutput.h"
#include <cstdarg>
#include <cstdio>
#include <sstream>
#include <string>
#include "../../Asserts/Asserts.h"

namespace
{
	HWND s_mainWindow = NULL;
}

void eae6320::UserOutput::Print( const char* const i_message, ... )
{
	std::string message;
	{
		const size_t bufferSize = 512;
		static char buffer[bufferSize];
		int formattingResult;
		{
			va_list insertions;
			va_start( insertions, i_message );
			formattingResult = vsnprintf( buffer, bufferSize, i_message, insertions );
			va_end( insertions );
		}
		if ( formattingResult >= 0 )
		{
			if ( formattingResult < bufferSize )
			{
				message = buffer;
			}
			else
			{
				EAE6320_ASSERTF( false, "The internal UserOutput buffer of size %u was not big enough to hold the formatted message of length %i",
					bufferSize, formattingResult + 1 );
				std::ostringstream errorMessage;
				errorMessage << "FORMATTING ERROR! (The internal user output buffer of size " << bufferSize
					<< " was not big enough to hold the formatted message of length " << ( formattingResult + 1 ) << ".)"
					" Cut-off message is:\n\t" << buffer;
				message = errorMessage.str();
			}
		}
		else
		{
			EAE6320_ASSERTF( false, "An encoding error occurred in UserOutput for the message \"%s\"", i_message );
			std::ostringstream errorMessage;
			errorMessage << "ENCODING ERROR! Unformatted message was:\n\t" << i_message;
			message = errorMessage.str();
		}
	}

	const char* const caption = "Message to User";
	MessageBox( s_mainWindow, message.c_str(), caption, MB_OK | MB_ICONINFORMATION );
}

bool eae6320::UserOutput::Initialize( const sInitializationParameters& i_initializationParameters )
{
	s_mainWindow = i_initializationParameters.mainWindow;

	return true;
}

bool eae6320::UserOutput::CleanUp()
{
	s_mainWindow = NULL;

	return true;
}
