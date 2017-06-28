#include "../Mesh.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../../External/Lua/Includes.h"

namespace eae6320
{
	namespace Mesh
	{
		bool Mesh::Initialize()
		{
			bool wereThereErrors = false;
			GLuint vertexBufferId = 0;
			GLuint indexBufferId = 0;

			{
				const GLsizei arrayCount = 1;
				glGenVertexArrays(arrayCount, &s_vertexArrayId);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glBindVertexArray(s_vertexArrayId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to bind the vertex array: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}

			{
				const GLsizei bufferCount = 1;
				glGenBuffers(bufferCount, &vertexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to bind the vertex buffer: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			{
				const unsigned int bufferSize = s_numVertices * sizeof(sVertex);

				glBufferData(GL_ARRAY_BUFFER, bufferSize, reinterpret_cast<GLvoid*>(vertexArray),
					GL_STATIC_DRAW);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			{
				const GLsizei stride = sizeof(sVertex);

				{
					const GLuint vertexElementLocation = 0;
					const GLint elementCount = 3;
					const GLboolean notNormalized = GL_FALSE;
					glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
						reinterpret_cast<GLvoid*>(offsetof(sVertex, x)));
					const GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						glEnableVertexAttribArray(vertexElementLocation);
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
								vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
							vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				{
					const GLuint vertexElementLocation = 1;
					const GLint elementCount = 4;
					const GLboolean isNormalized = GL_TRUE;	
					glVertexAttribPointer(vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, isNormalized, stride,
						reinterpret_cast<GLvoid*>(offsetof(sVertex, r)));
					const GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						glEnableVertexAttribArray(vertexElementLocation);
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
								vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
							vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				{
					const GLuint vertexElementLocation = 2;
					const GLint elementCount = 2;
					const GLboolean notNormalized = GL_FALSE;	
					glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
						reinterpret_cast<GLvoid*>(offsetof(sVertex, u)));
					const GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						glEnableVertexAttribArray(vertexElementLocation);
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
								vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
							vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
			}

			{
				const GLsizei bufferCount = 1;
				glGenBuffers(bufferCount, &indexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to bind the index buffer: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get an unused index buffer ID: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}

			{
				const unsigned int bufferSize = s_numIndices * sizeof(uint16_t);

				glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, reinterpret_cast<GLvoid*>(indiceArray),
					GL_STATIC_DRAW);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to allocate the index buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}

		OnExit:

			if (s_vertexArrayId != 0)
			{
				glBindVertexArray(0);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (vertexBufferId != 0)
					{
#ifndef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
						const GLsizei bufferCount = 1;
						glDeleteBuffers(bufferCount, &vertexBufferId);
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to vertex buffer: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
						vertexBufferId = 0;
#else
						s_vertexBufferId = vertexBufferId;
#endif
					}
					if (indexBufferId != 0)
					{
#ifndef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
						const GLsizei bufferCount = 1;
						glDeleteBuffers(bufferCount, &indexBufferId);
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to index buffer: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
						indexBufferId = 0;
#else
						s_indexBufferId = indexBufferId;
#endif
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to unbind the vertex array: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}

			return !wereThereErrors;
		}

		bool Mesh::CleanUp()
		{
			bool wereThereErrors = false;
			
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			if (s_vertexBufferId != 0)
			{
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &s_vertexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
				s_vertexBufferId = 0;
			}
			if (s_indexBufferId != 0)
			{
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &s_indexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					Logging::OutputError("OpenGL failed to delete the index buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
				s_indexBufferId = 0;
			}
#endif
			
			if (s_vertexArrayId != 0)
			{
				const GLsizei arrayCount = 1;
				glDeleteVertexArrays(arrayCount, &s_vertexArrayId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					Logging::OutputError("OpenGL failed to delete the vertex array: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
				s_vertexArrayId = 0;
			}
			return !wereThereErrors;
		}

		void Mesh::Draw()
		{
			{
				glBindVertexArray(s_vertexArrayId);
				EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			}
			{
				const GLenum mode = GL_TRIANGLES;
				const GLenum indexType = GL_UNSIGNED_SHORT;
				const GLvoid* const offset = 0;
				glDrawElements(mode, s_numIndices, indexType, offset);
				EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			}
		}
	}
}