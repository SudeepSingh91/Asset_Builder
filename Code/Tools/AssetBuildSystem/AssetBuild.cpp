#include "AssetBuild.h"
#include <iostream>
#include <string>
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"

namespace
{
	lua_State* s_luaState = NULL;
}

namespace
{
	int luaCopyFile( lua_State* io_luaState );
	int luaCreateDirectoryIfNecessary( lua_State* io_luaState );
	int luaDoesFileExist( lua_State* io_luaState );
	int luaExecuteCommand( lua_State* io_luaState );
	int luaGetEnvironmentVariable( lua_State* io_luaState );
	int luaGetLastWriteTime( lua_State* io_luaState );
	int luaInvalidateLastWriteTime( lua_State* io_luaState );
	int luaOutputErrorMessage( lua_State* io_luaState );
}

bool Engine::AssetBuild::BuildAssets( const char* const i_path_assetsToBuild )
{
	{
		std::string path;
		{
			std::string errorMessage;
			if ( !GetAssetBuildSystemPath( path, &errorMessage ) )
			{
				OutputErrorMessage( errorMessage.c_str(), __FILE__ );
				return false;
			}
		}
		{
			std::string errorMessage;
			if ( Platform::DoesFileExist( path.c_str(), &errorMessage ) )
			{
				const int luaResult = luaL_loadfile( s_luaState, path.c_str() );
				if ( luaResult == LUA_OK )
				{
					const int argumentCount = 1;
					{
						lua_pushstring( s_luaState, i_path_assetsToBuild );
					}
					const int returnValueCount = 0;
					const int noErrorHandler = 0;
					const int luaResult = lua_pcall( s_luaState, argumentCount, returnValueCount, noErrorHandler );
					if ( luaResult != LUA_OK )
					{
						std::cerr << lua_tostring( s_luaState, -1 ) << "\n";
						lua_pop( s_luaState, 1 );
						return false;
					}
				}
				else
				{
					std::cerr << lua_tostring( s_luaState, -1 ) << "\n";
					lua_pop( s_luaState, 1 );
					return false;
				}
			}
			else
			{
				OutputErrorMessage( errorMessage.c_str(), path.c_str() );
				return false;
			}
		}
	}

	return true;
}

bool Engine::AssetBuild::Initialize()
{
	{
		s_luaState = luaL_newstate();
		if ( !s_luaState )
		{
			OutputErrorMessage( "Memory allocation error creating Lua state", __FILE__ );
			return false;
		}
	}
	luaL_openlibs( s_luaState );
	{
		lua_register( s_luaState, "CopyFile", luaCopyFile );
		lua_register( s_luaState, "CreateDirectoryIfNecessary", luaCreateDirectoryIfNecessary );
		lua_register( s_luaState, "DoesFileExist", luaDoesFileExist );
		lua_register( s_luaState, "ExecuteCommand", luaExecuteCommand );
		lua_register( s_luaState, "GetEnvironmentVariable", luaGetEnvironmentVariable );
		lua_register( s_luaState, "GetLastWriteTime", luaGetLastWriteTime );
		lua_register( s_luaState, "InvalidateLastWriteTime", luaInvalidateLastWriteTime );
		lua_register( s_luaState, "OutputErrorMessage", luaOutputErrorMessage );
	}

	return true;
}

bool Engine::AssetBuild::CleanUp()
{
	bool wereThereErrors = false;

	if ( s_luaState )
	{
		ASSERT( lua_gettop( s_luaState ) == 0 );
		lua_close( s_luaState );
		s_luaState = NULL;
	}

	return !wereThereErrors;
}

namespace
{
	int luaCopyFile( lua_State* io_luaState )
	{
		const char* i_path_source;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path_source = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}
		const char* i_path_target;
		if ( lua_isstring( io_luaState, 2 ) )
		{
			i_path_target = lua_tostring( io_luaState, 2 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #2 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 2 ) );
		}

		{
			std::string errorMessage;
			const bool noErrorIfTargetAlreadyExists = false;
			const bool updateTheTargetFileTime = true;
			if ( Engine::Platform::CopyFile( i_path_source, i_path_target, noErrorIfTargetAlreadyExists, updateTheTargetFileTime, &errorMessage ) )
			{
				lua_pushboolean( io_luaState, true );
				const int returnValueCount = 1;
				return returnValueCount;
			}
			else
			{
				lua_pushboolean( io_luaState, false );
				lua_pushstring( io_luaState, errorMessage.c_str() );
				const int returnValueCount = 2;
				return returnValueCount;
			}
		}
	}

	int luaCreateDirectoryIfNecessary( lua_State* io_luaState )
	{
		const char* i_path;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		std::string errorMessage;
		if ( Engine::Platform::CreateDirectoryIfNecessary( i_path, &errorMessage ) )
		{
			const int returnValueCount = 0;
			return returnValueCount;
		}
		else
		{
			return luaL_error( io_luaState, errorMessage.c_str() );
		}
	}

	int luaDoesFileExist( lua_State* io_luaState )
	{
		const char* i_path;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		std::string errorMessage;
		if ( Engine::Platform::DoesFileExist( i_path, &errorMessage ) )
		{
			lua_pushboolean( io_luaState, true );
			const int returnValueCount = 1;
			return returnValueCount;
		}
		else
		{
			lua_pushboolean( io_luaState, false );
			lua_pushstring( io_luaState, errorMessage.c_str() );
			const int returnValueCount = 2;
			return returnValueCount;
		}
	}

	int luaExecuteCommand( lua_State* io_luaState )
	{
		const char* i_command;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_command = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		int exitCode;
		std::string errorMessage;
		if ( Engine::Platform::ExecuteCommand( i_command, &exitCode, &errorMessage ) )
		{
			lua_pushboolean( io_luaState, true );
			lua_pushinteger( io_luaState, exitCode );
			const int returnValueCount = 2;
			return returnValueCount;
		}
		else
		{
			lua_pushboolean( io_luaState, false );
			lua_pushstring( io_luaState, errorMessage.c_str() );
			const int returnValueCount = 2;
			return returnValueCount;
		}
	}

	int luaGetEnvironmentVariable( lua_State* io_luaState )
	{
		const char* i_key;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_key = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		std::string value;
		std::string errorMessage;
		if ( Engine::Platform::GetEnvironmentVariable( i_key, value, &errorMessage ) )
		{
			lua_pushstring( io_luaState, value.c_str() );
			const int returnValueCount = 1;
			return returnValueCount;
		}
		else
		{
			lua_pushboolean( io_luaState, false );
			lua_pushstring( io_luaState, errorMessage.c_str() );
			const int returnValueCount = 2;
			return returnValueCount;
		}
	}

	int luaGetLastWriteTime( lua_State* io_luaState )
	{
		const char* i_path;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}
		uint64_t lastWriteTime;
		std::string errorMessage;
		if ( Engine::Platform::GetLastWriteTime( i_path, lastWriteTime, &errorMessage ) )
		{
			lua_pushnumber( io_luaState, static_cast<lua_Number>( lastWriteTime ) );
			const int returnValueCount = 1;
			return returnValueCount;
		}
		else
		{
			return luaL_error( io_luaState, errorMessage.c_str() );
		}
	}

	int luaInvalidateLastWriteTime( lua_State* io_luaState )
	{
		const char* i_path;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		std::string errorMessage;
		if ( Engine::Platform::InvalidateLastWriteTime( i_path, &errorMessage ) )
		{
			const int returnValueCount = 0;
			return returnValueCount;
		}
		else
		{
			return luaL_error( io_luaState, errorMessage.c_str() );
		}
	}

	int luaOutputErrorMessage( lua_State* io_luaState )
	{
		const char* i_errorMessage;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_errorMessage = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}
		const char* i_optionalFileName = NULL;
		if ( !lua_isnoneornil( io_luaState, 2 ) )
		{
			if ( lua_isstring( io_luaState, 2 ) )
			{
				i_optionalFileName = lua_tostring( io_luaState, 2 );
			}
			else
			{
				return luaL_error( io_luaState,
					"Argument #2 must be a string (instead of a %s)",
					luaL_typename( io_luaState, 2 ) );
			}
		}

		Engine::AssetBuild::OutputErrorMessage( i_errorMessage, i_optionalFileName );

		const int returnValueCount = 0;
		return returnValueCount;
	}
}
