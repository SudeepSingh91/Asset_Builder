#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Configuration.h"
#include "Mesh.h"
#include "cSprite.h"
#include "Effect.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "../Math/cQuaternion.h"
#include "../Math/cVector.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/Functions.h"
#include "../Platform/Platform.h"
#include "Effect.h"

#if defined( PLATFORM_WINDOWS )
#include "../Windows/Includes.h"
#endif

#include <vector>

namespace Engine
{
	namespace Graphics
	{
		struct MeshData
		{
			Mesh::Mesh* s_mesh;
			Math::cVector s_pos;
			Math::cVector s_rot;
			Materials::Material* s_mat;
		};

		struct DrawCallData
		{
			Mesh::Mesh* s_mesh;
			Materials::Material* s_mat;
			ConstantBuffer::sDrawCall s_drawCall;
		};

		struct SpriteCallData
		{
			cSprite* s_sprite;
			Materials::Material* s_mat;
		};

		struct sInitializationParameters
		{
#if defined( PLATFORM_WINDOWS )
			HWND mainWindow;
#if defined( PLATFORM_D3D )
			unsigned int resolutionWidth, resolutionHeight;
#elif defined( PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication;
#endif
#endif
		};

		class GraphicsData
		{
		public:
			static GraphicsData* GetGraphicsInst();
			static void Destroy();

			bool CleanUp();
			void RenderFrame();
			bool Initialize(const sInitializationParameters& i_initializationParameters);
			void SetMesh(MeshData& i_mesh);
			void SetSprite(cSprite* i_spr, Materials::Material* i_mat);
			void SetCamera(Camera::Camera& i_cam);

#if defined( PLATFORM_D3D )
			bool CreateConstantBuffer(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
			bool CreateDevice(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
			bool CreateView(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
			bool CreateSampler();
#elif defined( PLATFORM_GL )
			bool CreateConstantBuffer();
			bool CreateProgram();
			bool CreateRenderingContext();
			bool EnableBackfaceCulling();
			bool CreateSampler();
#endif

		private:
			GraphicsData();
			GraphicsData(const GraphicsData& i_data);
			GraphicsData& operator=(const GraphicsData& i_data);
			~GraphicsData();

			static GraphicsData* m_graphicsData;

			std::vector<Engine::Graphics::DrawCallData> s_MeshList;
			std::vector<Engine::Graphics::SpriteCallData> s_SprList;

			Engine::ConstantBuffer::sFrame s_Frame;
			Engine::ConstantBuffer::sDrawCall s_Draw;

			Engine::ConstantBuffer::ConstantBuffer s_FrameBuffer;
			Engine::ConstantBuffer::ConstantBuffer s_DrawBuffer;

#if defined( PLATFORM_D3D )
			HWND s_renderingWindow = NULL;
			IDXGISwapChain* s_swapChain = NULL;
			ID3D11DepthStencilView* s_depthStencilView = NULL;
			ID3D11RenderTargetView* s_renderTargetView = NULL;
			ID3D11SamplerState* s_samplerState = NULL;
#elif defined( PLATFORM_GL )
			HWND s_renderingWindow = NULL;
			HDC s_deviceContext = NULL;
			HGLRC s_openGlRenderingContext = NULL;
			GLuint s_samplerState = NULL;
			GLuint s_vertexArrayId = 0;
			GLuint s_programId = 0;

#ifdef GRAPHICS_ISDEVICEDEBUGINFOENABLED
			GLuint s_vertexBufferId = 0;
#endif

			/*Engine::Math::cVector camPos(0.0f, 1.5f, 10.0f);
			Engine::Math::cQuaternion camOri;
			Engine::Camera::Camera s_Camera(camPos, camOri, Engine::Math::ConvertDegreesToRadians(60.0f), 0.1f, 100.0f);*/

#endif
			Engine::Math::cVector* s_camPos;
			Engine::Math::cQuaternion* s_camOri;
			Engine::Camera::Camera* s_camera;
		};
	}
}

#endif	