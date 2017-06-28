#include "cMyGame.h"

eae6320::cMyGame::~cMyGame()
{

}

bool eae6320::cMyGame::Initialize()
{
	const char* const path = "data/Meshes/Car.bin";
	const char* const path2 = "data/Meshes/Platform.bin";
	const char* const path3 = "data/Meshes/Cylinder.bin";

	const char* const matpath1 = "data/Materials/mat1.bin";
	const char* const matpath2 = "data/Materials/mat2.bin";
	const char* const matpath3 = "data/Materials/mat3.bin";
	const char* const matpath4 = "data/Materials/mat4.bin";
	
	m1.s_mesh = new eae6320::Mesh::Mesh;
	m2.s_mesh = new eae6320::Mesh::Mesh;
	m3.s_mesh = new eae6320::Mesh::Mesh;
	m4.s_mesh = new eae6320::Mesh::Mesh;
	m5.s_mesh = new eae6320::Mesh::Mesh;
	m6.s_mesh = new eae6320::Mesh::Mesh;
	m7.s_mesh = new eae6320::Mesh::Mesh;
	m8.s_mesh = new eae6320::Mesh::Mesh;
	mat1 = new eae6320::Graphics::Materials::Material;
	mat2 = new eae6320::Graphics::Materials::Material;
	mat3 = new eae6320::Graphics::Materials::Material;
	mat4 = new eae6320::Graphics::Materials::Material;

	s_camPos = new eae6320::Math::cVector(0.0f, 5.0f, 20.0f);
	s_Camera = new eae6320::Camera::Camera(*s_camPos, s_camOri, eae6320::Math::ConvertDegreesToRadians(60.0f), 0.1f, 100.0f);
	spr1 = new eae6320::Graphics::cSprite(-0.3f, 0.3f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f);
	spr1->Initialize();
	bool wereThereErrors = false;

	if (!m1.s_mesh->LoadFile(path))
	{
		wereThereErrors = true;
	}
	if (!m2.s_mesh->LoadFile(path2))
	{
		wereThereErrors = true;
	}
	if (!m3.s_mesh->LoadFile(path2))
	{
		wereThereErrors = true;
	}
	if (!m4.s_mesh->LoadFile(path3))
	{
		wereThereErrors = true;
	}
	if (!m5.s_mesh->LoadFile(path3))
	{
		wereThereErrors = true;
	}
	if (!m6.s_mesh->LoadFile(path3))
	{
		wereThereErrors = true;
	}
	if (!m7.s_mesh->LoadFile(path2))
	{
		wereThereErrors = true;
	}
	if (!m8.s_mesh->LoadFile(path2))
	{
		wereThereErrors = true;
	}
	if (!mat1->Load(matpath1))
	{
		wereThereErrors = true;
	}
	if (!mat2->Load(matpath2))
	{
		wereThereErrors = true;
	}
	if (!mat3->Load(matpath3))
	{
		wereThereErrors = true;
	}
	if (!mat4->Load(matpath4))
	{
		wereThereErrors = true;
	}
	m1.s_mat = mat1;
	m2.s_mat = mat2;
	m3.s_mat = mat2;
	m4.s_mat = mat3;
	m5.s_mat = mat3;
	m6.s_mat = mat3;
	m7.s_mat = mat2;
	m8.s_mat = mat2;
	eae6320::Graphics::SetCamera(*s_Camera);
	m3.s_pos.z -= 24.0f;
	m7.s_pos.z -= 48.0f;
	m8.s_pos.z -= 72.0f;
	eae6320::Graphics::SetMesh(m3);
	m4.s_pos.z -= 20.0f;
	m4.s_pos.x += 5.0f;
	eae6320::Graphics::SetMesh(m4);
	m5.s_pos.z -= 35.0f;
	m5.s_pos.x -= 5.0f;
	eae6320::Graphics::SetMesh(m5);
	m6.s_pos.z -= 43.0f;
	m6.s_pos.x += 5.0f;
	eae6320::Graphics::SetMesh(m6);
	return !wereThereErrors;
}

void eae6320::cMyGame::DrawMesh()
{
	float offset[2] = { 0.0f, 0.0f };
	{
		if (UserInput::IsKeyPressed('A'))
			offset[0] -= 4.0f;
		if (UserInput::IsKeyPressed('D'))
			offset[0] += 4.0f;
		offset[1] -= 60.0f;
	}
	{
		const float speed_unitsPerSecond = 0.2f;
		const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
		
		offset[0] *= offsetModifier;
		offset[1] *= offsetModifier;
	}
	eae6320::Math::cVector tempVec(0.0f, 0.0f, offset[1]);
	s_Camera->Move(tempVec);

	eae6320::Math::cVector tempx(1.0f, 0.0f, 0.0f);
	eae6320::Math::cVector tempy(0.0f, 1.0f, 0.0f);

	m1.s_pos.z += offset[1];
	m1.s_pos.x += offset[0];

	eae6320::Graphics::SetMesh(m1);
	eae6320::Graphics::SetMesh(m2);
	eae6320::Graphics::SetMesh(m3);
	eae6320::Graphics::SetMesh(m4);
	eae6320::Graphics::SetMesh(m5);
	eae6320::Graphics::SetMesh(m6);
	eae6320::Graphics::SetMesh(m7);
	eae6320::Graphics::SetMesh(m8);
	eae6320::Graphics::SetSprite(spr1, mat4);
	eae6320::Graphics::SetCamera(*s_Camera);
}

bool eae6320::cMyGame::CleanUp()
{
	bool wereThereErrors = false;
	if (m1.s_mesh && !m1.s_mesh->CleanUp())
	{
		return false;
	}
	if (m1.s_mesh)
	{
		delete m1.s_mesh;
	}
	if (m2.s_mesh && !m2.s_mesh->CleanUp())
	{
		return false;
	}
	if (m2.s_mesh)
	{
		delete m2.s_mesh;
	}
	if (m3.s_mesh && !m3.s_mesh->CleanUp())
	{
		return false;
	}
	if (m3.s_mesh)
	{
		delete m3.s_mesh;
	}
	if (m4.s_mesh && !m4.s_mesh->CleanUp())
	{
		return false;
	}
	if (m4.s_mesh)
	{
		delete m4.s_mesh;
	}
	if (m5.s_mesh && !m5.s_mesh->CleanUp())
	{
		return false;
	}
	if (m5.s_mesh)
	{
		delete m5.s_mesh;
	}
	if (m6.s_mesh && !m6.s_mesh->CleanUp())
	{
		return false;
	}
	if (m6.s_mesh)
	{
		delete m6.s_mesh;
	}
	if (m7.s_mesh)
	{
		delete m7.s_mesh;
	}
	if (m8.s_mesh)
	{
		delete m8.s_mesh;
	}
	if (mat1)
	{
		mat1->CleanUp();
		delete mat1;
	}
	if (mat2)
	{
		mat2->CleanUp();
		delete mat2;
	}
	if (mat3)
	{
		mat3->CleanUp();
		delete mat3;
	}
	if (mat4)
	{
		mat4->CleanUp();
		delete mat4;
	}
	if (s_camPos)
	{
		delete s_camPos;
	}
	if (s_Camera)
	{
		delete s_Camera;
	}
	if (spr1)
	{
		spr1->CleanUp();
		delete spr1;
	}
	return !wereThereErrors;
}
