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

#include "shaders/app_config.h"

#include "Application.h"
#include "Float4Buffer.h"
#include "CameraPathParser.h"

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#include <optixu/optixu_quaternion_namespace.h>
#include <OptiXMesh.h>
#include <cuda.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
 // DAR Only for sutil::samplesPTXDir() and sutil::writeBufferToFile()
#include <sutil.h>

#include "MyAssert.h"
#include "RayTypes.h"
#include "BuffertoRender.h"
#include "shaders/Basiclight.h"
#include "..\inc\ModelInfo.h"

const char* const SAMPLE_NAME = "optScan";
std::string Meshpath="";
optix::Aabb aabb; //Axis-aligned bouding box

// This only runs inside the OptiX Advanced Samples location,
// unless the environment variable OPTIX_SAMPLES_SDK_PTX_DIR is set.
// A standalone application which should run anywhere would place the *.ptx files 
// into a subdirectory next to the executable and use a relative file path here!
static std::string ptxPath(std::string const& cuda_file)
{
	return std::string(sutil::samplesPTXDir()) + std::string("/") +
		std::string(SAMPLE_NAME) + std::string("_generated_") + cuda_file + std::string(".ptx");
}

Application::Application(GLFWwindow* window,
	const int width,
	const int height,
	const unsigned int devices,
	const unsigned int stackSize,
	const bool interop)
	: m_window(window)
	, m_width(width)
	, m_height(height)
	, m_devicesEncoding(devices)
	, m_stackSize(stackSize)
	, m_interop(interop)
{
	// Setup ImGui binding.
	ImGui::CreateContext();
	ImGui_ImplGlfwGL2_Init(window, true);
	// This initializes the GLFW part including the font texture.
	ImGui_ImplGlfwGL2_NewFrame();
	ImGui::EndFrame();

	ImGuiStyle& style = ImGui::GetStyle();
	
	// Style the GUI colors to a neutral greyscale with plenty of transaparency to concentrate on the image.
	// Change these RGB values to get any other tint.
	const float r = 1.0f;
	const float g = 1.0f;
	const float b = 1.0f;

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(r * 0.2f, g * 0.2f, b * 0.2f, 0.6f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(r * 0.2f, g * 0.2f, b * 0.2f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(r * 0.2f, g * 0.2f, b * 0.2f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(r * 0.4f, g * 0.4f, b * 0.4f, 0.4f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(r * 0.0f, g * 0.0f, b * 0.0f, 0.4f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(r * 0.4f, g * 0.4f, b * 0.4f, 0.4f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(r * 0.6f, g * 0.6f, b * 0.6f, 0.6f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(r * 0.6f, g * 0.6f, b * 0.6f, 0.6f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(r * 0.2f, g * 0.2f, b * 0.2f, 0.2f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(r * 0.2f, g * 0.2f, b * 0.2f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(r * 0.2f, g * 0.2f, b * 0.2f, 0.2f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(r * 0.4f, g * 0.4f, b * 0.4f, 0.4f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(r * 0.6f, g * 0.6f, b * 0.6f, 0.6f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(r * 0.4f, g * 0.4f, b * 0.4f, 0.4f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_Button] = ImVec4(r * 0.4f, g * 0.4f, b * 0.4f, 0.4f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(r * 0.6f, g * 0.6f, b * 0.6f, 0.6f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_Header] = ImVec4(r * 0.4f, g * 0.4f, b * 0.4f, 0.4f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(r * 0.6f, g * 0.6f, b * 0.6f, 0.6f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_Column] = ImVec4(r * 0.4f, g * 0.4f, b * 0.4f, 0.4f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(r * 0.6f, g * 0.6f, b * 0.6f, 0.6f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(r * 0.6f, g * 0.6f, b * 0.6f, 0.6f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(r * 1.0f, g * 1.0f, b * 1.0f, 1.0f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(r * 0.4f, g * 0.4f, b * 0.4f, 0.4f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(r * 0.6f, g * 0.6f, b * 0.6f, 0.6f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 0.8f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(r * 1.0f, g * 1.0f, b * 1.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(r * 0.8f, g * 0.8f, b * 0.8f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(r * 1.0f, g * 1.0f, b * 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(r * 0.5f, g * 0.5f, b * 0.5f, 1.0f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(r * 0.2f, g * 0.2f, b * 0.2f, 0.2f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(r * 1.0f, g * 1.0f, b * 0.0f, 1.0f); // Yellow
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(r * 1.0f, g * 1.0f, b * 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(r * 1.0f, g * 1.0f, b * 1.0f, 1.0f);

	// Renderer setup and GUI parameters.
	m_builder = std::string("Trbvh");

	// GLSL shaders objects and program. 
	m_glslVS = 0;
	m_glslFS = 0;
	m_glslProgram = 0;

	m_guiState = GUI_STATE_NONE;

	m_isWindowVisible = true;

	m_mouseSpeedRatio = 10.0f;

	//Meshfile(mesh_file);

	m_pinholeCamera.setViewport(m_width, m_height);

	initOpenGL();
	initOptiX(); // Sets m_isValid when OptiX initialization was successful.
}

Application::~Application()
{
	delete m_positionBuffer;
	delete m_normalBuffer;
	delete m_colorBuffer;
	delete m_noiseBuffer;

	// DAR FIXME Do any other destruction here.
	if (m_isValid)
	{
		m_context->destroy();
	}

	ImGui_ImplGlfwGL2_Shutdown();
	ImGui::DestroyContext();
}

void Application::Meshfile(std::string const& mesh_file)
{
	Meshpath = mesh_file;
}

bool Application::isValid() const
{
	return m_isValid;
}

void Application::reshape(int width, int height)
{
	if ((width != 0 && height != 0) && // Zero sized interop buffers are not allowed in OptiX.
		(m_width != width || m_height != height))
	{
		m_width = width;
		m_height = height;

		glViewport(0, 0, m_width, m_height);

		m_positionBuffer->setSize(m_width, m_height);
		m_normalBuffer->setSize(m_width, m_height);
		m_colorBuffer->setSize(m_width, m_height);
		m_noiseBuffer->setSize(m_width, m_height);

		
		m_pinholeCamera.setViewport(m_width, m_height);
	}
}

void Application::guiNewFrame()
{
	ImGui_ImplGlfwGL2_NewFrame();
}

void Application::guiReferenceManual()
{
	ImGui::ShowTestWindow();
}

void Application::guiRender()
{
	ImGui::Render();
	ImGui_ImplGlfwGL2_RenderDrawData(ImGui::GetDrawData());
}


void Application::getSystemInformation()
{
	unsigned int optixVersion;
	RT_CHECK_ERROR_NO_CONTEXT(rtGetVersion(&optixVersion));

	unsigned int major = optixVersion / 1000; // Check major with old formula.
	unsigned int minor;
	unsigned int micro;
	if (3 < major) // New encoding since OptiX 4.0.0 to get two digits micro numbers?
	{
		major = optixVersion / 10000;
		minor = (optixVersion % 10000) / 100;
		micro = optixVersion % 100;
	}
	else // Old encoding with only one digit for the micro number.
	{
		minor = (optixVersion % 1000) / 10;
		micro = optixVersion % 10;
	}
	std::cout << "Optscan Prototype "<< std::endl;
	std::cout << "OptiX " << major << "." << minor << "." << micro << std::endl;

	unsigned int numberOfDevices = 0;
	RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetDeviceCount(&numberOfDevices));
	std::cout << "Number of Devices = " << numberOfDevices << std::endl << std::endl;

	for (unsigned int i = 0; i < numberOfDevices; ++i)
	{
		char name[256];
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_NAME, sizeof(name), name));
		std::cout << "Device " << i << ": " << name << std::endl;

		int computeCapability[2] = { 0, 0 };
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY, sizeof(computeCapability), &computeCapability));
		std::cout << "  Compute Support: " << computeCapability[0] << "." << computeCapability[1] << std::endl;

		RTsize totalMemory = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_TOTAL_MEMORY, sizeof(totalMemory), &totalMemory));
		std::cout << "  Total Memory: " << (unsigned long long) totalMemory << std::endl;

		int clockRate = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_CLOCK_RATE, sizeof(clockRate), &clockRate));
		std::cout << "  Clock Rate: " << clockRate << " kHz" << std::endl;

		int maxThreadsPerBlock = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK, sizeof(maxThreadsPerBlock), &maxThreadsPerBlock));
		std::cout << "  Max. Threads per Block: " << maxThreadsPerBlock << std::endl;

		int smCount = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, sizeof(smCount), &smCount));
		std::cout << "  Streaming Multiprocessor Count: " << smCount << std::endl;

		int executionTimeoutEnabled = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_EXECUTION_TIMEOUT_ENABLED, sizeof(executionTimeoutEnabled), &executionTimeoutEnabled));
		std::cout << "  Execution Timeout Enabled: " << executionTimeoutEnabled << std::endl;

		int maxHardwareTextureCount = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MAX_HARDWARE_TEXTURE_COUNT, sizeof(maxHardwareTextureCount), &maxHardwareTextureCount));
		std::cout << "  Max. Hardware Texture Count: " << maxHardwareTextureCount << std::endl;

		int tccDriver = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_TCC_DRIVER, sizeof(tccDriver), &tccDriver));
		std::cout << "  TCC Driver enabled: " << tccDriver << std::endl;

		int cudaDeviceOrdinal = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_CUDA_DEVICE_ORDINAL, sizeof(cudaDeviceOrdinal), &cudaDeviceOrdinal));
		std::cout << "  CUDA Device Ordinal: " << cudaDeviceOrdinal << std::endl << std::endl;
	}
}

void Application::initOpenGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glViewport(0, 0, m_width, m_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glGenTextures(1, &m_hdrTexture);
	MY_ASSERT(m_hdrTexture != 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hdrTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// DAR ImGui has been changed to push the GL_TEXTURE_BIT so that this works. 
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	initGLSL();
}


void Application::initOptiX()
{
	try
	{
		getSystemInformation();

		m_context = optix::Context::create();

		// Select the GPUs to use with this context.
		unsigned int numberOfDevices = 0;
		RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetDeviceCount(&numberOfDevices));
		std::cout << "Number of Devices = " << numberOfDevices << std::endl << std::endl;

		std::vector<int> devices;

		int devicesEncoding = m_devicesEncoding; // Preserve this information, it can be stored in the system file.
		unsigned int i = 0;
		do
		{
			int device = devicesEncoding % 10;
			devices.push_back(device); // DAR FIXME Should be a std::set to prevent duplicate device IDs in m_devicesEncoding.
			devicesEncoding /= 10;
			++i;
		} while (i < numberOfDevices && devicesEncoding);

		m_context->setDevices(devices.begin(), devices.end());

		// Print out the current configuration to make sure what's currently running.
		devices = m_context->getEnabledDevices();
		for (size_t i = 0; i < devices.size(); ++i)
		{
			std::cout << "m_context is using local device " << devices[i] << ": " << m_context->getDeviceName(devices[i]) << std::endl;
		}
		std::cout << "OpenGL interop is " << ((m_interop) ? "enabled" : "disabled") << std::endl;
		
		initPrograms();
		initRenderer();

		m_positionBuffer = new Float4Buffer(m_context, "sysPositionsBuffer", m_interop);
		m_positionBuffer->setSize(m_width, m_height);
		m_normalBuffer = new Float4Buffer(m_context, "sysNormalsBuffer", m_interop);
		m_normalBuffer->setSize(m_width, m_height);
		m_colorBuffer = new Float4Buffer(m_context, "sysColorsBuffer", m_interop);
		m_colorBuffer->setSize(m_width, m_height);
		m_noiseBuffer = new Float4Buffer(m_context, "sysNoisesBuffer", m_interop);
		m_noiseBuffer->setSize(m_width, m_height);

		initScene();

		m_isValid = true; // If we get here with no exception, flag the initialization as successful. Otherwise the app will exit with error message.
	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}
}

void Application::initRenderer()
{
	try
	{
		m_context->setEntryPointCount(1); // 0 = render
		m_context->setRayTypeCount(3);    // 0 = radiance, 1 = shadow, 2 = meta see RayTypes

		m_context->setStackSize(m_stackSize);
		std::cout << "stackSize = " << m_stackSize << std::endl;

#if USE_DEBUG_EXCEPTIONS
		// Disable this by default for performance, otherwise the stitched PTX code will have lots of exception handling inside. 
		m_context->setPrintEnabled(true);
		//m_context->setPrintLaunchIndex(256, 256);
		m_context->setExceptionEnabled(RT_EXCEPTION_ALL, true);
#endif 

	    std::map<std::string, optix::Program>::const_iterator it = m_mapOfPrograms.find("raygeneration");
		MY_ASSERT(it != m_mapOfPrograms.end());
		m_context->setRayGenerationProgram(0, it->second); // entrypoint

		it = m_mapOfPrograms.find("exception");
		MY_ASSERT(it != m_mapOfPrograms.end());
		m_context->setExceptionProgram(0, it->second); // entrypoint

		it = m_mapOfPrograms.find("miss_radiance");
		MY_ASSERT(it != m_mapOfPrograms.end());
		m_context->setMissProgram(RayTypes::Radiance, it->second);

		it = m_mapOfPrograms.find("miss_meta");
		MY_ASSERT(it != m_mapOfPrograms.end());
		m_context->setMissProgram(RayTypes::Meta, it->second);

		// Default initialization. Will be overwritten on the first frame.
		m_context["sysCameraPosition"]->setFloat(0.0f, 0.0f, 1.0f);
		m_context["sysCameraU"]->setFloat(1.0f, 0.0f, 0.0f);
		m_context["sysCameraV"]->setFloat(0.0f, 1.0f, 0.0f);
		m_context["sysCameraW"]->setFloat(0.0f, 0.0f, -1.0f);

		//caracteristicas do raio
		m_context["radiance_ray_type"]->setUint(RayTypes::Radiance);
		m_context["shadow_ray_type"]->setUint(RayTypes::Shadow);
		m_context["meta_ray_type"]->setUint(RayTypes::Meta);

		m_context["scene_epsilon"]->setFloat(1.e-4f);

		//Still brewing this section
		//Escrevendo um buffer com 1 e 0 aleatoriamente e passando do host para device
			optix::Buffer m_noise = m_context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT, m_width, m_height);

			float* dst = static_cast<float*>(m_noise->map());	//habilita buffer para escrita no host

			for (unsigned int y = 0; y < m_height; ++y)
				{
					for (unsigned int x = 0; x < m_width; ++x)
					{
						float babs;
						const float random_variable = (float) std::rand()/RAND_MAX;
						babs = random_variable;
						*dst++ = static_cast<float>(babs);
					}
				}
		
			m_noise->unmap();	  //desabilita buffer para escrita no host

		m_context["scene_noise"]->setBuffer(m_noise); //cria flag para o device do buffer criado acima

		//End of brewer section

		setBuffertoRender(BuffertoRender::Color);	//default Buffer to Render
	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}
}


void Application::initScene()
{
	try
	{
		m_timer.restart();
		const double timeInit = m_timer.getTime();

		std::cout << "createScene()" << std::endl;
		//createScene();//geometry
		const double timeScene = m_timer.getTime();

		std::cout << "m_context->validate()" << std::endl;
		m_context->validate();
		const double timeValidate = m_timer.getTime();
		
		std::cout << "m_context->launch()" << std::endl;
		m_context->launch(0, 0, 0); // Dummy launch to build everything (entrypoint, width, height)
		const double timeLaunch = m_timer.getTime();

		std::cout << "initScene(): " << timeLaunch - timeInit << " seconds overall" << std::endl;
		std::cout << "{" << std::endl;
		std::cout << "  createScene() = " << timeScene - timeInit << " seconds" << std::endl;
		std::cout << "  validate()    = " << timeValidate - timeScene << " seconds" << std::endl;
		std::cout << "  launch()      = " << timeLaunch - timeValidate << " seconds" << std::endl;
		std::cout << "}" << std::endl;
		
	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}
}

bool Application::setBuffertoRender(int selectedBuffer)
{
	setBuffertoRender_flag = selectedBuffer;
	return true;
}

bool Application::render()
{
	bool repaint = false;

	try
	{
		optix::float3 cameraPosition;
		optix::float3 cameraU;
		optix::float3 cameraV;
		optix::float3 cameraW;
		
		bool cameraChanged = m_pinholeCamera.getFrustum(cameraPosition, cameraU, cameraV, cameraW);
		if (cameraChanged)
		{
			m_context["sysCameraPosition"]->setFloat(cameraPosition);
			m_context["sysCameraU"]->setFloat(cameraU);
			m_context["sysCameraV"]->setFloat(cameraV);
			m_context["sysCameraW"]->setFloat(cameraW);
		}

		//Não apagar codigo comentado a seguir. Usando para fazer caminhos de camera, até ter algo mais inteligente. 
		/*std::cerr << "I see pos:" << cameraPosition.x << ", " << cameraPosition.y << " and " << cameraPosition.z << "\n";
		std::cerr << "I see up:" << cameraU.x << ", " << cameraU.y << " and " << cameraU.z << "\n";
		std::cerr << "I see right:" << cameraV.x << ", " << cameraV.y << " and " << cameraV.z << "\n";
		std::cerr << "I see forward:" << cameraW.x << ", " << cameraW.y << " and " << cameraW.z << "\n"; */

		glActiveTexture(GL_TEXTURE0);
		m_context->launch(0, m_width, m_height);
		
		glBindTexture(GL_TEXTURE_2D, m_hdrTexture);

		if (setBuffertoRender_flag == BuffertoRender::Position)
		{
			m_positionBuffer->unpackBuffer();
		}
		else if (setBuffertoRender_flag == BuffertoRender::Normal)
		{
			m_normalBuffer->unpackBuffer();
		}
		else if (setBuffertoRender_flag == BuffertoRender::Color)
		{
			m_colorBuffer->unpackBuffer();
		}
		repaint = true; // Indicate that there is a new image.
	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}
	return repaint;
}

void Application::display()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hdrTexture);

	glUseProgram(m_glslProgram);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();

	glUseProgram(0);
}

//função responsavel pelo escaneamento
void Application::screenshot(std::string const& filename, std::string const& filenameRNG, Pose& pose, int format)
{
	/*std::cerr << "I see pos:" << pose.position.x << ", " << pose.position.y << " and " << pose.position.z << "\n";
	std::cerr << "I see up:" << pose.up.x << ", " << pose.up.y << " and " << pose.up.z << "\n";
	std::cerr << "I see right:" << pose.right.x << ", " << pose.right.y << " and " << pose.right.z << "\n";
	std::cerr << "I see forward:" << pose.forward.x << ", " << pose.forward.y << " and " << pose.forward.z << "\n";	*/

	try
	{
		//Importação para o contexto. Para o Optix, o vetor U é o right e o vetor V é o up
		m_context["sysCameraPosition"]->setFloat(pose.position);
		m_context["sysCameraU"]->setFloat(pose.right);
		m_context["sysCameraV"]->setFloat(pose.up);
		m_context["sysCameraW"]->setFloat(pose.forward);

		m_context->launch(0, m_width, m_height);

		//std::cerr << "My name is " << filename << "\n";
		sutil::writeBufferToFile(filename.c_str(), filenameRNG.c_str(), m_positionBuffer->optixBuffer(), m_normalBuffer->optixBuffer(), m_colorBuffer->optixBuffer(), format);	 //saving procedure
		std::cerr << "Wrote " << filename << std::endl;
	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}

}

// Helper functions:
void Application::checkInfoLog(const char *msg, GLuint object)
{
	GLint maxLength;
	GLint length;
	GLchar *infoLog;

	if (glIsProgram(object))
	{
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &maxLength);
	}
	else
	{
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &maxLength);
	}
	if (maxLength > 1)
	{
		infoLog = (GLchar *)malloc(maxLength);
		if (infoLog != NULL)
		{
			if (glIsShader(object))
			{
				glGetShaderInfoLog(object, maxLength, &length, infoLog);
			}
			else
			{
				glGetProgramInfoLog(object, maxLength, &length, infoLog);
			}
			//fprintf(fileLog, "-- tried to compile (len=%d): %s\n", (unsigned int)strlen(msg), msg);
			//fprintf(fileLog, "--- info log contents (len=%d) ---\n", (int) maxLength);
			//fprintf(fileLog, "%s", infoLog);
			//fprintf(fileLog, "--- end ---\n");
			std::cout << infoLog << std::endl;
			// Look at the info log string here...
			free(infoLog);
		}
	}
}


void Application::initGLSL()
{
	static const std::string vsSource =
		"#version 330\n"
		"layout(location = 0) in vec4 attrPosition;\n"
		"layout(location = 8) in vec2 attrTexCoord0;\n"
		"out vec2 varTexCoord0;\n"
		"void main()\n"
		"{\n"
		"  gl_Position  = attrPosition;\n"
		"  varTexCoord0 = attrTexCoord0;\n"
		"}\n";

	static const std::string fsSource =
		"#version 330\n"
		"uniform sampler2D samplerHDR;\n"
		"in vec2 varTexCoord0;\n"
		"layout(location = 0, index = 0) out vec4 outColor;\n"
		"void main()\n"
		"{\n"
		"  outColor = texture(samplerHDR, varTexCoord0);\n"
		"}\n";

	GLint vsCompiled = 0;
	GLint fsCompiled = 0;

	m_glslVS = glCreateShader(GL_VERTEX_SHADER);
	if (m_glslVS)
	{
		GLsizei len = (GLsizei)vsSource.size();
		const GLchar *vs = vsSource.c_str();
		glShaderSource(m_glslVS, 1, &vs, &len);
		glCompileShader(m_glslVS);
		checkInfoLog(vs, m_glslVS);

		glGetShaderiv(m_glslVS, GL_COMPILE_STATUS, &vsCompiled);
		MY_ASSERT(vsCompiled);
	}

	m_glslFS = glCreateShader(GL_FRAGMENT_SHADER);
	if (m_glslFS)
	{
		GLsizei len = (GLsizei)fsSource.size();
		const GLchar *fs = fsSource.c_str();
		glShaderSource(m_glslFS, 1, &fs, &len);
		glCompileShader(m_glslFS);
		checkInfoLog(fs, m_glslFS);

		glGetShaderiv(m_glslFS, GL_COMPILE_STATUS, &fsCompiled);
		MY_ASSERT(fsCompiled);
	}

	m_glslProgram = glCreateProgram();
	if (m_glslProgram)
	{
		GLint programLinked = 0;

		if (m_glslVS && vsCompiled)
		{
			glAttachShader(m_glslProgram, m_glslVS);
		}
		if (m_glslFS && fsCompiled)
		{
			glAttachShader(m_glslProgram, m_glslFS);
		}

		glLinkProgram(m_glslProgram);
		checkInfoLog("m_glslProgram", m_glslProgram);

		glGetProgramiv(m_glslProgram, GL_LINK_STATUS, &programLinked);
		MY_ASSERT(programLinked);

		if (programLinked)
		{
			glUseProgram(m_glslProgram);

			glUniform1i(glGetUniformLocation(m_glslProgram, "samplerHDR"), 0); // texture image unit 0

			glUseProgram(0);
		}
	}
}

void Application::guiEventHandler()
{
	ImGuiIO const& io = ImGui::GetIO();

	if (ImGui::IsKeyPressed(' ', false)) // Toggle the GUI window display with SPACE key.
	{
		m_isWindowVisible = !m_isWindowVisible;
	}
	//Set buffers to render. Key 1 - position, 2 - normal, 3 - color
	else if (ImGui::IsKeyPressed(GLFW_KEY_1, false))
	{
		setBuffertoRender(BuffertoRender::Position);
	}
	else if (ImGui::IsKeyPressed(GLFW_KEY_2, false))
	{
		setBuffertoRender(BuffertoRender::Normal);
	}
	else if (ImGui::IsKeyPressed(GLFW_KEY_3, false))
	{
		setBuffertoRender(BuffertoRender::Color);
	}

	const ImVec2 mousePosition = ImGui::GetMousePos(); // Mouse coordinate window client rect.
	const int x = int(mousePosition.x);
	const int y = int(mousePosition.y);

	switch (m_guiState)
	{
		case GUI_STATE_NONE:
			if (!io.WantCaptureMouse) // Only allow camera interactions to begin when not interacting with the GUI.
			{
				if (ImGui::IsMouseDown(0)) // LMB down event?
				{
					m_pinholeCamera.setBaseCoordinates(x, y);
					m_guiState = GUI_STATE_ORBIT;
				}
				else if (ImGui::IsMouseDown(1)) // RMB down event?
				{
					m_pinholeCamera.setBaseCoordinates(x, y);
					m_guiState = GUI_STATE_DOLLY;
				}
				else if (ImGui::IsMouseDown(2)) // MMB down event?
				{
					m_pinholeCamera.setBaseCoordinates(x, y);
					m_guiState = GUI_STATE_PAN;
				}
				else if (io.MouseWheel != 0.0f) // Mouse wheel zoom.
				{
					m_pinholeCamera.zoom(io.MouseWheel);
				}
			}
			break;

		case GUI_STATE_ORBIT:
			if (ImGui::IsMouseReleased(0)) // LMB released? End of orbit mode.
			{
				m_guiState = GUI_STATE_NONE;
			}
			else
			{
				m_pinholeCamera.orbit(x, y);
			}
			break;

		case GUI_STATE_DOLLY:
			if (ImGui::IsMouseReleased(1)) // RMB released? End of dolly mode.
			{
				m_guiState = GUI_STATE_NONE;
			}
			else
			{
				m_pinholeCamera.dolly(x, y);
			}
			break;

		case GUI_STATE_PAN:
			if (ImGui::IsMouseReleased(2)) // MMB released? End of pan mode.
			{
				m_guiState = GUI_STATE_NONE;
			}
			else
			{
				m_pinholeCamera.pan(x, y);
			}
			break;
	}
}


void Application::initPrograms()
{
	try
	{
		// First load all programs and put them into a map.
		// Programs which are reused multiple times can be queried from that map.
		// (This renderer does not put variables on program scope!)

		// Renderer
		m_mapOfPrograms["raygeneration"] = m_context->createProgramFromPTXFile(ptxPath("raygeneration.cu"), "raygeneration"); // entry point 0
		m_mapOfPrograms["exception"] = m_context->createProgramFromPTXFile(ptxPath("exception.cu"), "exception"); // entry point 0

		// RayType::Radiance
		m_mapOfPrograms["miss_radiance"] = m_context->createProgramFromPTXFile(ptxPath("miss.cu"), "miss_radiance");

		// RayType::Meta
		m_mapOfPrograms["miss_meta"] = m_context->createProgramFromPTXFile(ptxPath("miss.cu"), "miss_meta");
		m_mapOfPrograms["closest_hit_meta"] = m_context->createProgramFromPTXFile(ptxPath("closesthit.cu"), "closest_hit_meta");

		m_mapOfPrograms["cylinder_intersect"] = m_context->createProgramFromPTXFile(ptxPath("cylinder.cu"), "intersect");
		m_mapOfPrograms["cylinder_bounds"] = m_context->createProgramFromPTXFile(ptxPath("cylinder.cu"), "bounds");
		m_mapOfPrograms["cylinder_closest_hit_radiance"] = m_context->createProgramFromPTXFile(ptxPath("cylinder.cu"), "closest_hit_radiance");
	}

	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}
}

//void Application::initMaterials()
//{
//	try
//	{
//		// Create the main Material node
//
//		std::map<std::string, optix::Program>::const_iterator it;
//
//		// Used for all materials without cutout opacity. (Faster than using the cutout opacity material for everything.)
//		m_opaqueMaterial = m_context->createMaterial();
//
//		it = m_mapOfPrograms.find("closest_hit");
//		MY_ASSERT(it != m_mapOfPrograms.end());
//		m_opaqueMaterial->setClosestHitProgram(0, it->second); // raytype 0 == radiance
//		
//	}
//	catch (optix::Exception& e)
//	{
//		std::cerr << e.getErrorString() << std::endl;
//	}
//}

void Application::createCylinder(optix::Group top_group)
{
	optix::Geometry cylinder;

	cylinder = m_context->createGeometry();
	cylinder->setPrimitiveCount(1u);

	auto it = m_mapOfPrograms.find("cylinder_bounds");
	MY_ASSERT(it != m_mapOfPrograms.end());
	cylinder->setBoundingBoxProgram(it->second);

	it = m_mapOfPrograms.find("cylinder_intersect");
	MY_ASSERT(it != m_mapOfPrograms.end());
	cylinder->setIntersectionProgram(it->second);

	optix::Material mat = m_context->createMaterial();
	it = m_mapOfPrograms.find("closest_hit_meta");
	MY_ASSERT(it != m_mapOfPrograms.end());
	mat->setClosestHitProgram(RayTypes::Meta, it->second);

	it = m_mapOfPrograms.find("cylinder_closest_hit_radiance");
	MY_ASSERT(it != m_mapOfPrograms.end());
	mat->setClosestHitProgram(RayTypes::Radiance, it->second);

	optix::GeometryInstance gi = m_context->createGeometryInstance();
	gi->setGeometry(cylinder);
	gi->addMaterial(mat);
	gi->declareVariable("objectID")->setInt(666);

	float sphere_loc[3] = { 0, 0, -1.500-0.822f*0.5 };//(-0.911 - 1.2 - 2.699)
//+0.0083)};
	gi->declareVariable("center")->set3fv(&sphere_loc[0]);

	sphere_loc[0] = 10.f*0.5;
	gi->declareVariable("halfHeight")->set1fv(&sphere_loc[0]);

	sphere_loc[0] = 0.822f*0.5;
	gi->declareVariable("radius")->set1fv(&sphere_loc[0]);

	optix::GeometryGroup gg = m_context->createGeometryGroup();
	gg->setAcceleration(m_context->createAcceleration("Trbvh"));
	gg->addChild(gi);
					 
	optix::Matrix4x4 matrixTrans = optix::Matrix4x4::identity();
	optix::Transform trans = m_context->createTransform();
	trans->setMatrix(false, matrixTrans.getData(), matrixTrans.getData());
	trans->setChild(gg);

	top_group->addChild(trans);
}

//Cria cenário 3D com as especificações recebidas
void Application::createScene(std::vector<ModelInfo> const& Mesh_atrib)
{
//	initMaterials();
	optix::Program closestHitMeta = m_mapOfPrograms.find("closest_hit_meta")->second;

	//int id = 0;
	int current_mesh_atrib = 0;
	int total_meshes = Mesh_atrib.size();
	std::vector<optix::Transform> trans;

	optix::Group top_group = m_context->createGroup();

	top_group->setAcceleration(m_context->createAcceleration("Trbvh"));

	for (auto n : Mesh_atrib)
	{
		std::cerr<<"Position: " << std::to_string(n.pose.position.x) << ", " << std::to_string(n.pose.position.y) << ", " << std::to_string(n.pose.position.z) <<"; Scale: "<< n.pose.scale << "\n";
		std::cerr << "Rotation: " << std::to_string(n.pose.rotation.x) << ", " << std::to_string(n.pose.rotation.y) << ", " << std::to_string(n.pose.rotation.z) << ", " << std::to_string(n.pose.rotation.w) << "\n";
	}

	//Create a mathematical cylinder model
	//createCylinder(top_group);

	try
	{
		 //load meshes 
		for (int i = 0; i < total_meshes; i++)
		{
			//Transposing matrix
			
			optix::Transform trans1 = m_context->createTransform();
			optix::Matrix4x4 matrixTrans = Mesh_atrib[i].pose.getMatrixTRS();
			trans1->setMatrix(false, matrixTrans.getData(), matrixTrans.inverse().getData());
			trans.push_back(trans1);

			//Loading mesh
			OptiXMesh mesh;
			mesh.context = m_context;
			mesh.objectID = Mesh_atrib[i].id;
			mesh.closest_hit_meta = closestHitMeta;
			std::cerr << Mesh_atrib[i].meshfilePath.c_str() << "\n";
			loadMesh(Mesh_atrib[i].meshfilePath.c_str(), mesh);
			
			optix::float4 bbox_min = matrixTrans * make_float4(mesh.bbox_min,1);
			optix::float4 bbox_max = matrixTrans * make_float4(mesh.bbox_max, 1);

			aabb.set({ bbox_min.x,bbox_min.y,bbox_min.z }, { bbox_max.x,bbox_max.y,bbox_max.z });

			/*std::cerr << "Min_x: " << std::to_string(mesh.bbox_min.x) << ", Max_x: " << std::to_string(mesh.bbox_max.x) << "\n";
			std::cerr << "Min_y: " << std::to_string(mesh.bbox_min.y) << ", Max_y: " << std::to_string(mesh.bbox_max.y) << "\n";
			std::cerr << "Min_z: " << std::to_string(mesh.bbox_min.z) << ", Max_z: " << std::to_string(mesh.bbox_max.z) << "\n";  */

			optix::GeometryGroup geometry_group = m_context->createGeometryGroup();
			geometry_group->addChild(mesh.geom_instance);
			geometry_group->setAcceleration(m_context->createAcceleration("Trbvh"));
			current_mesh_atrib++;
			trans1->setChild(geometry_group);
		}

		for (auto n : trans)
		{
			top_group->addChild(n);
		}
		m_context["top_object"]->set(top_group);
		m_context["top_shadower"]->set(top_group);
		const float max_dim = fmaxf(aabb.extent(0), aabb.extent(1)); // max of x, y components

		//informações de iluminação
		BasicLight lights[] = {
		{ optix::make_float3(-0.5f,  0.25f, -1.0f), optix::make_float3(0.2f, 0.2f, 0.25f), 0 },
		{ optix::make_float3(-0.5f,  0.0f ,  1.0f), optix::make_float3(0.1f, 0.1f, 0.10f), 0 },
		{ optix::make_float3(0.5f,  0.5f ,  0.5f), optix::make_float3(0.7f, 0.7f, 0.65f), 1 }
		};
		lights[0].pos *= max_dim * 100.0f;
		lights[1].pos *= max_dim * 100.0f;
		lights[2].pos *= max_dim * 100.0f;

		optix::Buffer light_buffer = m_context->createBuffer(RT_BUFFER_INPUT);
		light_buffer->setFormat(RT_FORMAT_USER);
		light_buffer->setElementSize(sizeof(BasicLight));
		light_buffer->setSize(sizeof(lights) / sizeof(lights[0]));
		memcpy(light_buffer->map(), lights, sizeof(lights));
		light_buffer->unmap();

		m_context["lights"]->set(light_buffer);
		//std::cerr << light_buffer->getFormat()<<"\n"<<light_buffer->getElementSize()<< "\n";
	}
	catch (optix::Exception& e)
	{
		std::cerr << e.getErrorString() << std::endl;
	}
}


void Application::setAccelerationProperties(optix::Acceleration acceleration)
{
	// To speed up the acceleration structure build for triangles, skip calls to the bounding box program and
	// invoke the special splitting BVH builder for indexed triangles by setting the necessary acceleration properties.
	// Using the fast Trbvh builder which does splitting has a positive effect on the rendering performanc as well.
	if (m_builder == std::string("Trbvh") || m_builder == std::string("Sbvh"))
	{
		// This requires that the position is the first element and it must be float x, y, z.
		acceleration->setProperty("vertex_buffer_name", "attributesBuffer");
		MY_ASSERT(sizeof(VertexAttributes) == 48);
		acceleration->setProperty("vertex_buffer_stride", "48");

		acceleration->setProperty("index_buffer_name", "indicesBuffer");
		MY_ASSERT(sizeof(optix::uint3) == 12);
		acceleration->setProperty("index_buffer_stride", "12");
	}
}

