//Leio arquivo de conf
//Crio string vector
//retorno string vector
#ifndef COMMANDCONFIG_H
#define COMMANDCONFIG_H

#include <string>
#include <vector>


namespace CommandConfig
{
	bool Parse(std::string filename, std::vector < std:: string > & output);
}
#endif