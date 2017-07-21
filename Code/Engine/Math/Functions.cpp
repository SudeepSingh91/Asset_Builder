#include "Functions.h"

const float Engine::Math::Pi = 3.141592654f;

float Engine::Math::ConvertDegreesToRadians( const float i_degrees )
{
	return i_degrees * Pi / 180.0f;
}
