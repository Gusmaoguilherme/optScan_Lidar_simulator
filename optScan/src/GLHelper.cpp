#include "..\inc\GLHelper.h"

#include <iostream>
#include <GL/glew.h>

void GLHelper::CheckError()
{
	GLenum err = glGetError();

	if (err != GL_NO_ERROR)
	{
		const GLubyte* msg = gluErrorString(err);
		std::cerr << "OpenGL error:" << msg << "\n";
	}
}
