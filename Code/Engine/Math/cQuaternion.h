#ifndef MATH_CQUATERNION_H
#define MATH_CQUATERNION_H

namespace Engine
{
	namespace Math
	{
		class cVector;
	}
}

namespace Engine
{
	namespace Math
	{
		class cQuaternion
		{
	
		public:

			cQuaternion operator *( const cQuaternion& i_rhs ) const;

			void Invert();
			cQuaternion CreateInverse() const;

			void Normalize();
			cQuaternion CreateNormalized() const;

			friend float Dot( const cQuaternion& i_lhs, const cQuaternion& i_rhs );

			cQuaternion();	
			cQuaternion( const float i_angleInRadians, const cVector& i_axisOfRotation_normalized );

		private:

			float m_w, m_x, m_y, m_z;

		private:

			cQuaternion( const float i_w, const float i_x, const float i_y, const float i_z );

			friend class cMatrix_transformation;
		};
	}
}

#endif	
