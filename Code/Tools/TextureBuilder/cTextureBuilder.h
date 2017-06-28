#ifndef EAE6320_CTEXTUREBUILDER_H
#define EAE6320_CTEXTUREBUILDER_H

#include "../AssetBuildLibrary/cbBuilder.h"

namespace eae6320
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