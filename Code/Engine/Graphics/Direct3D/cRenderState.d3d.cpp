#include "../cRenderState.h"
#include "../../Windows/Includes.h"
#include <D3D11.h>
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../Context.h"

namespace
{
	Context* myCont = Context::GetContext();
}

void Engine::Graphics::cRenderState::Bind() const
{
	{
		ASSERT( m_blendState != NULL );
		const float* const noBlendFactor = NULL;
		const unsigned int defaultSampleMask = 0xffffffff;
		myCont->s_direct3dImmediateContext->OMSetBlendState( m_blendState, noBlendFactor, defaultSampleMask );
	}
	{
		ASSERT( m_depthStencilState != NULL );
		const unsigned int unusedStencilReference = 0;
		myCont->s_direct3dImmediateContext->OMSetDepthStencilState( m_depthStencilState, unusedStencilReference );
	}
	{
		ASSERT( m_rasterizerState != NULL );
		myCont->s_direct3dImmediateContext->RSSetState( m_rasterizerState );
	}
}

bool Engine::Graphics::cRenderState::CleanUp()
{
	if ( m_blendState )
	{
		m_blendState->Release();
		m_blendState = NULL;
	}
	if ( m_depthStencilState )
	{
		m_depthStencilState->Release();
		m_depthStencilState = NULL;
	}
	if ( m_rasterizerState )
	{
		m_rasterizerState->Release();
		m_rasterizerState = NULL;
	}

	return true;
}

bool Engine::Graphics::cRenderState::InitializeFromBits()
{
	bool wereThereErrors = false;
	{
		D3D11_BLEND_DESC blendStateDescription = { 0 };
		blendStateDescription.AlphaToCoverageEnable = FALSE;
		blendStateDescription.IndependentBlendEnable = FALSE;
		{
			D3D11_RENDER_TARGET_BLEND_DESC& renderTargetBlendDescription = blendStateDescription.RenderTarget[0];
			if ( IsAlphaTransparencyEnabled() )
			{
				renderTargetBlendDescription.BlendEnable = TRUE;
				renderTargetBlendDescription.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				renderTargetBlendDescription.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				renderTargetBlendDescription.BlendOp = D3D11_BLEND_OP_ADD;
				renderTargetBlendDescription.SrcBlendAlpha = D3D11_BLEND_ONE;
				renderTargetBlendDescription.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
				renderTargetBlendDescription.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			}
			else
			{
				renderTargetBlendDescription.BlendEnable = FALSE;
				renderTargetBlendDescription.SrcBlend = D3D11_BLEND_ONE;
				renderTargetBlendDescription.DestBlend = D3D11_BLEND_ZERO;
				renderTargetBlendDescription.BlendOp = D3D11_BLEND_OP_ADD;
				renderTargetBlendDescription.SrcBlendAlpha = D3D11_BLEND_ONE;
				renderTargetBlendDescription.DestBlendAlpha = D3D11_BLEND_ZERO;
				renderTargetBlendDescription.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			}
			renderTargetBlendDescription.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		const HRESULT result = myCont->s_direct3dDevice->CreateBlendState( &blendStateDescription, &m_blendState );
		if ( FAILED( result ) )
		{
			wereThereErrors = true;
			ASSERTF( false, "CreateBlendState() failed" );
			Engine::Logging::OutputError( "Direct3D failed to create a blend render state object from %u with HRESULT %#010x", m_bits, result );
			goto OnExit;
		}
	}
	{
		D3D11_DEPTH_STENCIL_DESC depthStateDescription = { 0 };
		depthStateDescription.StencilEnable = FALSE;
		depthStateDescription.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStateDescription.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		depthStateDescription.FrontFace.StencilFunc = depthStateDescription.BackFace.StencilFunc =
			D3D11_COMPARISON_ALWAYS;
		depthStateDescription.FrontFace.StencilDepthFailOp = depthStateDescription.BackFace.StencilDepthFailOp =
			depthStateDescription.FrontFace.StencilPassOp = depthStateDescription.BackFace.StencilPassOp =
			depthStateDescription.FrontFace.StencilFailOp = depthStateDescription.BackFace.StencilFailOp =
			D3D11_STENCIL_OP_KEEP;
		if ( IsDepthBufferingEnabled() )
		{
			depthStateDescription.DepthEnable = TRUE;
			depthStateDescription.DepthFunc = D3D11_COMPARISON_LESS;
			depthStateDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			depthStateDescription.DepthEnable = FALSE;
			depthStateDescription.DepthFunc = D3D11_COMPARISON_ALWAYS;
			depthStateDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		}
		const HRESULT result = myCont->s_direct3dDevice->CreateDepthStencilState( &depthStateDescription, &m_depthStencilState );
		if ( FAILED( result ) )
		{
			wereThereErrors = true;
			ASSERTF( false, "CreateDepthStencilState() failed" );
			Engine::Logging::OutputError( "Direct3D failed to create a depth/stencil render state object from %u with HRESULT %#010x", m_bits, result );
			goto OnExit;
		}
	}
	{
		D3D11_RASTERIZER_DESC rasterizerStateDescription;
		rasterizerStateDescription.FillMode = D3D11_FILL_SOLID;
		rasterizerStateDescription.FrontCounterClockwise = FALSE;
		rasterizerStateDescription.DepthBias = 0;
		rasterizerStateDescription.SlopeScaledDepthBias = 0.0f;
		rasterizerStateDescription.DepthBiasClamp = 0.0f;
		rasterizerStateDescription.DepthClipEnable = TRUE;
		rasterizerStateDescription.ScissorEnable = FALSE;
		rasterizerStateDescription.MultisampleEnable = FALSE;
		rasterizerStateDescription.AntialiasedLineEnable = FALSE;
		if ( ShouldBothTriangleSidesBeDrawn() )
		{
			rasterizerStateDescription.CullMode = D3D11_CULL_NONE;
		}
		else
		{
			rasterizerStateDescription.CullMode = D3D11_CULL_BACK;
		}
		const HRESULT result = myCont->s_direct3dDevice->CreateRasterizerState( &rasterizerStateDescription, &m_rasterizerState );
		if ( FAILED( result ) )
		{
			wereThereErrors = true;
			ASSERTF( false, "CreateRasterizerState() failed" );
			Engine::Logging::OutputError( "Direct3D failed to create a rasterizer render state object from %u with HRESULT %#010x", m_bits, result );
			goto OnExit;
		}
	}

OnExit:

	return !wereThereErrors;
}
