#ifndef CTEXTUREBUILDER_H
#define CTEXTUREBUILDER_H

#include "../AssetBuildLibrary/cbBuilder.h"

namespace Engine
{
	namespace AssetBuild
	{
		class cTextureBuilder : public cbBuilder
		{
		private:
			virtual bool Build( const std::vector<std::string>& i_arguments );
		};
	}
}

#endif