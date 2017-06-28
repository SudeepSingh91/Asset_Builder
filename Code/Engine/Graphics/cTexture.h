#ifndef EAE6320_GRAPHICS_CTEXTURE_H
#define EAE6320_GRAPHICS_CTEXTURE_H

#include "Configuration.h"

#ifdef EAE6320_PLATFORM_GL
	#include "OpenGL/Includes.h"
#endif

namespace eae6320
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

#ifdef EAE6320_PLATFORM_D3D
	struct ID3D11ShaderResourceView;
#endif

namespace eae6320
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

#if defined( EAE6320_PLATFORM_D3D )
			ID3D11ShaderResourceView* m_textureView;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_textureId;
#endif

		private:
			bool Initialize( const char* const i_path, const sDdsInfo& i_ddsInfo );
		};
	}
}

#endif	
