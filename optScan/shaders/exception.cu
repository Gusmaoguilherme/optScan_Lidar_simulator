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
  * Code in case of ray misbehavior.
  */

#include "app_config.h"

#include <optix.h>


rtBuffer<float4, 2> sysOutputBuffer; // X,Y,Z
rtBuffer<uchar4, 2> sysColorBuffer;	//RGB
rtDeclareVariable(uint2, theLaunchIndex, rtLaunchIndex, );

RT_PROGRAM void exception()
{
#if USE_DEBUG_EXCEPTIONS
  const unsigned int code = rtGetExceptionCode();
  if (RT_EXCEPTION_USER <= code)
  {
    rtPrintf("User exception %d at (%d, %d)\n", code - RT_EXCEPTION_USER, theLaunchIndex.x, theLaunchIndex.y);
  }
  else
  {
    rtPrintf("Exception code 0x%X at (%d, %d)\n", code, theLaunchIndex.x, theLaunchIndex.y);
  }

  sysOutputBuffer[theLaunchIndex] = make_float4(1000000.0f, 0.0f, 1000000.0f, 1.0f);  // X,Y,Z super magenta
  //sysColorBuffer[theLaunchIndex] = make_color(1.0f,0.0f,1.0f);	//RGB
#endif
}
