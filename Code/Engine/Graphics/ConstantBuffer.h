#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include "Context.h"
#include "../Math/cMatrix_transformation.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"

#if defined( EAE6320_PLATFORM_D3D )
	#include <D3D11.h>
#elif defined( EAE6320_PLATFORM_GL )
	#include "../Windows/OpenGl.h"
	#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

namespace eae6320
{
	namespace ConstantBuffer
	{
		enum sBufferType: int { frameType = 0, drawCallType = 1, materialType = 2};
		
		struct sFrame
		{
			Math::cMatrix_transformation g_transform_worldToCamera;
			Math::cMatrix_transformation g_transform_cameraToScreen;
			union
			{
				float g_elapsedSecondCount_total;
				float register8[4];
			};
		};

		struct sDrawCall
		{
			Math::cMatrix_transformation g_transform_localToWorld;
		};

		struct sMaterial
		{
			struct
			{
				float r, g, b, a;
			} g_color;

			sMaterial()
			{
				g_color.r = 1.0f;
				g_color.g = 1.0f;
				g_color.b = 1.0f;
				g_color.a = 1.0f;
			}
		};


		class ConstantBuffer
		{
		private:
			sBufferType s_type;
			size_t s_BufferSize;

			bool CreateBuffer(void* i_initialData);

#if defined( EAE6320_PLATFORM_D3D )
			ID3D11Buffer* s_constantBuffer = NULL;

#elif defined( EAE6320_PLATFORM_GL )
			GLuint s_constantBufferId = 0;

#endif
		public:
			bool Initialize(enum sBufferType i_type, size_t i_BufferSize, void* i_initialData);
			void Bind();
			void Update(void* i_Data);
			bool CleanUp();
		};
	}
}

#endif
