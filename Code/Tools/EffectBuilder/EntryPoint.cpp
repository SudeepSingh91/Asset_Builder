#include "cEffectBuilder.h"

int main(int i_argumentCount, char** i_arguments)
{
	return Engine::AssetBuild::Build<Engine::AssetBuild::cEffectBuilder>(i_arguments, i_argumentCount);
}
