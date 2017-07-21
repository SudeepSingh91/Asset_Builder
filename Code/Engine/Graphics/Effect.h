#ifndef EFFECT_H
#define EFFECT_H

#if defined( PLATFORM_D3D )
	#include <D3D11.h>
#elif defined( PLATFORM_GL )
	#include "../Windows/OpenGl.h"
	#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../Platform/Platform.h"
#include "cRenderState.h"

namespace Engine
{
	namespace Graphics
	{
		namespace ShaderTypes
		{
			enum eShaderType : int { Unknown = 0, Vertex = 1, Fragment = 2 };

		}

		namespace Effect
		{
			class Effect
			{
			public:
				bool LoadFile(const char* i_filepath);
				bool CleanUp();
				void Set();

			private:
#if defined( PLATFORM_D3D )
				ID3D11VertexShader* s_vertexShader = NULL;
				ID3D11PixelShader* s_fragmentShader = NULL;
				ID3D11InputLayout* s_vertexLayout = NULL;

#elif defined( PLATFORM_GL )
				GLuint s_programId = 0;

#endif
				
				cRenderState s_renderState;

				bool LoadVertexShader();
				bool LoadFragmentShader();

				const char* vertexshaderpath;
				const char* fragmentshaderpath;
			};
		}
	}
}

#endif