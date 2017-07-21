#ifndef MATH_CMATRIX_TRANSFORMATION_H
#define MATH_CMATRIX_TRANSFORMATION_H

namespace Engine
{
	namespace Math
	{
		class cVector;
		class cQuaternion;
	}
}

namespace Engine
{
	namespace Math
	{
		class cMatrix_transformation
		{

		public:

			static cMatrix_transformation CreateWorldToCameraTransform(
				const cQuaternion& i_cameraOrientation, const cVector& i_cameraPosition );
			static cMatrix_transformation CreateCameraToProjectedTransform(
				const float i_fieldOfView_y, const float i_aspectRatio,
				const float i_z_nearPlane, const float i_z_farPlane );

			cMatrix_transformation();
			cMatrix_transformation( const cQuaternion& i_rotation, const cVector& i_translation );

		private:

			float m_00, m_10, m_20, m_30,
				m_01, m_11, m_21, m_31,
				m_02, m_12, m_22, m_32,
				m_03, m_13, m_23, m_33;

		private:

			cMatrix_transformation(
				const float i_00, const float i_10, const float i_20, const float i_30,
				const float i_01, const float i_11, const float i_21, const float i_31,
				const float i_02, const float i_12, const float i_22, const float i_32,
				const float i_03, const float i_13, const float i_23, const float i_33 );
		};
	}
}

#endif	
