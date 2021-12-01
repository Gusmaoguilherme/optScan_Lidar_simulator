#include "PCvertice.h"

void PCvertice::set3Dpoint(float x, float y, float z)
{

	position.x = x;
	position.y = y;
	position.z = z;

}

void PCvertice::setNormals(float nx, float ny, float nz)
{

	normals.x = nx;
	normals.y = ny;
	normals.z = nz;

}

void PCvertice::setColors(float r, float g, float b)
{

	colors.red = r;
	colors.green = g;
	colors.blue = b;

}

void PCvertice::setID(int tag)
{

	id = tag;

}




