#ifndef CSHADERBUILDER_H
#define CSHADERBUILDER_H

#include "../AssetBuildLibrary/cbBuilder.h"

#include "../../Engine/Graphics/Configuration.h"

#include "../../Engine/Graphics/Effect.h"

namespace Engine
{
	namespace AssetBuild
	{
		class cShaderBuilder : public cbBuilder
		{

		private:

			virtual bool Build(const std::vector<std::string>& i_arguments);

		private:

			bool Build( const Graphics::ShaderTypes::eShaderType i_shaderType, const std::vector<std::string>& i_arguments );
		};
	}
}

#endif
