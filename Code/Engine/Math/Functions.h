/*
	This file contains math-related functions
*/

#ifndef EAE6320_MATH_FUNCTIONS_H
#define EAE6320_MATH_FUNCTIONS_H

namespace eae6320
{
	namespace Math
	{
		// Interface
		//==========

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

#endif	// EAE6320_MATH_FUNCTIONS_H
