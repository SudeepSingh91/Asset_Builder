#include "cVector.h"

#include <cmath>
#include "../Asserts/Asserts.h"

namespace
{
	const float s_epsilon = 1.0e-9f;
}

namespace Engine
{
	namespace Math
	{
		float cVector::x() const
		{
			return m_x;
		}

		float cVector::y() const
		{
			return m_y;
		}

		float cVector::z() const
		{
			return m_z;
		}

		void cVector::x(const float i_x)
		{
			m_x = i_x;
		}

		void cVector::y(const float i_y)
		{
			m_y = i_y;
		}

		void cVector::z(const float i_z)
		{
			m_z = i_z;
		}

		cVector cVector::operator +(const cVector& i_rhs) const
		{
			return cVector(m_x + i_rhs.m_x, m_y + i_rhs.m_y, m_z + i_rhs.m_z);
		}

		cVector& cVector::operator +=(const cVector& i_rhs)
		{
			m_x += i_rhs.m_x;
			m_y += i_rhs.m_y;
			m_z += i_rhs.m_z;
			return *this;
		}

		cVector Engine::Math::cVector::operator -(const cVector& i_rhs) const
		{
			return cVector(m_x - i_rhs.m_x, m_y - i_rhs.m_y, m_z - i_rhs.m_z);
		}

		cVector& cVector::operator -=(const cVector& i_rhs)
		{
			m_x -= i_rhs.m_x;
			m_y -= i_rhs.m_y;
			m_z -= i_rhs.m_z;
			return *this;
		}

		cVector cVector::operator -() const
		{
			return cVector(-m_x, -m_y, -m_z);
		}

		cVector cVector::operator *(const float i_rhs) const
		{
			return cVector(m_x * i_rhs, m_y * i_rhs, m_z * i_rhs);
		}

		cVector& cVector::operator *=(const float i_rhs)
		{
			m_x *= i_rhs;
			m_y *= i_rhs;
			m_z *= i_rhs;
			return *this;
		}

		cVector operator *(const float i_lhs, const Engine::Math::cVector& i_rhs)
		{
			return i_rhs * i_lhs;
		}

		cVector cVector::operator /(const float i_rhs) const
		{
			ASSERTF(std::abs(i_rhs) > s_epsilon, "Can't divide by zero");
			const float rhs_reciprocal = 1.0f / i_rhs;
			return cVector(m_x * rhs_reciprocal, m_y * rhs_reciprocal, m_z * rhs_reciprocal);
		}

		cVector& cVector::operator /=(const float i_rhs)
		{
			ASSERTF(std::abs(i_rhs) > s_epsilon, "Can't divide by zero");
			const float rhs_reciprocal = 1.0f / i_rhs;
			m_x *= rhs_reciprocal;
			m_y *= rhs_reciprocal;
			m_z *= rhs_reciprocal;
			return *this;
		}

		float cVector::GetLength() const
		{
			return std::sqrt((m_x * m_x) + (m_y * m_y) + (m_z * m_z));
		}

		float cVector::Normalize()
		{
			const float length = GetLength();
			ASSERTF(length > s_epsilon, "Can't divide by zero");
			operator /=(length);
			return length;
		}

		cVector cVector::CreateNormalized() const
		{
			const float length = GetLength();
			ASSERTF(length > s_epsilon, "Can't divide by zero");
			const float length_reciprocal = 1.0f / length;
			return cVector(m_x * length_reciprocal, m_y * length_reciprocal, m_z * length_reciprocal);
		}

		float Dot(const cVector& i_lhs, const cVector& i_rhs)
		{
			return (i_lhs.x() * i_rhs.x()) + (i_lhs.y() * i_rhs.y()) + (i_lhs.z() * i_rhs.z());
		}

		cVector Cross(const cVector& i_lhs, const cVector& i_rhs)
		{
			return cVector(
				(i_lhs.y() * i_rhs.z()) - (i_lhs.z() * i_rhs.y()),
				(i_lhs.z() * i_rhs.x()) - (i_lhs.x() * i_rhs.z()),
				(i_lhs.x() * i_rhs.y()) - (i_lhs.y() * i_rhs.x()));
		}

		bool cVector::operator ==(const cVector& i_rhs) const
		{
			return (m_x == i_rhs.m_x) & (m_y == i_rhs.m_y) & (m_z == i_rhs.m_z);
		}

		bool cVector::operator !=(const cVector& i_rhs) const
		{
			return (m_x != i_rhs.m_x) | (m_y != i_rhs.m_y) | (m_z != i_rhs.m_z);
		}

		cVector::cVector(const float i_x, const float i_y, const float i_z)
			:
			m_x(i_x), m_y(i_y), m_z(i_z)
		{

		}
	}
}