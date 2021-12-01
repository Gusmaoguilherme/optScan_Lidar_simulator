#include "..\inc\Pose.h"

//default
Pose::Pose()
{
	position = {0.0f,0.0f,0.0f};
	rotation = { 1.0f,0.0f,0.0f,0.0f};
	scale = 1.0f;
}

Pose::Pose(optix::float3 mesh_position)
{
	position = mesh_position;
}

void Pose::setScale(float scale_factor)
{
	
	scale = scale_factor;

}

void Pose::setRotation(optix::float4 rotation_quat)
{
	
	rotation = rotation_quat;

}

optix::Matrix4x4 Pose::getMatrixTRS() const
{
		//Transposing matrix
		std::cerr << "Position: " << position.x << ", " << position.y << ", " << position.z << "; Scale: " << scale << "\n";
		std::cerr << "Rotation: " << rotation.x << ", " << rotation.y << ", " << rotation.z << ", " << rotation.w << "\n";	   
		optix::float3 translation = position;
		optix::float3 axis = optix::make_float3(rotation.x, rotation.y, rotation.z);
		optix::Quaternion rotation_quaternion(axis,rotation.w);

		float scaleInfo[16] =
		{
			scale, 0.0f,  0.0f,  0.0f,
			0.0f,  scale, 0.0f,  0.0f,
			0.0f,  0.0f,  scale, 0.0f,
			0.0f,  0.0f,  0.0f,  1.0f
		};

		float rotationInfo[16];
		rotation_quaternion.toMatrix(rotationInfo);

		// util para visualizar a matriz on the fly para debbug
		/*std::cerr << "\n " << rotationInfo[0] << " " << rotationInfo[1] << " " << rotationInfo[2] << " " << rotationInfo[3] << "\n";
		std::cerr << "\n " << rotationInfo[4] << " " << rotationInfo[5] << " " << rotationInfo[6] << " " << rotationInfo[7] << "\n";
		std::cerr << "\n " << rotationInfo[8] << " " << rotationInfo[9] << " " << rotationInfo[10] << " " << rotationInfo[11] << "\n";
		std::cerr << "\n " << rotationInfo[12] << " " << rotationInfo[13] << " " << rotationInfo[14] << " " << rotationInfo[15] << "\n";	*/

		float translationInfo[16] =
		{
			1.0f, 0.0f, 0.0f, translation.x,
			0.0f, 1.0f, 0.0f, translation.y,
			0.0f, 0.0f, 1.0f, translation.z,
			0.0f, 0.0f, 0.0f, 1.0f
		};

	optix::Matrix4x4 matrixS(scaleInfo), matrixR(rotationInfo), matrixT(translationInfo);
	return matrixT * matrixR *matrixS;
}
