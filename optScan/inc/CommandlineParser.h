
#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>
#include <vector>
#include "Pose.h"
#include "ModelInfo.h"

// Parametros de comando

namespace CommandlineParser
{
	// Default parameters
	class CommandlineArguments
	{
	public:
		int windowHeight = 1024;
		int windowWidth = 1024;
		int devices = 3210; // Decimal digits encode OptiX device ordinals. Default 3210 means to use all four first installed devices, when available.

		bool interop = true; // Use OpenGL interop Pixel-Bufferobject to display the resulting image. Disable this when running on multi-GPU or TCC driver mode.
		bool hasGUI = true;
		int stackSize = 1024; // Command line parameter just to be able to find the smallest working size.

		std::string outputPath;
		std::string cameraFilePath;
		std::string configFilePath;
		int format = 0;	//quais parametros para nuvem. 0 - xyz,normal | 1 - xyz,normal,color | 2 - xyz,normal,id | 3 - xyz,normal,color,id 
		std::vector<ModelInfo> models;
	};

	bool Parse(const std::vector<std::string>& args, CommandlineArguments& arguments);
}
#endif