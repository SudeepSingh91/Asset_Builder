#include "ConstantBuffer.h"

namespace Engine
{
	namespace ConstantBuffer
	{
		bool ConstantBuffer::Initialize(enum sBufferType i_type, size_t i_BufferSize, void* i_initialData)
		{
			s_type = i_type;
			s_BufferSize = i_BufferSize;

			if (CreateBuffer(i_initialData))
				return true;
			else
				return false;
		}
	}
}