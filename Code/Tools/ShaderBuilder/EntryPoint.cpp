#include "cShaderBuilder.h"

int main( int i_argumentCount, char** i_arguments )
{
	return Engine::AssetBuild::Build<Engine::AssetBuild::cShaderBuilder>( i_arguments, i_argumentCount );
}
