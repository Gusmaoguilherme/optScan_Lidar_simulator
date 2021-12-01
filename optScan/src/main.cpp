/*
 * Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.
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

 //-----------------------------------------------------------------------------
 //
 //
 //-----------------------------------------------------------------------------

#include "shaders/app_config.h"

#include "inc/Application.h"
#include "inc/CommandlineParser.h"
#include "inc/CameraPathParser.h"
#include "inc/Pose.h"
#include "inc/ModelInfo.h"

#include <sutil.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

using namespace std::experimental::filesystem;
using CommandlineParser::CommandlineArguments;


static void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << error << ": " << description << std::endl;
}

int main(int argc, char *argv[])
{
	std::vector<Pose> screenshotPoses;
	CommandlineArguments arguments;
	std::vector<std::string> allArgs(argv+1, argv + argc); //conversão argv para string

	// TODO: Use namespaces to house those parsing methods. 
	if (CommandlineParser::Parse(allArgs, arguments) == false) //Recebe argumentos com os comandos para o programa
		return 0;

	if (arguments.cameraFilePath.length() != 0)
	{
		if (CameraPathParser::Parse(arguments.cameraFilePath, screenshotPoses) == false) //recebe o arquivo de caminho de camera
			return 0;
	}	
	
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		error_callback(1, "GLFW failed to initialize.");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(arguments.windowWidth, arguments.windowHeight, "OptScan", NULL, NULL);  //cria janela
	if (!window)
	{
		error_callback(2, "glfwCreateWindow() failed.");
		glfwTerminate();
		return 2;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GL_NO_ERROR)
	{
		error_callback(3, "GLEW failed to initialize.");
		glfwTerminate();
		return 3;
	}

	Application* g_app = new Application(window, arguments.windowWidth, arguments.windowHeight,
		arguments.devices, arguments.stackSize, arguments.interop);	 //inicializa a classe application e as rotinas iniciais do Optix

	if (!g_app->isValid())
	{
		error_callback(4, "Application initialization failed.");
		glfwTerminate();
		return 4;
	}

	g_app->reshape(arguments.windowWidth, arguments.windowHeight);
	g_app->createScene(arguments.models);	//Cria a cena com os modelos recebidos

	std::srand(std::time(nullptr)); // use current time as seed for random generator
	const int random_variable = std::rand()%2;
	std::cerr << "Generating number: " << random_variable << "\n";

	//Executa escaneamento
	if (screenshotPoses.size() != 0)
	{
		int counter = 0;
		path outputPath(arguments.outputPath);
		create_directories(outputPath);

		for (auto pose : screenshotPoses)
		{
			std::string outputFile = absolute("output_" + std::to_string(counter) + ".txt", outputPath).string();

			std::string outputFile_RNG = absolute("output_RNG_" + std::to_string(counter) + ".txt", outputPath).string();  //arquivo de matriz de ruido
			
			std::cerr << "Generating file " << outputFile << "\n";

			g_app->screenshot(outputFile, outputFile_RNG, pose, arguments.format); //Escreve nuvem de pontos

			counter++;
		}
	}

	// Main loop

	if (arguments.hasGUI == true)
	{
		while (!glfwWindowShouldClose(window))
		{
			int currentWindowWidth, currentWindowHeight;

			glfwPollEvents(); // Render continuously.

			glfwGetFramebufferSize(window, &currentWindowWidth, &currentWindowHeight);

			g_app->reshape(currentWindowWidth, currentWindowHeight);

			g_app->guiNewFrame();

			//g_app->guiReferenceManual(); // DAR HACK The ImGui "Programming Manual" as example code.

			g_app->guiEventHandler(); // Currently only reacting on SPACE to toggle the GUI window.

			g_app->render();  // OptiX rendering and OpenGL texture update.
			g_app->display(); // OpenGL display.

			g_app->guiRender(); // Render all ImGUI elements at last.

			glfwSwapBuffers(window);

			//glfwWaitEvents(); // Render only when an event is happening. Needs some glfwPostEmptyEvent() to prevent GUI lagging one frame behind when ending an action.
		}
	}

	// Cleanup
	delete g_app;
	
	glfwTerminate();

	return 0;
}

