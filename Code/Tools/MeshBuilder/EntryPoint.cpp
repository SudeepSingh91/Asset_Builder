#include "cMeshBuilder.h"

int main( int i_argumentCount, char** i_arguments )
{
	return Engine::AssetBuild::Build<Engine::AssetBuild::cMeshBuilder>( i_arguments, i_argumentCount );
}
