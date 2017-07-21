#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef PLATFORM_WINDOWS
	#include "../Windows/Includes.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <string>

namespace Engine
{
	namespace Platform
	{
		struct sDataFromFile
		{
			void* data;
			size_t size;

			void Free()
			{
				if ( data != NULL )
				{
					free( data );
					data = NULL;
				}
			}

			sDataFromFile() : data( NULL ), size( 0 ) {}
		};

		bool CopyFile( const char* const i_path_source, const char* i_path_target,
			const bool i_shouldFunctionFailIfTargetAlreadyExists = false, const bool i_shouldTargetFileTimeBeModified = false,
			std::string* o_errorMessage = NULL );
		bool CreateDirectoryIfNecessary( const std::string& i_path, std::string* const o_errorMessage = NULL );
		bool DoesFileExist( const char* const i_path, std::string* const o_errorMessage = NULL );
		bool ExecuteCommand( const char* const i_command, int* const o_exitCode = NULL, std::string* const o_errorMessage = NULL );
		bool GetEnvironmentVariable( const char* const i_key, std::string& o_value, std::string* const o_errorMessage = NULL );
		bool GetLastWriteTime( const char* const i_path, uint64_t& o_lastWriteTime, std::string* const o_errorMessage = NULL );
		bool InvalidateLastWriteTime( const char* const i_path, std::string* const o_errorMessage = NULL );
		bool LoadBinaryFile( const char* const i_path, sDataFromFile& o_data, std::string* const o_errorMessage = NULL );
		bool WriteBinaryFile( const char* const i_path, const void* const i_data, const size_t i_size, std::string* const o_errorMessage = NULL );
	}
}

#endif
