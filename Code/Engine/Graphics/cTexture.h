#ifndef GRAPHICS_CTEXTURE_H
#define GRAPHICS_CTEXTURE_H

#include "Configuration.h"

#ifdef PLATFORM_GL
	#include "OpenGL/Includes.h"
#endif

namespace Engine
{
	namespace Graphics
	{
		struct sDdsInfo;
	}

	namespace Platform
	{
		struct sDataFromFile;
	}
}
namespace DirectX
{
	struct DDS_HEADER;
}

#ifdef PLATFORM_D3D
	struct ID3D11ShaderResourceView;
#endif

namespace Engine
{
	namespace Graphics
	{
		class cTexture
		{
		public:
			void Bind( const unsigned int i_id ) const;
			bool Load( const char* const i_path );
			bool CleanUp();

			cTexture();
			~cTexture();

		private:

#if defined( PLATFORM_D3D )
			ID3D11ShaderResourceView* m_textureView;
#elif defined( PLATFORM_GL )
			GLuint m_textureId;
#endif

		private:
			bool Initialize( const char* const i_path, const sDdsInfo& i_ddsInfo );
		};
	}
}

#endif	
