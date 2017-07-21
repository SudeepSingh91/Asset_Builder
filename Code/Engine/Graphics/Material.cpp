#include "Material.h"

namespace Engine
{
	namespace Graphics
	{
		namespace Materials
		{
			bool Material::Load(const char* i_filepath)
			{
				bool wereThereErrors = false;

				Platform::sDataFromFile s_Filedata;

				if (!Platform::LoadBinaryFile(i_filepath, s_Filedata))
				{
					Engine::Logging::OutputError("Direct3D failed to load file");
					return false;
				}

				uint8_t* s_data = reinterpret_cast<uint8_t*>(s_Filedata.data);
				ConstantBuffer::sMaterial mat = *reinterpret_cast<ConstantBuffer::sMaterial*>(s_data);
				uint16_t size = *reinterpret_cast<uint16_t*>(s_data + sizeof(ConstantBuffer::sMaterial));
				const char* texpath = reinterpret_cast<const char*>(s_data + sizeof(ConstantBuffer::sMaterial) + sizeof(uint16_t));
				const char* effpath = reinterpret_cast<const char*>(s_data + sizeof(ConstantBuffer::sMaterial) + sizeof(uint16_t) + size);

				s_texture.Load(texpath);
				s_effect.LoadFile(effpath);
				s_constantBuffer.Initialize(ConstantBuffer::sBufferType::materialType, sizeof(ConstantBuffer::sMaterial), reinterpret_cast<void*>(&mat));

				return !wereThereErrors;
			}

			bool Material::CleanUp()
			{
				bool wereThereErrors = false;
				
				if (!s_constantBuffer.CleanUp())
					wereThereErrors = true;
				if (!s_effect.CleanUp())
					wereThereErrors = true;
				if (!s_texture.CleanUp())
					wereThereErrors = true;

				return !wereThereErrors;
			}

			void Material::Bind()
			{
				unsigned int id = 0;
				s_texture.Bind(id);
				s_effect.Set();
				s_constantBuffer.Bind();
			}
		}
	}
}