#ifndef ASSETBUILD_UTILITYFUNCTIONS_H
#define ASSETBUILD_UTILITYFUNCTIONS_H

#include <string>

namespace Engine
{
	namespace AssetBuild
	{
		bool ConvertSourceRelativePathToBuiltRelativePath( const char* const i_sourceRelativePath, const char* const i_assetType,
			std::string& o_builtRelativePath, std::string* const o_errorMessage );

		bool GetAssetBuildSystemPath( std::string& o_path, std::string* const o_errorMessage );
		void OutputErrorMessage( const char* const i_errorMessage, const char* const i_optionalFileName = NULL );
	}
}

#endif