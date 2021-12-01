//Leio arquivo de conf
//Crio string vector
//retorno string vector
#ifndef COMMANDCONFIG_H
#define COMMANDCONFIG_H

#include <string>
#include <vector>

#include<optix.h>
#include <optixu/optixu_matrix_namespace.h>

//parametros para execução
namespace CommandConfig
{
	bool Parse(std::string filename, std::vector < std:: string > & output);
}
#endif