#include "Camera.h"

namespace eae6320
{
	namespace Camera
	{
		Camera::Camera()
		{
			s_fov = 0.0f;
			s_nearPlaneDist = 0.0f;
			s_farPlaneDist = 0.0f;
		}

		Camera::Camera(Math::cVector i_pos, Math::cQuaternion i_ori, float i_fov, float i_nearPlaneDist, float i_farPlaneDist)
		{
			s_pos = i_pos;
			s_ori = i_ori;
			s_fov = i_fov;
			s_nearPlaneDist = i_nearPlaneDist;
			s_farPlaneDist = i_farPlaneDist;
		}

		Camera::~Camera()
		{

		}

		Camera& Camera::operator=(Camera& i_other)
		{
			s_pos = i_other.Pos();
			s_ori = i_other.Ori();
			s_fov = i_other.fov();
			s_nearPlaneDist = i_other.nearPlaneDist();
			s_farPlaneDist = i_other.farPlaneDist();
			return *this;
		}

		Math::cVector Camera::Pos()
		{
			return s_pos;
		}

		Math::cQuaternion Camera::Ori()
		{
			return s_ori;
		}

		float Camera::fov()
		{
			return s_fov;
		}

		float Camera::nearPlaneDist()
		{
			return s_nearPlaneDist;
		}

		float Camera::farPlaneDist()
		{
			return s_farPlaneDist;
		}

		void Camera::Move(const Math::cVector& i_pos)
		{
			s_pos = s_pos + i_pos;
		}

		void Camera::Rotate(float i_rad, const Math::cVector i_axisNormalized)
		{
			eae6320::Math::cQuaternion temp(i_rad, i_axisNormalized);
			s_ori = s_ori * temp;
		}
	}
}