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
/*
 * Code responsible for the closest hit of the ray shot in raygeneration.cu to the material surface. 
 * Disturbance and measurement errors apllied here.
 */


#include "app_config.h"
#include <corecrt_math.h>

#include <optix.h>
#include <optixu/optixu_math_namespace.h>

#include "rt_function.h"
#include "per_ray_data.h"

#define CONVERT_METER 1.e-3f
#define POLYNOMIAL_A2_CONST -5.139e-6f*1000*1000
#define POLYNOMIAL_A1_CONST 0.000992f*1000
#define POLYNOMIAL_A0_CONST 15.26f

// Context global variables provided by the renderer system.

// Semantic variables.
rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(MetaPerRayData, thePrd, rtPayload, );
rtDeclareVariable(float, theIntersectionDistance, rtIntersectionDistance, );
rtDeclareVariable(int, objectID, , );
// Attributes.
rtDeclareVariable(optix::float3, varGeoNormal, attribute geometric_normal, );

// OptiX programming tip!
// This closest hit program only uses the geometric normal and the shading normal attributes.
// OptiX will remove all code from the intersection programs for unused attributes automatically.
// Note that the matching between attribute outputs from the intersection program and 
// the inputs in the closesthit and anyhit programs is done with the type (here float3) and
// the user defined attribute semantic (e.g. here NORMAL). 
// The actual variable name doesn't need to match but it's recommended for clarity.
 

RT_PROGRAM void closest_hit_meta()
{	
  // Transform the(unnormalized) object space normals into world space.
  // Information about light and shadow in phong.cu of sutil sdk

  thePrd.worldNormal = optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, varGeoNormal)); //Finds normal vector from the closest hit surface.

  float LIDAR_Error_Polynomial; //Equation of LIDAR sensor error model

  if (theIntersectionDistance < 60) // Maximum scanning range of chosen LiDAR sensor
  {
	  LIDAR_Error_Polynomial = POLYNOMIAL_A2_CONST *(theIntersectionDistance*theIntersectionDistance) + POLYNOMIAL_A1_CONST*(theIntersectionDistance) +POLYNOMIAL_A0_CONST; // The sensor error model in mm
	  thePrd.worldPos = theRay.origin + theRay.direction * (theIntersectionDistance + LIDAR_Error_Polynomial * CONVERT_METER + thePrd.noise*CONVERT_METER); // 3D position of the surface hit
	  thePrd.intersectDist = theIntersectionDistance + LIDAR_Error_Polynomial * CONVERT_METER + thePrd.noise*CONVERT_METER; // Total distance perceived by the virtual sensor 
																															// (including sensor error model and gaussian noise N(0,2.8)).
  }
  else //virtual sensor fails to detect outside of maximum distance range
  {
	  thePrd.worldPos = make_float3(0.0f); //sensor has no return
	  thePrd.intersectDist = 0; 

  }


  thePrd.objectID = objectID; //classification number from the 3D model hit
}
