#include "inc/CameraPathParser.h"
#include "inc/Pose.h"
#include "inc/stringHelper.h"

#include <iostream>
#include <fstream>
#include <iterator>

//Carrega o caminho de camera

bool parseFloat3(std::string strEncoded, optix::float3& output)
{
	//std::cerr << strEncoded << std::endl;

	std::vector<std::string> floats{ stringHelper::explode(strEncoded, ',') }; //split string em comma

	if (floats.size() != 3)
	{
		std::cerr << "Could not parse vector " << strEncoded;
		return false;
	}

	try
	{
		output.x = std::stof(floats[0], nullptr);
		output.y = std::stof(floats[1], nullptr);
		output.z = std::stof(floats[2], nullptr);
	}
	catch (std::invalid_argument ex)
	{
		return false;
	}
	
	return true;
}

bool CameraPathParser::Parse(std::string filename, std::vector<Pose>& output)
{
	std::ifstream myfile(filename.c_str());
						
	if (myfile.good() == false)
	{ 
		std::cerr << "Could not open file " << filename;
		return false;
	}

	int lineCount = 0;
	std::string line;
			
	while (getline(myfile, line))
	{
		lineCount++;
		//std::cerr << line << "\n";

		std::vector<std::string> v { stringHelper::explode(line, '\t') };//split string em tab

		if (v.size() != 4)
		{
			std::cerr << "Could not parse line " << lineCount << "\n";
			std::cerr << line << "\n";
			continue;
		}

		bool success = true;

		Pose pose;

		success = 
			parseFloat3(v[0], pose.position) &&
			parseFloat3(v[1], pose.right) &&
			parseFloat3(v[2], pose.up) &&
			parseFloat3(v[3], pose.forward);

		if (success == false)
			continue;

		output.push_back(pose);
	}
				
	myfile.close();

	return true;
}
