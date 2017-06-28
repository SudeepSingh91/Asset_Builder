#ifndef EAE6320_ASSETBUILD_H
#define EAE6320_ASSETBUILD_H

namespace eae6320
{
	namespace AssetBuild
	{
		bool BuildAssets( const char* const i_path_assetsToBuild );
		bool Initialize();
		bool CleanUp();
	}
}

#endif	
