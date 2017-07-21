#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

namespace Engine
{
	namespace Math
	{
		extern const float Pi;

		float ConvertDegreesToRadians( const float i_degrees );
		template<typename tUnsignedInteger>
			tUnsignedInteger RoundUpToMultiple( const tUnsignedInteger i_value, const tUnsignedInteger i_multiple );
		template<typename tUnsignedInteger>
			tUnsignedInteger RoundUpToMultiple_powerOf2( const tUnsignedInteger i_value,
				const tUnsignedInteger i_multipleWhichIsAPowerOf2 );
	}
}

#include "Functions.inl"

#endif
