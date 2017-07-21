#include "../cRenderState.h"

bool Engine::Graphics::cRenderState::Initialize( const uint8_t i_renderStateBits )
{
	m_bits = i_renderStateBits;
	return InitializeFromBits();
}

Engine::Graphics::cRenderState::cRenderState()
	:
#if defined( PLATFORM_D3D )
	m_blendState( NULL ), m_depthStencilState( NULL ), m_rasterizerState( NULL ),
#endif
	m_bits( 0xff )
{

}

Engine::Graphics::cRenderState::~cRenderState()
{
	CleanUp();
}
