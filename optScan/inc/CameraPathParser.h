#ifndef CAMERAPATHPARSER_H
#define CAMERAPATHPARSER_H

#include <string>
#include <vector>
#include "inc/Pose.h"

#include<optix.h>
#include <optixu/optixu_matrix_namespace.h>

namespace CameraPathParser
{
	bool Parse(std::string filename, std::vector<Pose>& output);
}
#endif