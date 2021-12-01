#include "..\inc\ModelInfo.h"


//Default
ModelInfo::ModelInfo()
{
	pose = Pose();
	id = 0;

}

void ModelInfo::setID(int tag)
{
	id = tag;
}