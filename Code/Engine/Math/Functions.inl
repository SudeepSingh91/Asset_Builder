#ifndef MATH_FUNCTIONS_INL
#define MATH_FUNCTIONS_INL

#include "Functions.h"

#include "../Asserts/Asserts.h"

template<typename tUnsignedInteger>
	tUnsignedInteger Engine::Math::RoundUpToMultiple( const tUnsignedInteger i_value, const tUnsignedInteger i_multiple )
{
	ASSERTF( i_multiple != 0, "Zero isn't a valid multiple" );
	ASSERTF( i_multiple > 0, "The multiple must be positive" );
	const tUnsignedInteger returnValue = ( ( i_value + i_multiple - 1 ) / i_multiple ) * i_multiple;
	ASSERT( ( returnValue % i_multiple ) == 0 );
	return returnValue;
}

template<typename tUnsignedInteger>
	inline tUnsignedInteger Engine::Math::RoundUpToMultiple_powerOf2( const tUnsignedInteger i_value,
		const tUnsignedInteger i_multipleWhichIsAPowerOf2 )
{
	ASSERTF( i_multipleWhichIsAPowerOf2 != 0, "Zero isn't a valid multiple" );
	ASSERTF( i_multipleWhichIsAPowerOf2 > 0, "The multiple must be positive" );
	const tUnsignedInteger nonLeadingBits = i_multipleWhichIsAPowerOf2 - 1;
	ASSERTF( ( i_multipleWhichIsAPowerOf2 && ( i_multipleWhichIsAPowerOf2 & nonLeadingBits ) ) == 0, "The multiple must be a power-of-2" );
	const tUnsignedInteger returnValue = ( i_value + nonLeadingBits ) & ~nonLeadingBits;
	ASSERT( ( returnValue % i_multipleWhichIsAPowerOf2 ) == 0 );
	return returnValue;
}

#endif	