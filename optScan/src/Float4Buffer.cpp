#include "Float4Buffer.h"

#include "MyAssert.h"
#include "GLHelper.h"

#include <iostream>

Float4Buffer::Float4Buffer(optix::Context& context, const char* name, bool interop) : _context(context), _optixBuffer(nullptr)
{
	_interop = interop;

	if (_interop == true)
	{
		glCreateBuffers(1, &_pboBuffer);
		MY_ASSERT(_pboBuffer != 0);

		// Buffer size must be > 0 or OptiX can't create a buffer from it.
		// RGBA32F from byte offset 0 in the pixel unpack buffer.
		glNamedBufferData(_pboBuffer, 1 * 1 * sizeof(float) * 4, nullptr, GL_STREAM_READ);
		GLHelper::CheckError();
	}

	_optixBuffer = (_interop == true) ? 
		_context->createBufferFromGLBO(RT_BUFFER_INPUT_OUTPUT, _pboBuffer) : 
		_context->createBuffer(RT_BUFFER_INPUT_OUTPUT);

	_optixBuffer->setFormat(RT_FORMAT_FLOAT4);
	_optixBuffer->setSize(1, 1);

	_context[name]->set(_optixBuffer);
}

void Float4Buffer::setSize(int width, int height)
{
	try
	{
		_optixBuffer->setSize(width, height);

		if (_interop == true)
		{
			_optixBuffer->unregisterGLBuffer(); // Must unregister or CUDA won't notice the size change and crash.
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _optixBuffer->getGLBOId());
			glBufferData(GL_PIXEL_UNPACK_BUFFER, _optixBuffer->getElementSize() * width * height, nullptr, GL_STREAM_DRAW);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			_optixBuffer->registerGLBuffer();
		}
	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}
}

void Float4Buffer::unpackBuffer()
{
	RTsize width, height;
	_optixBuffer->getSize(width, height);

	if (_interop == true)
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _optixBuffer->getGLBOId());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei) width, (GLsizei) height, 0, GL_RGBA, GL_FLOAT, (void*)0); // RGBA32F from byte offset 0 in the pixel unpack buffer.
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	else
	{
		const void* data = _optixBuffer->map(0, RT_BUFFER_MAP_READ);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei) width, (GLsizei) height, 0, GL_RGBA, GL_FLOAT, data);
		_optixBuffer->unmap();
	}
}

optix::Buffer Float4Buffer::optixBuffer()
{
	return _optixBuffer;
}
