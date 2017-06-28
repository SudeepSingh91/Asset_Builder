#ifndef EAE6320_CONTEXT_H
#define EAE6320_CONTEXT_H

#if defined( EAE6320_PLATFORM_D3D )
	#include <D3D11.h>
#endif

#include "../Platform/Platform.h"

class Context
{
public:
	~Context();
	static Context* GetContext();

#if defined( EAE6320_PLATFORM_D3D )
	ID3D11InputLayout* s_vertexLayout = NULL;
	ID3D11Device* s_direct3dDevice = NULL;
	eae6320::Platform::sDataFromFile* compiledVertexShader;
	ID3D11DeviceContext* s_direct3dImmediateContext = NULL;

#endif

private:
	Context();

	static Context* myContext;
};

#endif