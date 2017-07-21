#ifndef MATERIALBUILDER_H
#define MATERIALBUILDER_H

#include "../AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

namespace Engine
{
	namespace AssetBuild
	{
		class cMaterialBuilder : public cbBuilder
		{
		private:
			virtual bool Build(const std::vector<std::string>& i_arguments);
		};
	}
}

#endif