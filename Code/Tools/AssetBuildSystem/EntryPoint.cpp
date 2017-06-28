#include <cstdlib>
#include "AssetBuild.h"
#include <sstream>
#include "../AssetBuildLibrary/UtilityFunctions.h"

int main( int i_argumentCount, char** i_arguments )
{
	bool wereThereErrors = false;

	if ( !eae6320::AssetBuild::Initialize() )
	{
		wereThereErrors = true;
		goto OnExit;
	}

	if ( i_argumentCount == 2 )
	{
		const char* const path_assetsToBuild = i_arguments[1];
		if ( !eae6320::AssetBuild::BuildAssets( path_assetsToBuild ) )
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "AssetBuildSystem.exe must be run with a single command line argument which is the path to the list of assets to build"
			" (the invalid argument count being passed to main is " << i_argumentCount << ")";
		eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str() );
	}

OnExit:

	if ( !eae6320::AssetBuild::CleanUp() )
	{
		wereThereErrors = true;
	}

	if ( !wereThereErrors )
	{
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
}
