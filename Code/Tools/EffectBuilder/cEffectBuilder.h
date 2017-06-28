#ifndef EAE6320_CEFFECTBUILDER_H
#define EAE6320_CEFFECTBUILDER_H

#include "../AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

namespace eae6320
{
	namespace AssetBuild
	{
		class cEffectBuilder : public cbBuilder
		{
		private:
			virtual bool Build(const std::vector<std::string>& i_arguments);
		};
	}
}

#endif