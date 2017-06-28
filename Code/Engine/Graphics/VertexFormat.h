#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

namespace eae6320
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