#ifndef CEFFECTBUILDER_H
#define CEFFECTBUILDER_H

#include "../AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

namespace Engine
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