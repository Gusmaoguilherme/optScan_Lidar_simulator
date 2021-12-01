#ifndef POSE_H
#define POSE_H

//Classe referente as caracteristicas espaciais de uma determinada malha. Pode ser usada para as informações de camera.

#include<optix.h>
#include <optixu/optixu_matrix_namespace.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_quaternion_namespace.h>
#include <vector>
#include <iostream>

class Pose
{
public:
	optix::float3 position;	//posicao no mundo
	float scale;
	optix::float3 up;
	optix::float3 right;
	optix::float3 forward;
	optix::float4 rotation;

	Pose();
	Pose(optix::float3);
	void setScale(float scale_factor);
	void setRotation(optix::float4 rotation_quat);
	optix::Matrix4x4 getMatrixTRS() const;
};

#endif