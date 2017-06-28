/*
	This class builds shaders
*/

#ifndef EAE6320_CSHADERBUILDER_H
#define EAE6320_CSHADERBUILDER_H

// Header Files
//=============

#include "../AssetBuildLibrary/cbBuilder.h"

#include "../../Engine/Graphics/Configuration.h"

#include "../../Engine/Graphics/Effect.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace AssetBuild
	{
		class cShaderBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			virtual bool Build( const std::vector<std::string>& i_arguments );

			// Implementation
			//===============

		private:

			// Build
			//------

			bool Build( const Graphics::ShaderTypes::eShaderType i_shaderType, const std::vector<std::string>& i_arguments );
		};
	}
}

#endif	// EAE6320_CSHADERBUILDER_H
