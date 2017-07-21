#include "cSprite.h"

Engine::Graphics::cSprite::cSprite( const float i_position_left, const float i_position_right, const float i_position_top, const float i_position_bottom,
	const float i_texcoord_left, const float i_texcoord_right, const float i_texcoord_top, const float i_texcoord_bottom )
{
	m_position_screen.left = i_position_left;
	m_position_screen.right = i_position_right;
	m_position_screen.top = i_position_top;
	m_position_screen.bottom = i_position_bottom;

	m_textureCoordinates.left = i_texcoord_left;
	m_textureCoordinates.right = i_texcoord_right;
	m_textureCoordinates.top = i_texcoord_top;
	m_textureCoordinates.bottom = i_texcoord_bottom;
}
