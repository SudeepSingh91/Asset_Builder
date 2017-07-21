#ifndef GRAPHICS_CRENDERSTATE_H
#define GRAPHICS_CRENDERSTATE_H

#include "Configuration.h"

#include <cstdint>

namespace Engine
{
	namespace Graphics
	{
		namespace RenderStates
		{
			enum eRenderState
			{
				AlphaTransparency = 1 << 0,
				DepthBuffering = 1 << 1,
				DrawBothTriangleSides = 1 << 2
			};
			bool IsAlphaTransparencyEnabled( const uint8_t i_renderStateBits );
			void EnableAlphaTransparency( uint8_t& io_renderStateBits );
			void DisableAlphaTransparency( uint8_t& io_renderStateBits );
			bool IsDepthBufferingEnabled( const uint8_t i_renderStateBits );
			void EnableDepthBuffering( uint8_t& io_renderStateBits );
			void DisableDepthBuffering( uint8_t& io_renderStateBits );
			bool ShouldBothTriangleSidesBeDrawn( const uint8_t i_renderStateBits );
			void EnableDrawingBothTriangleSides( uint8_t& io_renderStateBits );
			void DisableDrawingBothTriangleSides( uint8_t& io_renderStateBits );
		}
	}
}

#ifdef PLATFORM_D3D
	struct ID3D11BlendState;
	struct ID3D11DepthStencilState;
	struct ID3D11RasterizerState;
#endif

namespace Engine
{
	namespace Graphics
	{
		class cRenderState
		{
		public:
			void Bind() const;
			bool IsAlphaTransparencyEnabled() const;
			bool IsDepthBufferingEnabled() const;
			bool ShouldBothTriangleSidesBeDrawn() const;
			uint8_t GetRenderStateBits() const;
			bool Initialize( const uint8_t i_renderStateBits );
			bool CleanUp();

			cRenderState();
			~cRenderState();

		private:

#if defined( PLATFORM_D3D )
			ID3D11BlendState* m_blendState;
			ID3D11DepthStencilState* m_depthStencilState;
			ID3D11RasterizerState* m_rasterizerState;
#endif
			uint8_t m_bits;
			bool InitializeFromBits();
		};
	}
}

#include "cRenderState/cRenderState.inl"

#endif
