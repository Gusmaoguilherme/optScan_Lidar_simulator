#ifndef FLOAT4BUFFER_H
#define FLOAT4BUFFER_H

#include <GL/glew.h>
#include <GL/wglew.h>

#include <optix.h>
#include <optixu/optixpp_namespace.h>

class Float4Buffer
{
public:
	Float4Buffer(optix::Context& context, const char* name, bool interop);

	// TODO: Implement destructor

	void setSize(int width, int height);

	void unpackBuffer();

	optix::Buffer optixBuffer();

private:
	bool _interop;

	optix::Context& _context;

	GLuint _pboBuffer;

	optix::Buffer _optixBuffer;
};

#endif