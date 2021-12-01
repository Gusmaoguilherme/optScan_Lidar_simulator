/* 
 * Copyright (c) 2013-2018, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "app_config.h"

#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optix_world.h>

#include "rt_function.h"
#include "per_ray_data.h"

#include <iostream>
#include <assert.h>
#include <stdio.h>


#include "random.h"

#include "rt_assert.h"
#include "helpers.h"

#include "Random123/philox.h"
#include "Random123/threefry.h"
#include "Random123/ReinterpretCtr.hpp"
#include "Random123/uniform.hpp"
#include "Random123/boxmuller.hpp"

rtBuffer<float4, 2> sysPositionsBuffer; 
rtBuffer<float4, 2> sysNormalsBuffer;	
rtBuffer<float4, 2> sysColorsBuffer;
rtBuffer<float, 2> scene_noise;


rtDeclareVariable(rtObject, top_object, , );

rtDeclareVariable(uint2, theLaunchDim,   rtLaunchDim, );
rtDeclareVariable(uint2, theLaunchIndex, rtLaunchIndex, );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(unsigned int, radiance_ray_type, , );
rtDeclareVariable(unsigned int, meta_ray_type, , );
rtDeclareVariable(float3, sysCameraPosition, , );
rtDeclareVariable(float3, sysCameraU, , );
rtDeclareVariable(float3, sysCameraV, , );
rtDeclareVariable(float3, sysCameraW, , );

rtDeclareVariable(float, time_view_scale, , ) = 1e-6f;
rtDeclareVariable(float3, angle, attribute angle, );

// Entry point for a pinhole camera.

RT_PROGRAM void raygeneration()
{

#ifdef TIME_VIEW
	clock_t t0 = clock();
#endif

	// The launch index is the pixel coordinate.
	// Note that launchIndex = (0, 0) is the bottom left corner of the image,
	// which matches the origin in the OpenGL texture used to display the result.
	const float2 pixel = make_float2(theLaunchIndex);
	// Sample the ray in the center of the pixel.
	const float2 fragment = pixel + make_float2(0.5f);
	// The launch dimension (set with rtContextLaunch) is the full client window in this demo's setup.
	float2 screen = make_float2(theLaunchDim);
	//float2 screen = make_float2(256,360);
	// Normalized device coordinates in range [-1, 1].

	screen.x = fmaxf(1.0f, screen.x - 1.0f);
	screen.y = fmaxf(1.0f, screen.y - 1.0f);

	//const float2 ndc = (fragment / screen) * 2.0f - 1.0f;
	//posição e angulação da visão esferica
	//Codigo original exemplo optix
	/*const float2 ndc = (pixel / screen) *
		make_float2(2.0f * M_PIf, M_PIf) + make_float2(M_PIf, 0);
	float3 angle = make_float3(cos(ndc.x) * sin(ndc.y),	-cos(ndc.y),sin(ndc.x) * sin(ndc.y));  */

	float totalVerticalAngle = M_PIf*((360.0f)/180.f);
	float totalHorizontalAngle = M_PIf*((300.0f)/180.f);

	/*const float2 ndc = (pixel / screen) * make_float2(totalHorizontalAngle, totalVerticalAngle) 
		- 0.5f * make_float2(totalHorizontalAngle, totalVerticalAngle);*/
	const float2 ndc = (pixel / screen) * make_float2(totalVerticalAngle, totalHorizontalAngle)
		- 0.5f * make_float2(totalVerticalAngle, totalHorizontalAngle);

	float3 angle = make_float3(
		cos(ndc.y) * sin(ndc.x), 
		sin(ndc.y),
		cos(ndc.y) * cos(ndc.x)
	);

	
	// Gerando numero aleatorio
	unsigned int seed = tea<16>(screen.x*theLaunchIndex.y + theLaunchIndex.x, ndc.x);

	typedef r123::Threefry4x64 CBRNG;

	CBRNG g;
	CBRNG::ctr_type  ctr = { { 0,6} };
	CBRNG::key_type key = { {seed} };


	ctr[0] = 4;
	CBRNG::ctr_type rand = g(ctr, key);
	
	float rand_00 = r123::u01<float>(rand[0]);
	float rand_01 = r123::u01<float>(rand[1]);
	float rand_m = (rand_00 + rand_01) * 0.5;

	float test = scene_noise[theLaunchIndex]; // testando passagem de dados do host para device

	r123::double2 ashiok = r123::boxmuller(rand[0],rand[1]);
  


	if (test > 0.5)
	{
		test = 0;
	}
	else
	{
		test = 1;
	} 

  const float3 origin   = sysCameraPosition;
//  const float3 direction = optix::normalize((ndc.x * sysCameraU + ndc.y * sysCameraV + sysCameraW));
  //direction for spheric view
 const float3 direction = optix::normalize(angle.x*optix::normalize(sysCameraU) +  angle.y*optix::normalize(sysCameraV) +  angle.z*optix::normalize(sysCameraW));

  MetaPerRayData metaData;
  metaData.angle = angle;
  metaData.noise = ashiok.x*.5;
    optix::Ray ray = optix::make_Ray(origin, direction, meta_ray_type, scene_epsilon, RT_DEFAULT_MAX);
  rtTrace(top_object, ray, metaData);

  
  RadiancePerRayData radianceData;
  radianceData.depth = 0;
  // TODO: Assess if we can just change ray.ray_type
  //optix::Ray radianceRay = optix::make_Ray(origin, direction, radiance_ray_type, scene_epsilon, RT_DEFAULT_MAX);
  optix::Ray radianceRay = optix::make_Ray(origin, direction, radiance_ray_type, scene_epsilon, RT_DEFAULT_MAX);
  rtTrace(top_object, radianceRay, radianceData);

  //printf("P: %f, %f, %f \n U: %f, %f, %f \n V: %f, %f, %f \n W: %f, %f, %f \n I: %f\n", sysCameraPosition.x, sysCameraPosition.y, sysCameraPosition.z, sysCameraU.x, sysCameraU.y, sysCameraU.z, sysCameraV.x, sysCameraV.y, sysCameraV.z, sysCameraW.x, sysCameraW.y, sysCameraW.z, metaData.intersectDist);

#ifdef TIME_VIEW
	  clock_t t1 = clock();

  float expected_fps = 1.0f;
  float pixel_time = (t1 - t0) * time_view_scale * expected_fps;
  sysColorsBuffer[theLaunchIndex] = make_float4(make_float3(pixel_time),0);

#else
  sysPositionsBuffer[theLaunchIndex] = make_float4(metaData.worldPos, (float)metaData.objectID); //Receives 3d points and label
  //sysPositionsBuffer[theLaunchIndex] = make_float4(metaData.worldPos, (float)metaData.intersectDist);
  sysNormalsBuffer[theLaunchIndex] = make_float4(metaData.worldNormal, (float)rand_m); //Receives normal vectors
  //sysNormalsBuffer[theLaunchIndex] = make_float4(angle,(float)rand_m);
  sysColorsBuffer[theLaunchIndex] = make_float4(radianceData.result, 0); //Receives RGB color vectors
#endif
}
