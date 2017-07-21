#include "../Effect.h"
#include "../Context.h"

namespace
{
	Context* myCont = Context::GetContext();

	struct sVertex
	{
		float x, y, z;
		float u, v;
		uint8_t r, g, b, a;
	};
}

namespace Engine
{
	namespace Graphics
	{
		namespace Effect
		{
			bool Effect::LoadFile(const char* i_filepath)
			{
				bool wereThereErrors = false;
					
				Platform::sDataFromFile s_Filedata;

				if (!Platform::LoadBinaryFile(i_filepath, s_Filedata))
				{
					Engine::Logging::OutputError("Direct3D failed to load file");
					return false;
				}

				uint8_t* s_data = reinterpret_cast<uint8_t*>(s_Filedata.data);
				const uint8_t s_renderbits = *reinterpret_cast<uint8_t*>(s_data);
				const uint16_t numverbytes = *reinterpret_cast<uint16_t*>(s_data + sizeof(uint8_t));
				vertexshaderpath = reinterpret_cast<const char*>(s_data + sizeof(uint8_t) + 2);
				fragmentshaderpath = reinterpret_cast<const char*>(s_data + sizeof(uint8_t) + 2 + numverbytes);

				if (!LoadVertexShader())
				{
					wereThereErrors = true;
					goto OnExit;
				}
				if (!LoadFragmentShader())
				{
					wereThereErrors = true;
					goto OnExit;
				}

				s_renderState.Initialize(s_renderbits);
				const unsigned int vertexElementCount = 3;
				D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = { 0, 0 };
				{
					{
						D3D11_INPUT_ELEMENT_DESC& positionElement = layoutDescription[0];

						positionElement.SemanticName = "POSITION";
						positionElement.SemanticIndex = 0;	
						positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
						positionElement.InputSlot = 0;
						positionElement.AlignedByteOffset = offsetof(sVertex, x);
						positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
						positionElement.InstanceDataStepRate = 0;	
					}
					{
						D3D11_INPUT_ELEMENT_DESC& colorElement = layoutDescription[1];

						colorElement.SemanticName = "COLOR";
						colorElement.SemanticIndex = 0;	
						colorElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
						colorElement.InputSlot = 0;
						colorElement.AlignedByteOffset = offsetof(sVertex, r);
						colorElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
						colorElement.InstanceDataStepRate = 0;	
					}
					{
						D3D11_INPUT_ELEMENT_DESC& textureElement = layoutDescription[2];

						textureElement.SemanticName = "TEXCOORD";
						textureElement.SemanticIndex = 0;	
						textureElement.Format = DXGI_FORMAT_R32G32_FLOAT;
						textureElement.InputSlot = 0;
						textureElement.AlignedByteOffset = offsetof(sVertex, u);
						textureElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
						textureElement.InstanceDataStepRate = 0;	
					}
				}

				const HRESULT result = myCont->s_direct3dDevice->CreateInputLayout(layoutDescription, vertexElementCount,
					(*myCont->compiledVertexShader).data, (*myCont->compiledVertexShader).size, &s_vertexLayout);
				if (FAILED(result))
				{
					ASSERT(false);
					Engine::Logging::OutputError("Direct3D failed to create a vertex input layout with HRESULT %#010x", result);
					return false;
				}

			OnExit:
				return !wereThereErrors;
			}
			

			bool Effect::CleanUp()
			{
				bool wereThereErrors = false;

				if (s_vertexLayout)
				{
					s_vertexLayout->Release();
					s_vertexLayout = NULL;
				}

				if (s_vertexShader)
				{
					s_vertexShader->Release();
					s_vertexShader = NULL;
				}
				if (s_fragmentShader)
				{
					s_fragmentShader->Release();
					s_fragmentShader = NULL;
				}

				return !wereThereErrors;
			}

			void Effect::Set()
			{
				{
					myCont->s_direct3dImmediateContext->IASetInputLayout(s_vertexLayout);
					myCont->s_direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				}
				
				ID3D11ClassInstance** const noInterfaces = NULL;
				const unsigned int interfaceCount = 0;
				myCont->s_direct3dImmediateContext->VSSetShader(s_vertexShader, noInterfaces, interfaceCount);
				myCont->s_direct3dImmediateContext->PSSetShader(s_fragmentShader, noInterfaces, interfaceCount);

				s_renderState.Bind();
			}

			bool Effect::LoadFragmentShader()
			{
				bool wereThereErrors = false;
				Engine::Platform::sDataFromFile compiledShader;
				{
					std::string errorMessage;
					if (!Engine::Platform::LoadBinaryFile(fragmentshaderpath, compiledShader, &errorMessage))
					{
						wereThereErrors = true;
						ASSERTF(false, errorMessage.c_str());
						Engine::Logging::OutputError("Failed to load the shader file \"%s\": %s", fragmentshaderpath, errorMessage.c_str());
						goto OnExit;
					}
				}
				{
					ID3D11ClassLinkage* const noInterfaces = NULL;
					const HRESULT result = myCont->s_direct3dDevice->CreatePixelShader(
						compiledShader.data, compiledShader.size, noInterfaces, &s_fragmentShader);
					if (FAILED(result))
					{
						wereThereErrors = true;
						ASSERT(false);
						Engine::Logging::OutputError("Direct3D failed to create the shader %s with HRESULT %#010x", fragmentshaderpath, result);
						goto OnExit;
					}
				}

			OnExit:

				compiledShader.Free();

				return !wereThereErrors;
			}

			bool Effect::LoadVertexShader()
			{
				bool wereThereErrors = false;
				{
					std::string errorMessage;
					if (!Engine::Platform::LoadBinaryFile(vertexshaderpath, *myCont->compiledVertexShader, &errorMessage))
					{
						wereThereErrors = true;
						ASSERTF(false, errorMessage.c_str());
						Engine::Logging::OutputError("Failed to load the shader file \"%s\": %s", vertexshaderpath, errorMessage.c_str());
						goto OnExit;
					}
				}
				{
					ID3D11ClassLinkage* const noInterfaces = NULL;
					const HRESULT result = myCont->s_direct3dDevice->CreateVertexShader(myCont->compiledVertexShader->data, myCont->compiledVertexShader->size,
						noInterfaces, &s_vertexShader);
					if (FAILED(result))
					{
						wereThereErrors = true;
						ASSERT(false);
						Engine::Logging::OutputError("Direct3D failed to create the shader %s with HRESULT %#010x", vertexshaderpath, result);
						goto OnExit;
					}
				}

			OnExit:

				return !wereThereErrors;
			}
		}
	}
}