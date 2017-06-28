#include "../ConstantBuffer.h"

namespace eae6320
{
	namespace ConstantBuffer
	{
		bool ConstantBuffer::CreateBuffer(void* i_initialData)
		{
			bool wereThereErrors = false;
			{
				const GLsizei bufferCount = 1;
				glGenBuffers(bufferCount, &s_constantBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glBindBuffer(GL_UNIFORM_BUFFER, s_constantBufferId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to bind the new uniform buffer %u: %s",
							s_constantBufferId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get an unused uniform buffer ID: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			{
				const GLenum usage = GL_DYNAMIC_DRAW;	
				glBufferData(GL_UNIFORM_BUFFER, s_BufferSize, reinterpret_cast<const GLvoid*>(i_initialData), usage);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to allocate the new uniform buffer %u: %s",
						s_constantBufferId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}

		OnExit:

			return !wereThereErrors;
		}

		void ConstantBuffer::Bind()
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, s_type, s_constantBufferId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}

		void ConstantBuffer::Update(void* i_Data)
		{
			{
				glBindBuffer(GL_UNIFORM_BUFFER, s_constantBufferId);
				EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			}
			{
				GLintptr updateAtTheBeginning = 0;
				GLsizeiptr updateTheEntireBuffer = static_cast<GLsizeiptr>(s_BufferSize);
				glBufferSubData(GL_UNIFORM_BUFFER, updateAtTheBeginning, updateTheEntireBuffer, i_Data);
				EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			}
		}

		bool ConstantBuffer::CleanUp()
		{
			bool wereThereErrors = false;
			
			if (s_constantBufferId != 0)
			{
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &s_constantBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					Logging::OutputError("OpenGL failed to delete the constant buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
				s_constantBufferId = 0;
			}
			return !wereThereErrors;
		}
	}
}