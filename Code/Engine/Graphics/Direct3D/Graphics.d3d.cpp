#include "../Graphics.h"
#include "../Context.h"
#include "../Mesh.h"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX11core.h>
#include <DXGI.h>
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Time/Time.h"

namespace
{
	Context* myCont = Context::GetContext();
	HWND s_renderingWindow = NULL;
	IDXGISwapChain* s_swapChain = NULL;
	ID3D11DepthStencilView* s_depthStencilView = NULL;
	ID3D11RenderTargetView* s_renderTargetView = NULL;
	ID3D11SamplerState* s_samplerState = NULL;

	eae6320::ConstantBuffer::sFrame s_Frame;
	eae6320::ConstantBuffer::sDrawCall s_Draw;

	eae6320::ConstantBuffer::ConstantBuffer s_FrameBuffer;
	eae6320::ConstantBuffer::ConstantBuffer s_DrawBuffer;

	eae6320::Math::cVector camPos(0.0f, 1.5f, 10.0f);
	eae6320::Math::cQuaternion camOri;
	eae6320::Camera::Camera s_Camera(camPos, camOri, eae6320::Math::ConvertDegreesToRadians(60.0f), 0.1f, 100.0f);

	std::vector<eae6320::Graphics::DrawCallData> s_MeshList;
	std::vector<eae6320::Graphics::SpriteCallData> s_SprList;
}

namespace
{
	bool CreateConstantBuffer(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
	bool CreateDevice( const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight );
	bool CreateView( const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight );
	bool CreateSampler();
	bool CreateRasterizer();
}

void eae6320::Graphics::RenderFrame()
{
	{
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		myCont->s_direct3dImmediateContext->ClearRenderTargetView(s_renderTargetView, clearColor);
	}

	{
		const float depthToClear = 1.0f;
		const uint8_t stencilToClear = 0; 
		myCont->s_direct3dImmediateContext->ClearDepthStencilView(s_depthStencilView, D3D11_CLEAR_DEPTH,
			depthToClear, stencilToClear);
	}

	s_Frame.g_transform_worldToCamera = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(s_Camera.Ori(), s_Camera.Pos());
	s_Frame.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	s_FrameBuffer.Update(reinterpret_cast<void*>(&s_Frame));

	for (std::vector<eae6320::Graphics::DrawCallData>::iterator it = s_MeshList.begin(); it != s_MeshList.end(); ++it)
	{
		it->s_mat->Bind();
		s_Draw = it->s_drawCall;
		s_DrawBuffer.Update(reinterpret_cast<void*>(&s_Draw));
		it->s_mesh->Draw();
	}

	s_MeshList.clear();

	for (std::vector<eae6320::Graphics::SpriteCallData>::iterator it = s_SprList.begin(); it != s_SprList.end(); ++it)
	{
		it->s_mat->Bind();
		it->s_sprite->Draw();
	}
	s_SprList.clear();

	{
		const unsigned int swapImmediately = 0;
		const unsigned int presentNextFrame = 0;
		const HRESULT result = s_swapChain->Present(swapImmediately, presentNextFrame);
		EAE6320_ASSERT(SUCCEEDED(result));
	}
}

void eae6320::Graphics::SetMesh(eae6320::Graphics::MeshData i_mesh)
{
	eae6320::Math::cVector temp1(0.0f, 1.0f, 0.0f);
	eae6320::Math::cQuaternion tempRot(eae6320::Math::ConvertDegreesToRadians(i_mesh.s_rot.y), temp1);
	eae6320::Math::cMatrix_transformation tempMat(tempRot, i_mesh.s_pos);
	eae6320::ConstantBuffer::sDrawCall tempDraw;
	tempDraw.g_transform_localToWorld = tempMat;
	eae6320::Graphics::DrawCallData s_data;
	s_data.s_mesh = i_mesh.s_mesh;
	s_data.s_drawCall = tempDraw;
	s_data.s_mat = i_mesh.s_mat;
	s_MeshList.push_back(s_data);
}

void eae6320::Graphics::SetSprite(cSprite* i_spr, Materials::Material* i_mat)
{
	SpriteCallData temp;
	temp.s_sprite = i_spr;
	temp.s_mat = i_mat;
	s_SprList.push_back(temp);
}

void eae6320::Graphics::SetCamera(eae6320::Camera::Camera i_cam)
{
	s_Camera = i_cam;
}

bool eae6320::Graphics::Initialize( const sInitializationParameters& i_initializationParameters )
{
	bool wereThereErrors = false;
	
	s_renderingWindow = i_initializationParameters.mainWindow;
	if ( !CreateDevice( i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}

	if (!CreateSampler())
	{
		wereThereErrors = true;
		goto OnExit;
	}

	if ( !CreateView( i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}

	if ( !CreateConstantBuffer(i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight) )
	{
		wereThereErrors = true;
		goto OnExit;
	}

OnExit:

	return !wereThereErrors;
}

bool eae6320::Graphics::CleanUp()
{
	bool wereThereErrors = false;

	if (s_samplerState)
	{
		s_samplerState->Release();
		s_samplerState = NULL;
	}

	if ( myCont->s_direct3dDevice )
	{
		if (!s_FrameBuffer.CleanUp())
			wereThereErrors = true;
		if (!s_DrawBuffer.CleanUp())
			wereThereErrors = true;

		if ( s_renderTargetView )
		{
			s_renderTargetView->Release();
			s_renderTargetView = NULL;
		}

		myCont->s_direct3dDevice->Release();
		myCont->s_direct3dDevice = NULL;
	}
	if ( myCont->s_direct3dImmediateContext )
	{
		myCont->s_direct3dImmediateContext->Release();
		myCont->s_direct3dImmediateContext = NULL;
	}
	if ( s_swapChain )
	{
		s_swapChain->Release();
		s_swapChain = NULL;
	}

	delete myCont;

	s_renderingWindow = NULL;

	return !wereThereErrors;
}

namespace
{
	bool CreateConstantBuffer(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		bool wereThereErrors = false;
		
		s_Frame.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
		s_Frame.g_transform_worldToCamera = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(s_Camera.Ori(), s_Camera.Pos());
		s_Frame.g_transform_cameraToScreen = eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform(s_Camera.fov(), static_cast<float>(i_resolutionWidth/i_resolutionHeight), s_Camera.nearPlaneDist(), s_Camera.farPlaneDist());
		
		if (!s_FrameBuffer.Initialize(eae6320::ConstantBuffer::sBufferType::frameType, sizeof(eae6320::ConstantBuffer::sFrame), reinterpret_cast<void*>(&s_Frame)))
			wereThereErrors = true;
		s_FrameBuffer.Bind();

		if (!s_DrawBuffer.Initialize(eae6320::ConstantBuffer::sBufferType::drawCallType, sizeof(eae6320::ConstantBuffer::sDrawCall), reinterpret_cast<void*>(&s_Draw)))
			wereThereErrors = true;
		s_DrawBuffer.Bind();

		return !wereThereErrors;
	}

	bool CreateDevice( const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight )
	{
		IDXGIAdapter* const useDefaultAdapter = NULL;
		const D3D_DRIVER_TYPE useHardwareRendering = D3D_DRIVER_TYPE_HARDWARE;
		const HMODULE dontUseSoftwareRendering = NULL;
		unsigned int flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
		{
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		}
		D3D_FEATURE_LEVEL* const useDefaultFeatureLevels = NULL;
		const unsigned int requestedFeatureLevelCount = 0;
		const unsigned int sdkVersion = D3D11_SDK_VERSION;
		DXGI_SWAP_CHAIN_DESC swapChainDescription = { 0 };
		{
			{
				DXGI_MODE_DESC& bufferDescription = swapChainDescription.BufferDesc;

				bufferDescription.Width = i_resolutionWidth;
				bufferDescription.Height = i_resolutionHeight;
				{
					DXGI_RATIONAL& refreshRate = bufferDescription.RefreshRate;

					refreshRate.Numerator = 0;	
					refreshRate.Denominator = 1;
				}
				bufferDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				bufferDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				bufferDescription.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			}
			{
				DXGI_SAMPLE_DESC& multiSamplingDescription = swapChainDescription.SampleDesc;

				multiSamplingDescription.Count = 1;
				multiSamplingDescription.Quality = 0;	
			}
			swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDescription.BufferCount = 1;
			swapChainDescription.OutputWindow = s_renderingWindow;
			swapChainDescription.Windowed = TRUE;
			swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDescription.Flags = 0;
		}
		D3D_FEATURE_LEVEL highestSupportedFeatureLevel;
		const HRESULT result = D3D11CreateDeviceAndSwapChain( useDefaultAdapter, useHardwareRendering, dontUseSoftwareRendering,
			flags, useDefaultFeatureLevels, requestedFeatureLevelCount, sdkVersion, &swapChainDescription,
			&s_swapChain, &myCont->s_direct3dDevice, &highestSupportedFeatureLevel, &myCont->s_direct3dImmediateContext );
		if ( SUCCEEDED( result ) )
		{
			return true;
		}
		else
		{
			EAE6320_ASSERT( false );
			eae6320::Logging::OutputError( "Direct3D failed to create a Direct3D11 device with HRESULT %#010x", result );
			return false;
		}
	}

	bool CreateView( const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight )
	{
		bool wereThereErrors = false;

		ID3D11Texture2D* backBuffer = NULL;
		ID3D11Texture2D* depthBuffer = NULL;

		{
			{
				const unsigned int bufferIndex = 0; 
				const HRESULT result = s_swapChain->GetBuffer(bufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to get the back buffer from the swap chain with HRESULT %#010x", result);
					goto OnExit;
				}
			}
			{
				const D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = NULL;
				const HRESULT result = myCont->s_direct3dDevice->CreateRenderTargetView(backBuffer, accessAllSubResources, &s_renderTargetView);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the render target view with HRESULT %#010x", result);
					goto OnExit;
				}
			}
		}
		{
			{
				D3D11_TEXTURE2D_DESC textureDescription = { 0 };
				{
					textureDescription.Width = i_resolutionWidth;
					textureDescription.Height = i_resolutionHeight;
					textureDescription.MipLevels = 1; 
					textureDescription.ArraySize = 1;
					textureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; 
					{
						DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;
						sampleDescription.Count = 1; 
						sampleDescription.Quality = 0; 
					}
					textureDescription.Usage = D3D11_USAGE_DEFAULT; 
					textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					textureDescription.CPUAccessFlags = 0; 
					textureDescription.MiscFlags = 0;
				}
				const D3D11_SUBRESOURCE_DATA* const noInitialData = NULL;
				const HRESULT result = myCont->s_direct3dDevice->CreateTexture2D(&textureDescription, noInitialData, &depthBuffer);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the depth buffer resource with HRESULT %#010x", result);
					goto OnExit;
				}
			}
			{
				const D3D11_DEPTH_STENCIL_VIEW_DESC* const noSubResources = NULL;
				const HRESULT result = myCont->s_direct3dDevice->CreateDepthStencilView(depthBuffer, noSubResources, &s_depthStencilView);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the depth stencil view with HRESULT %#010x", result);
					goto OnExit;
				}
			}
		}
		{
			const unsigned int renderTargetCount = 1;
			myCont->s_direct3dImmediateContext->OMSetRenderTargets(renderTargetCount, &s_renderTargetView, s_depthStencilView);
		}
		{
			D3D11_VIEWPORT viewPort = { 0 };
			viewPort.TopLeftX = viewPort.TopLeftY = 0.0f;
			viewPort.Width = static_cast<float>(i_resolutionWidth);
			viewPort.Height = static_cast<float>(i_resolutionHeight);
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;
			const unsigned int viewPortCount = 1;
			myCont->s_direct3dImmediateContext->RSSetViewports(viewPortCount, &viewPort);
		}

	OnExit:

		if (backBuffer)
		{
			backBuffer->Release();
			backBuffer = NULL;
		}
		if (depthBuffer)
		{
			depthBuffer->Release();
			depthBuffer = NULL;
		}

		return !wereThereErrors;
	}

	bool CreateSampler()
	{
		{
			D3D11_SAMPLER_DESC samplerStateDescription;
			{
				samplerStateDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				samplerStateDescription.AddressU = samplerStateDescription.AddressV
					= samplerStateDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerStateDescription.MipLODBias = 0.0f;
				samplerStateDescription.MaxAnisotropy = 1;
				samplerStateDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
				samplerStateDescription.BorderColor[0] = samplerStateDescription.BorderColor[1]
					= samplerStateDescription.BorderColor[2] = samplerStateDescription.BorderColor[3] = 1.0f;
				samplerStateDescription.MinLOD = -FLT_MAX;
				samplerStateDescription.MaxLOD = FLT_MAX;
			}
			const HRESULT result = myCont->s_direct3dDevice->CreateSamplerState(&samplerStateDescription, &s_samplerState);
			if (FAILED(result))
			{
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to create a sampler state with HRESULT %#010x", result);
				return false;
			}
		}
		{
			const unsigned int samplerStateRegister = 0; 
			const unsigned int samplerStateCount = 1;
			myCont->s_direct3dImmediateContext->PSSetSamplers(samplerStateRegister, samplerStateCount, &s_samplerState);
		}
		return true;
	}
}