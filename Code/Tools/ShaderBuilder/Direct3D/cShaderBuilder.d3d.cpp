#include "../cShaderBuilder.h"
#include <sstream>
#include "../../AssetBuildLibrary/UtilityFunctions.h"
#include "../../../Engine/Platform/Platform.h"

bool Engine::AssetBuild::cShaderBuilder::Build( const Graphics::ShaderTypes::eShaderType i_shaderType, const std::vector<std::string>& i_arguments )
{
	std::string path_fxc;
	{
		std::string path_sdk;
		{
			std::string errorMessage;
			if ( !Platform::GetEnvironmentVariable( "DXSDK_DIR", path_sdk, &errorMessage ) )
			{
				std::ostringstream decoratedErrorMessage;
				decoratedErrorMessage << "Failed to get the path to the DirectX SDK: " << errorMessage;
				OutputErrorMessage( decoratedErrorMessage.str().c_str(), __FILE__ );
				return false;
			}
		}
		path_fxc = path_sdk + "Utilities/bin/" +
#ifndef _WIN64
			"x86"
#else
			"x64"
#endif
			+ "/fxc.exe";
	}
	std::string command;
	{
		std::ostringstream commandToBuild;
		commandToBuild << "\"" << path_fxc << "\"";
		switch ( i_shaderType )
		{
		case Graphics::ShaderTypes::Vertex:
			commandToBuild << " /Tvs_4_0";
			break;
		case Graphics::ShaderTypes::Fragment:
			commandToBuild << " /Tps_4_0";
			break;
		}
		commandToBuild << " /Emain"
			<< " /DPLATFORM_D3D"
#ifdef GRAPHICS_AREDEBUGSHADERSENABLED
			<< " /Od"
			<< " /Zi"
#endif
			<< " /Fo\"" << m_path_target << "\""
			<< " /nologo"
			<< " \"" << m_path_source << "\""
		;
		command = commandToBuild.str();
	}
	{
		int exitCode;
		std::string errorMessage;
		if ( Platform::ExecuteCommand( command.c_str(), &exitCode, &errorMessage ) )
		{
			return exitCode == EXIT_SUCCESS;
		}
		else
		{
			OutputErrorMessage( errorMessage.c_str(), m_path_source );
			return false;
		}
	}
}
