#ifndef CMESHBUILDER_H
#define CMESHBUILDER_H

#include "../AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

namespace Engine
{
	namespace AssetBuild
	{
		class cMeshBuilder : public cbBuilder
		{
		private:
			virtual bool Build( const std::vector<std::string>& i_arguments );
		};
	}
}

#endif