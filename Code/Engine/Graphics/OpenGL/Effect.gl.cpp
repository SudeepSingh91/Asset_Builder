#include "../Effect.h"

namespace
{
	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
		~sLogInfo() { if (memory) free(memory); }
	};
}

namespace eae6320
{
	namespace Graphics
	{
		namespace Effect
		{
			bool Effect::LoadFile(const char* i_filepath)
			{
				{
					Platform::sDataFromFile s_Filedata;

					if (!Platform::LoadBinaryFile(i_filepath, s_Filedata))
					{
						eae6320::Logging::OutputError("OpenGL failed to load file");
						return false;
					}
					
					uint8_t* s_data = reinterpret_cast<uint8_t*>(s_Filedata.data);
					const uint8_t s_renderbits = *reinterpret_cast<uint8_t*>(s_data);
					const uint16_t numverbytes = *reinterpret_cast<uint16_t*>(s_data + sizeof(uint8_t));
					vertexshaderpath = reinterpret_cast<const char*>(s_data + sizeof(uint8_t) + 2);
					fragmentshaderpath = reinterpret_cast<const char*>(s_data + sizeof(uint8_t) + 2 + numverbytes);

					s_programId = glCreateProgram();
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to create a program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						return false;
					}
					else if (s_programId == 0)
					{
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("OpenGL failed to create a program");
						return false;
					}

					s_renderState.Initialize(s_renderbits);
				}
				if (!LoadVertexShader())
				{
					EAE6320_ASSERT(false);
					return false;
				}
				if (!LoadFragmentShader())
				{
					EAE6320_ASSERT(false);
					return false;
				}
				{
					glLinkProgram(s_programId);
					GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						std::string linkInfo;
						{
							GLint infoSize;
							glGetProgramiv(s_programId, GL_INFO_LOG_LENGTH, &infoSize);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								sLogInfo info(static_cast<size_t>(infoSize));
								GLsizei* dontReturnLength = NULL;
								glGetProgramInfoLog(s_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
								errorCode = glGetError();
								if (errorCode == GL_NO_ERROR)
								{
									linkInfo = info.memory;
								}
								else
								{
									EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
									eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
										reinterpret_cast<const char*>(gluErrorString(errorCode)));
									return false;
								}
							}
							else
							{
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
								return false;
							}
						}
						GLint didLinkingSucceed;
						{
							glGetProgramiv(s_programId, GL_LINK_STATUS, &didLinkingSucceed);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								if (didLinkingSucceed == GL_FALSE)
								{
									EAE6320_ASSERTF(false, linkInfo.c_str());
									eae6320::Logging::OutputError("The program failed to link: %s",
										linkInfo.c_str());
									return false;
								}
							}
							else
							{
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
								return false;
							}
						}
					}
					else
					{
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						return false;
					}
				}

				return true;
			}

			bool Effect::CleanUp()
			{
				bool wereThereErrors = false;
				
				if (s_programId != 0)
				{
					glDeleteProgram(s_programId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						Logging::OutputError("OpenGL failed to delete the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
					}
					s_programId = 0;
				}

				return !wereThereErrors;
			}

			void Effect::Set()
			{
				glUseProgram(s_programId);
				EAE6320_ASSERT(glGetError() == GL_NO_ERROR);

				s_renderState.Bind();
			}

			bool Effect::LoadFragmentShader()
			{
				{
					GLboolean isShaderCompilingSupported;
					glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
					if (!isShaderCompilingSupported)
					{
						return false;
					}
				}

				bool wereThereErrors = false;
				GLuint fragmentShaderId = 0;
				eae6320::Platform::sDataFromFile dataFromFile;
				{
					{
						std::string errorMessage;
						if (!eae6320::Platform::LoadBinaryFile(fragmentshaderpath, dataFromFile, &errorMessage))
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, errorMessage.c_str());
							eae6320::Logging::OutputError("Failed to load the fragment shader \"%s\": %s",
								fragmentshaderpath, errorMessage.c_str());
							goto OnExit;
						}
					}
					fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
					{
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to get an unused fragment shader ID: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
						else if (fragmentShaderId == 0)
						{
							wereThereErrors = true;
							EAE6320_ASSERT(false);
							eae6320::Logging::OutputError("OpenGL failed to get an unused fragment shader ID");
							goto OnExit;
						}
					}
					{
						const GLsizei shaderSourceCount = 1;
						const GLint length = static_cast<GLuint>(dataFromFile.size);
						glShaderSource(fragmentShaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&dataFromFile.data), &length);
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to set the fragment shader source code: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
					}
				}
				{
					glCompileShader(fragmentShaderId);
					GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						std::string compilationInfo;
						{
							GLint infoSize;
							glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								sLogInfo info(static_cast<size_t>(infoSize));
								GLsizei* dontReturnLength = NULL;
								glGetShaderInfoLog(fragmentShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
								errorCode = glGetError();
								if (errorCode == GL_NO_ERROR)
								{
									compilationInfo = info.memory;
								}
								else
								{
									wereThereErrors = true;
									EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
									eae6320::Logging::OutputError("OpenGL failed to get compilation info about the fragment shader source code: %s",
										reinterpret_cast<const char*>(gluErrorString(errorCode)));
									goto OnExit;
								}
							}
							else
							{
								wereThereErrors = true;
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to get the length of the fragment shader compilation info: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
								goto OnExit;
							}
						}
						GLint didCompilationSucceed;
						{
							glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								if (didCompilationSucceed == GL_FALSE)
								{
									wereThereErrors = true;
									EAE6320_ASSERTF(false, compilationInfo.c_str());
									eae6320::Logging::OutputError("OpenGL failed to compile the fragment shader: %s",
										compilationInfo.c_str());
									goto OnExit;
								}
							}
							else
							{
								wereThereErrors = true;
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to find if compilation of the fragment shader source code succeeded: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
								goto OnExit;
							}
						}
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to compile the fragment shader source code: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				{
					glAttachShader(s_programId, fragmentShaderId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to attach the fragment to the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}

			OnExit:

				if (fragmentShaderId != 0)
				{
					glDeleteShader(fragmentShaderId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to delete the fragment shader ID %u: %s",
							fragmentShaderId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					}
					fragmentShaderId = 0;
				}
				dataFromFile.Free();

				return !wereThereErrors;
			}

			bool Effect::LoadVertexShader()
			{
				{
					GLboolean isShaderCompilingSupported;
					glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
					if (!isShaderCompilingSupported)
					{
						return false;
					}
				}

				bool wereThereErrors = false;
				GLuint vertexShaderId = 0;
				eae6320::Platform::sDataFromFile dataFromFile;
				{
					{
						std::string errorMessage;
						if (!eae6320::Platform::LoadBinaryFile(vertexshaderpath, dataFromFile, &errorMessage))
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, errorMessage.c_str());
							eae6320::Logging::OutputError("Failed to load the vertex shader \"%s\": %s",
								vertexshaderpath, errorMessage.c_str());
							goto OnExit;
						}
					}
					vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
					{
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to get an unused vertex shader ID: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
						else if (vertexShaderId == 0)
						{
							wereThereErrors = true;
							EAE6320_ASSERT(false);
							eae6320::Logging::OutputError("OpenGL failed to get an unused vertex shader ID");
							goto OnExit;
						}
					}
					{
						const GLsizei shaderSourceCount = 1;
						const GLint length = static_cast<GLuint>(dataFromFile.size);
						glShaderSource(vertexShaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&dataFromFile.data), &length);
						const GLenum errorCode = glGetError();
						if (errorCode != GL_NO_ERROR)
						{
							wereThereErrors = true;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to set the vertex shader source code: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							goto OnExit;
						}
					}
				}
				{
					glCompileShader(vertexShaderId);
					GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						std::string compilationInfo;
						{
							GLint infoSize;
							glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								sLogInfo info(static_cast<size_t>(infoSize));
								GLsizei* dontReturnLength = NULL;
								glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
								errorCode = glGetError();
								if (errorCode == GL_NO_ERROR)
								{
									compilationInfo = info.memory;
								}
								else
								{
									wereThereErrors = true;
									EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
									eae6320::Logging::OutputError("OpenGL failed to get compilation info about the vertex shader source code: %s",
										reinterpret_cast<const char*>(gluErrorString(errorCode)));
									goto OnExit;
								}
							}
							else
							{
								wereThereErrors = true;
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to get the length of the vertex shader compilation info: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
								goto OnExit;
							}
						}
						GLint didCompilationSucceed;
						{
							glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								if (didCompilationSucceed == GL_FALSE)
								{
									wereThereErrors = true;
									EAE6320_ASSERTF(false, compilationInfo.c_str());
									eae6320::Logging::OutputError("OpenGL failed to compile the vertex shader: %s",
										compilationInfo.c_str());
									goto OnExit;
								}
							}
							else
							{
								wereThereErrors = true;
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to find if compilation of the vertex shader source code succeeded: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
								goto OnExit;
							}
						}
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to compile the vertex shader source code: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				{
					glAttachShader(s_programId, vertexShaderId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to attach the vertex to the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}

			OnExit:

				if (vertexShaderId != 0)
				{
					glDeleteShader(vertexShaderId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to delete the vertex shader ID %u: %s",
							vertexShaderId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					}
					vertexShaderId = 0;
				}
				dataFromFile.Free();

				return !wereThereErrors;
			}
		}
	}
}