#include "Mesh.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"

#include <cmath>

namespace Engine
{
	namespace Mesh
	{
		bool Mesh::LoadFile(const char* i_path)
		{
			bool wereThereErrors = false;

			Platform::sDataFromFile s_Filedata;

			if (!Platform::LoadBinaryFile(i_path, s_Filedata))
				wereThereErrors = true;

			uint8_t* s_data = reinterpret_cast<uint8_t*>(s_Filedata.data);
			s_numVertices = *reinterpret_cast<uint16_t*>(s_data);
			vertexArray = reinterpret_cast<sVertex*>(s_data + 2);

			s_numIndices = *reinterpret_cast<uint16_t*>(s_data + 2 + (s_numVertices * sizeof(sVertex)));
			indiceArray = reinterpret_cast<uint16_t*>(s_data + 2 + 2 + (s_numVertices * sizeof(sVertex)));

			if (!Initialize())
			{
				wereThereErrors = true;
			}

			return !wereThereErrors;
		}
	}
}