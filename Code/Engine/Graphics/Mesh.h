#ifndef MESH_H
#define MESH_H

#include "Configuration.h"
#include "../Platform/Platform.h"

#include <cstdint>

#if defined( PLATFORM_D3D )
	#include <D3D11.h>
#elif defined( PLATFORM_GL )
	#include "../Windows/OpenGl.h"
	#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

namespace Engine
{
	namespace Mesh
	{
		struct sVertex
		{
			float x, y, z;
			float u, v;
			uint8_t r, g, b, a;
		};
		
		class Mesh
		{
		public:
			bool LoadFile(const char* i_path);
			bool CleanUp();
			void Draw();

		private:
			bool Initialize();

			uint16_t s_numVertices;
			uint16_t s_numIndices;

			sVertex* vertexArray;
			uint16_t* indiceArray;

#if defined( PLATFORM_D3D )
			ID3D11Buffer* s_vertexBuffer = NULL;
			ID3D11Buffer* s_indexBuffer = NULL;

#elif defined( PLATFORM_GL )
			GLuint s_vertexArrayId = 0;

#ifdef GRAPHICS_ISDEVICEDEBUGINFOENABLED
			GLuint s_vertexBufferId = 0;
			GLuint s_indexBufferId = 0;
#endif

#endif 
		};
	}
}

#endif