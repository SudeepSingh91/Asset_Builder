#include "Context.h"

Context* Context::myContext = 0;

Context* Context::GetContext()
{
	if (myContext == 0)
	{
		myContext = new Context;
#if defined( EAE6320_PLATFORM_D3D )
		myContext->compiledVertexShader = new eae6320::Platform::sDataFromFile;
#endif
	}
	return myContext;
}

Context::~Context()
{
	
}

Context::Context()
{

}