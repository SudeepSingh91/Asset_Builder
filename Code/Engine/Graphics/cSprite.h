#ifndef EAE6320_GRAPHICS_CSPRITE_H
#define EAE6320_GRAPHICS_CSPRITE_H

#include "Configuration.h"

#include <cstdint>

#ifdef EAE6320_PLATFORM_GL
	#include "OpenGL/Includes.h"
#endif

#ifdef EAE6320_PLATFORM_D3D
	struct ID3D11Buffer;
#endif

namespace eae6320
{
	namespace Graphics
	{
		class cSprite
		{
		public:
			void Draw() const;

			static bool Initialize();
			static bool CleanUp();

			cSprite( const float i_position_left, const float i_position_right, const float i_position_top, const float i_position_bottom,
				const float i_texcoord_left, const float i_texcoord_right, const float i_texcoord_top, const float i_texcoord_bottom );

		private:
			struct
			{
				float left, right, top, bottom;
			} m_position_screen;
			struct
			{
				float left, right, top, bottom;
			} m_textureCoordinates;

#if defined( EAE6320_PLATFORM_D3D )
			static ID3D11Buffer* ms_vertexBuffer;
#elif defined( EAE6320_PLATFORM_GL )
			static GLuint ms_vertexArrayId;
			static GLuint ms_vertexBufferId;
#endif
		};
	}
}

#endif
