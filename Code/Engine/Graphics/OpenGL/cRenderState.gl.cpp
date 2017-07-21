#include "../cRenderState.h"
#include "../OpenGL/Includes.h"
#include "../../Asserts/Asserts.h"

void Engine::Graphics::cRenderState::Bind() const
{
	if ( IsAlphaTransparencyEnabled() )
	{
		glEnable( GL_BLEND );
		ASSERT( glGetError() == GL_NO_ERROR );
		glBlendEquation( GL_FUNC_ADD );
		ASSERT( glGetError() == GL_NO_ERROR );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		ASSERT( glGetError() == GL_NO_ERROR );
	}
	else
	{
		glDisable( GL_BLEND );
		ASSERT( glGetError() == GL_NO_ERROR );
	}
	if ( IsDepthBufferingEnabled() )
	{
		glEnable( GL_DEPTH_TEST );
		ASSERT( glGetError() == GL_NO_ERROR );
		glDepthFunc( GL_LESS );
		ASSERT( glGetError() == GL_NO_ERROR );
		glDepthMask( GL_TRUE );
		ASSERT( glGetError() == GL_NO_ERROR );
	}
	else
	{
		glDisable( GL_DEPTH_TEST );
		ASSERT( glGetError() == GL_NO_ERROR );
		glDepthMask( GL_FALSE );
		ASSERT( glGetError() == GL_NO_ERROR );
	}
	if ( ShouldBothTriangleSidesBeDrawn() )
	{
		glDisable( GL_CULL_FACE );
		ASSERT( glGetError() == GL_NO_ERROR );
	}
	else
	{
		glEnable( GL_CULL_FACE );
		ASSERT( glGetError() == GL_NO_ERROR );
		glFrontFace( GL_CCW );
		ASSERT( glGetError() == GL_NO_ERROR );
	}
}

bool Engine::Graphics::cRenderState::CleanUp()
{
	return true;
}

bool Engine::Graphics::cRenderState::InitializeFromBits()
{
	return true;
}
