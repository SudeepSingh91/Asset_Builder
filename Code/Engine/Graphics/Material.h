#ifndef MATERIAL_H
#define MATERIAL_H

#include "ConstantBuffer.h"
#include "Effect.h"
#include "cTexture.h"

namespace Engine
{
	namespace Graphics
	{
		namespace Materials
		{
			class Material
			{
			private:
				Effect::Effect s_effect;
				ConstantBuffer::ConstantBuffer s_constantBuffer;
				cTexture s_texture;

			public:
				bool Load(const char* i_filepath);
				bool CleanUp();
				void Bind();
			};
		}
	}
}

#endif

