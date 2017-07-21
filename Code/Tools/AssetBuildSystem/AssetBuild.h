#ifndef ASSETBUILD_H
#define ASSETBUILD_H

namespace Engine
{
	namespace AssetBuild
	{
		bool BuildAssets( const char* const i_path_assetsToBuild );
		bool Initialize();
		bool CleanUp();
	}
}

#endif	
