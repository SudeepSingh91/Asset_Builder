#include "../cRenderState.h"

bool eae6320::Graphics::cRenderState::Initialize( const uint8_t i_renderStateBits )
{
	m_bits = i_renderStateBits;
	return InitializeFromBits();
}

eae6320::Graphics::cRenderState::cRenderState()
	:
#if defined( EAE6320_PLATFORM_D3D )
	m_blendState( NULL ), m_depthStencilState( NULL ), m_rasterizerState( NULL ),
#endif
	m_bits( 0xff )
{

}

eae6320::Graphics::cRenderState::~cRenderState()
{
	CleanUp();
}
