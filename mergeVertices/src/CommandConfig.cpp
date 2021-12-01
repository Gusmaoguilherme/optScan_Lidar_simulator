#include "CommandConfig.h"
#include "stringHelper.h"

#include <iostream>
#include <fstream>
#include <iterator>

/*Leio arquivo de conf
Crio string vector
retorno string vector*/

bool CommandConfig::Parse(std::string filename, std::vector<std::string>& output)
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

		std::vector<std::string> v{ stringHelper::explode(line, ' ') };//split string quando encontrar space

		for (auto n : v)  //fill output vector 
		{
			//std::cerr << n << std::endl;
			output.push_back(n);
		}

	}
	myfile.close();
	return true;
}
