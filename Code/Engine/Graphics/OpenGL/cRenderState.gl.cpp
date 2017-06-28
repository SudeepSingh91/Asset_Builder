#include "../cRenderState.h"
#include "../OpenGL/Includes.h"
#include "../../Asserts/Asserts.h"

void eae6320::Graphics::cRenderState::Bind() const
{
	if ( IsAlphaTransparencyEnabled() )
	{
		glEnable( GL_BLEND );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		glBlendEquation( GL_FUNC_ADD );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	else
	{
		glDisable( GL_BLEND );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	if ( IsDepthBufferingEnabled() )
	{
		glEnable( GL_DEPTH_TEST );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		glDepthFunc( GL_LESS );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		glDepthMask( GL_TRUE );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	else
	{
		glDisable( GL_DEPTH_TEST );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		glDepthMask( GL_FALSE );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	if ( ShouldBothTriangleSidesBeDrawn() )
	{
		glDisable( GL_CULL_FACE );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	else
	{
		glEnable( GL_CULL_FACE );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		glFrontFace( GL_CCW );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
}

bool eae6320::Graphics::cRenderState::CleanUp()
{
	return true;
}

bool eae6320::Graphics::cRenderState::InitializeFromBits()
{
	return true;
}
