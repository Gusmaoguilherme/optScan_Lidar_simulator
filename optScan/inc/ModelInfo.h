#ifndef MODELINFO_H
#define MODELINFO_H

#include <Pose.h>
//Classe com os argumentos referentes a classificação e caracteristicas espaciais de um ponto

class ModelInfo
{
public:
	int id;
	Pose pose;
	std::string meshfilePath;

	ModelInfo();
	void setID(int tag);

};

#endif