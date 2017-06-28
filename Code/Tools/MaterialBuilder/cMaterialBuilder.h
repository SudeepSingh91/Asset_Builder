#ifndef EAE6320_MATERIALBUILDER_H
#define EAE6320_MATERIALBUILDER_H

#include "../AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

namespace eae6320
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