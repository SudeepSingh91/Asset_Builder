#include "UserSettings.h"
#include <cmath>
#include <sstream>
#include <string>
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../Platform/Platform.h"
#include "../../External/Lua/Includes.h"

namespace
{
	unsigned int s_resolutionHeight = 512;
	unsigned int s_resolutionWidth = 512;

	const char* const s_userSettingsFileName = "settings.ini";
}

namespace
{
	bool InitializeIfNecessary();
	bool IsNumberAnInteger( const lua_Number i_number );
	bool LoadUserSettingsIntoLuaTable( lua_State& io_luaState, bool& o_doesUserSettingsFileExist );
	bool PopulateUserSettingsFromLuaTable( lua_State& io_luaState );
}

unsigned int eae6320::UserSettings::GetResolutionHeight()
{
	InitializeIfNecessary();
	return s_resolutionHeight;
}

unsigned int eae6320::UserSettings::GetResolutionWidth()
{
	InitializeIfNecessary();
	return s_resolutionWidth;
}

namespace
{
	bool InitializeIfNecessary()
	{
		static bool isInitialized = false;
		if ( isInitialized )
		{
			return true;
		}

		bool wereThereErrors = false;

		lua_State* luaState = NULL;
		bool wasUserSettingsEnvironmentCreated = false;
		{
			luaState = luaL_newstate();
			if ( luaState == NULL )
			{
				EAE6320_ASSERTF( false, "Failed to create a new Lua state" );
				eae6320::Logging::OutputError( "Failed to create a new Lua state for the user settings" );
				wereThereErrors = true;
				goto CleanUp;
			}
		}
		{
			lua_newtable( luaState );
			wasUserSettingsEnvironmentCreated = true;
		}
		bool doesUserSettingsFileExist;
		if ( LoadUserSettingsIntoLuaTable( *luaState, doesUserSettingsFileExist ) )
		{
			if ( doesUserSettingsFileExist )
			{
				if ( !PopulateUserSettingsFromLuaTable( *luaState ) )
				{
					wereThereErrors = true;
					goto CleanUp;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			goto CleanUp;
		}

	CleanUp:

		if ( luaState )
		{
			if ( wasUserSettingsEnvironmentCreated )
			{
				lua_pop( luaState, 1 );
			}
			EAE6320_ASSERTF( lua_gettop( luaState ) == 0, "Lua stack is inconsistent" );
			lua_close( luaState );
		}

		isInitialized = !wereThereErrors;
		return isInitialized;
	}

	bool IsNumberAnInteger( const lua_Number i_number )
	{
		lua_Number integralPart;
		lua_Number fractionalPart = modf( i_number, &integralPart );
		return integralPart == i_number;
	}

	bool LoadUserSettingsIntoLuaTable( lua_State& io_luaState, bool& o_doesUserSettingsFileExist )
	{
		if ( eae6320::Platform::DoesFileExist( s_userSettingsFileName ) )
		{
			int result = luaL_loadfile( &io_luaState, s_userSettingsFileName );
			if ( result == LUA_OK )
			{
				o_doesUserSettingsFileExist = true;
				{
					lua_pushvalue( &io_luaState, -2 );
					const char* upValueName = lua_setupvalue( &io_luaState, -2, 1 );
					if ( upValueName == NULL )
					{
						EAE6320_ASSERT( false );
						eae6320::Logging::OutputError( "Internal error setting the Lua environment for the user settings file \"%s\"!"
							" This should never happen", s_userSettingsFileName );
						lua_pop( &io_luaState, 2 );
						return false;
					}
				}
				{
					int noArguments = 0;
					int noReturnValues = 0;
					int noErrorMessageHandler = 0;
					result = lua_pcall( &io_luaState, noArguments, noReturnValues, noErrorMessageHandler );
					if ( result == LUA_OK )
					{
						return true;
					}
					else
					{
						std::string luaErrorMessage( lua_tostring( &io_luaState, -1 ) );
						lua_pop( &io_luaState, 1 );

						EAE6320_ASSERTF( false, "User settings file error: %s", luaErrorMessage.c_str() );
						if ( result == LUA_ERRRUN )
						{
							eae6320::Logging::OutputError( "Error in the user settings file \"%s\": %s",
								s_userSettingsFileName, luaErrorMessage );
						}
						else
						{
							eae6320::Logging::OutputError( "Error processing the user settings file \"%s\": %s",
								s_userSettingsFileName, luaErrorMessage );
						}

						return false;
					}
				}
			}
			else
			{
				o_doesUserSettingsFileExist = false;

				std::string luaErrorMessage( lua_tostring( &io_luaState, -1 ) );
				lua_pop( &io_luaState, 1 );

				if ( result == LUA_ERRFILE )
				{
					EAE6320_ASSERTF( "Error opening or reading user settings file: %s", luaErrorMessage.c_str() );
					eae6320::Logging::OutputError( "Error opening or reading the user settings file \"%s\" even though it exists: %s",
						s_userSettingsFileName, luaErrorMessage.c_str() );
				}
				else if ( result == LUA_ERRSYNTAX )
				{
					EAE6320_ASSERTF( "Syntax error in user settings file: %s", luaErrorMessage.c_str() );
					eae6320::Logging::OutputError( "Syntax error in the user settings file \"%s\": %s",
						s_userSettingsFileName, luaErrorMessage.c_str() );
				}
				else
				{
					EAE6320_ASSERTF( "Error loading user settings file: %s", luaErrorMessage.c_str() );
					eae6320::Logging::OutputError( "Error loading the user settings file \"%s\": %s",
						s_userSettingsFileName, luaErrorMessage.c_str() );
				}

				return false;
			}
		}
		else
		{
			o_doesUserSettingsFileExist = false;
			eae6320::Logging::OutputMessage( "The user settings file \"%s\" doesn't exist. Using default settings instead.",
				s_userSettingsFileName );
			return true;
		}
	}

	bool PopulateUserSettingsFromLuaTable( lua_State& io_luaState )
	{
		{
			const char* key_width = "resolutionWidth";

			lua_pushstring( &io_luaState, key_width );
			lua_gettable( &io_luaState, -2 );
			if ( lua_isnumber( &io_luaState, -1 ) )
			{
				lua_Number floatingPointResult = lua_tonumber( &io_luaState, -1 );
				if ( IsNumberAnInteger( floatingPointResult ) )
				{
					if ( floatingPointResult >= lua_Number( 0 ) )
					{
						s_resolutionWidth = static_cast<unsigned int>( floatingPointResult + 0.5f );
						eae6320::Logging::OutputMessage("The resolution width of the game is %u", s_resolutionWidth);
					}
					else
					{
						eae6320::Logging::OutputError( "The user settings file %s specifies a negative resolution width of %f. Using default %u instead",
							s_userSettingsFileName, floatingPointResult, s_resolutionWidth );
					}
				}
			}
			lua_pop( &io_luaState, 1 );
		}
		{
			const char* key_height = "resolutionHeight";

			lua_pushstring( &io_luaState, key_height );
			lua_gettable( &io_luaState, -2 );
			if ( lua_isnumber( &io_luaState, -1 ) )
			{
				lua_Number floatingPointResult = lua_tonumber( &io_luaState, -1 );
				if ( IsNumberAnInteger( floatingPointResult ) )
				{
					if ( floatingPointResult >= lua_Number( 0 ) )
					{
						s_resolutionHeight = static_cast<unsigned int>( floatingPointResult + 0.5f );
						eae6320::Logging::OutputMessage("The resolution height of the game is %u", s_resolutionHeight);
					}
					else
					{
						eae6320::Logging::OutputError( "The user settings file %s specifies a negative resolution height of %f. Using default %u instead",
							s_userSettingsFileName, floatingPointResult, s_resolutionHeight );
					}
				}
			}
			lua_pop( &io_luaState, 1 );
		}

		return true;
	}
}
