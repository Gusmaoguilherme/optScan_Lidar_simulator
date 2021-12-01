/*
* Copyright (c) 2018 NVIDIA CORPORATION. All rights reserved.
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

#include <optix_world.h>
#include "per_ray_data.h"

using namespace optix;

rtDeclareVariable(float3, center, , );
rtDeclareVariable(float, halfHeight, , );
rtDeclareVariable(float, radius, , );

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

rtDeclareVariable(RadiancePerRayData, thePrd, rtPayload, );

template<bool use_robust_method>
static __device__
void intersect_sphere(void)
{
	float3 O = ray.origin - center;
	float3 D = ray.direction;

	float b = dot(O, D);
	float c = dot(O, O) - radius * radius;
	float disc = b * b - c;
	if (disc > 0.0f)
	{
		float sdisc = sqrtf(disc);
		float root1 = (-b - sdisc);

		bool do_refine = false;

		float root11 = 0.0f;

		if (use_robust_method && fabsf(root1) > 10.f * radius)
		{
			do_refine = true;
		}

		if (do_refine)
		{
			// refine root1
			float3 O1 = O + root1 * ray.direction;
			b = dot(O1, D);
			c = dot(O1, O1) - radius * radius;
			disc = b * b - c;

			if (disc > 0.0f)
			{
				sdisc = sqrtf(disc);
				root11 = (-b - sdisc);
			}
		}

		bool check_second = true;
		if (rtPotentialIntersection(root1 + root11))
		{
			shading_normal = geometric_normal = (O + (root1 + root11)*D) / radius;
			if (rtReportIntersection(0))
				check_second = false;
		}
		if (check_second)
		{
			float root2 = (-b + sdisc) + (do_refine ? root1 : 0);
			if (rtPotentialIntersection(root2))
			{
				shading_normal = geometric_normal = (O + root2 * D) / radius;
				rtReportIntersection(0);
			}
		}
	}
}

template<bool use_robust_method>
static __device__
void intersect_cylinder(void)
{
	float3 O = ray.origin - center;
	float3 D = ray.direction;

	float a = D.x * D.x + D.z * D.z;
	float b = 2 * (O.x * D.x + O.z * D.z);
	float c = (O.x * O.x + O.z * O.z) - radius * radius;

	float disc = b * b - 4 * a*c;

	float3 hit_p, offset;

	if (disc > 0.0f)
	{
		float root1, root2;

		float sdisc = copysign(sqrtf(disc), b);
		float q = (-b - sdisc) / 2.0;

		root1 = q / a;

		if (q != 0)
		{
			root2 = c / q;
		}
		else
		{
			root2 = root1;
		}

		if (root1 < 0) root1 = root2;
		if (root2 < 0) root2 = root1;

		float final_root = min(root1, root2);
		float3 hit_p = ray.origin + final_root * D;
		float3 dummy_normal = hit_p;

		if (abs(center.y - hit_p.y) < halfHeight)
		{
			dummy_normal.y = 0;
			dummy_normal = normalize(dummy_normal);

			if (rtPotentialIntersection(final_root))
			{
				shading_normal = geometric_normal = dummy_normal;
				rtReportIntersection(0);
			}
		}
	}
}


RT_PROGRAM void robust_intersect(int primIdx)
{
	intersect_cylinder<true>();
}

RT_PROGRAM void intersect(int primIdx)
{
	intersect_cylinder<false>();
}

RT_PROGRAM void closest_hit_radiance()
{
	thePrd.result = make_float3(1.0, 0.0, 0.0);
}			 

RT_PROGRAM void bounds(int, float result[6])
{
	const float3 rad = make_float3(radius, halfHeight, radius);

	optix::Aabb* aabb = (optix::Aabb*)result;

	if (rad.x > 0.0f && !isinf(rad.x))
	{
		aabb->m_min = center - rad;
		aabb->m_max = center + rad;
	}
	else
	{
		aabb->invalidate();
	}
}

