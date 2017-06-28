#ifndef CAMERA_H
#define CAMERA_H

#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "../Math/cMatrix_transformation.h"

namespace eae6320
{
	namespace Camera
	{
		class Camera
		{
		public:
			Camera();
			Camera(const Math::cVector i_pos, const Math::cQuaternion i_ori, const float i_fov, const float i_nearPlaneDist, const float i_farPlaneDist);
			~Camera();

			Camera& operator=(Camera& i_other);

			Math::cVector Pos();
			Math::cQuaternion Ori();
			float fov();
			float nearPlaneDist();
			float farPlaneDist();

			void Move(const Math::cVector& i_pos);
			void Rotate(float i_rad, const Math::cVector i_axisNormalized);

		private:
			Math::cVector s_pos;
			Math::cQuaternion s_ori;
			float s_fov;
			float s_nearPlaneDist;
			float s_farPlaneDist;
		};
	}
}
#endif