#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

namespace Engine
{
	namespace Graphics
	{
		namespace VertexFormat
		{
			struct sVertex
			{
				float x, y, z;
				float u, v;
				uint8_t r, g, b, a;
			};
		}
	}
}

#endif