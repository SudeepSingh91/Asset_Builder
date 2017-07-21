#ifndef MATH_CVECTOR_H
#define MATH_CVECTOR_H

namespace Engine
{
	namespace Math
	{
		class cVector
		{
		public:

			friend class cMatrix_transformation;
			friend class cQuaternion;

			float x() const;
			float y() const;
			float z() const;

			void x(const float i_x);
			void y(const float i_y);
			void z(const float i_z);

			cVector operator +(const cVector& i_rhs) const;
			cVector& operator +=(const cVector& i_rhs);

			cVector operator -(const cVector& i_rhs) const;
			cVector& operator -=(const cVector& i_rhs);
			cVector operator -() const;

			cVector operator *(const float i_rhs) const;
			cVector& operator *=(const float i_rhs);
			friend cVector operator *(const float i_lhs, const cVector& i_rhs);

			cVector operator /(const float i_rhs) const;
			cVector& operator /=(const float i_rhs);

			float GetLength() const;
			float Normalize();
			cVector CreateNormalized() const;

			friend float Dot(const cVector& i_lhs, const cVector& i_rhs);
			friend cVector Cross(const cVector& i_lhs, const cVector& i_rhs);

			bool operator ==(const cVector& i_rhs) const;
			bool operator !=(const cVector& i_rhs) const;

			cVector(const float i_x = 0.0f, const float i_y = 0.0f, const float i_z = 0.0f);

		private:

			float m_x, m_y, m_z;
		};

		cVector operator *(const float i_lhs, const cVector& i_rhs);
		float Dot(const cVector& i_lhs, const cVector& i_rhs);
		cVector Cross(const cVector& i_lhs, const cVector& i_rhs);
	}
}

#endif	
