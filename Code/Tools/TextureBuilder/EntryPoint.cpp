#include "cTextureBuilder.h"

int main( int i_argumentCount, char** i_arguments )
{
	return Engine::AssetBuild::Build<Engine::AssetBuild::cTextureBuilder>( i_arguments, i_argumentCount );
}
