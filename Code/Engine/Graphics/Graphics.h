#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

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

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "../Windows/Includes.h"
#endif

namespace eae6320
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

		void SetMesh(MeshData i_mesh);
		void SetSprite(cSprite* i_spr, Materials::Material* i_mat);
		void SetCamera(Camera::Camera i_cam);
		void ClearScreen();
		void RenderFrame();

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow;
	#if defined( EAE6320_PLATFORM_D3D )
			unsigned int resolutionWidth, resolutionHeight;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication;
	#endif
#endif
		};

		bool Initialize( const sInitializationParameters& i_initializationParameters );
		bool CleanUp();
	}
}

#endif	