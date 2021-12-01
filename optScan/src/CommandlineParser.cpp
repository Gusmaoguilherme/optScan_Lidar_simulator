
#include "inc/CommandlineParser.h"
#include "inc/CommandConfig.h"
#include "inc/Pose.h"
#include "inc/stringHelper.h"
#include "inc/ModelInfo.h"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <direct.h>

using namespace std::experimental::filesystem;

void printUsage()
{
	//std::cerr << "\nUsage: " << argv0 << " [options]\n";
	std::cerr <<
		"App Options:\n"
		"   ? | help | --help								Print this usage message and exit.\n"
		"  -w | --width <int>								Window client width  (1024).\n"
		"  -h | --height <int>								Window client height (1024).\n"
		"  -d | --devices <int>								OptiX device selection, each decimal digit selects one device (3210).\n"
		"  -n | --nopbo										Disable OpenGL interop for the image display.\n"
		"     | --nogui										Disable graphical interface.\n"
		"  -s | --stack <int>								Set the OptiX stack size (1024) (debug feature).\n"
		"  -m | --mesh <mesh_file>							Specify path to mesh file to be loaded.\n"
		"  -c | --camera <camera_file>						Specify path to camera positions file to be loaded.\n"
		"  -o | --outputPath <path>							Specify screenshot output path.\n"
		"	  | --configure <config_file>					Specify series of commands for execution.\n"
		"	  | --position <x,y,z>							Specify translation position for current loading mesh.\n"
		"	  | --rotation <x,y,z,angle>					Specify rotation for current loading mesh in the form of a quartenion.\n"
		"	  | --scale <float>								Specify scale factor for current loading mesh.\n"
		"     | --id <int>									Specify identification tag for current loading mesh.\n"
		"     | --optParam <color,id>						Add color or/and classification info to the scans.\n"
		"App Keystrokes:\n"
		"  SPACE  Toggles ImGui display.\n"
		"\n"
		<< std::endl;
}

void toAbsoluteIfNotEmpty(std::string& path)
{
	if (path.length() == 0)
		return;

	path.swap(absolute(path).string());
}

bool CommandlineParser::Parse(const std::vector<std::string>& args, CommandlineArguments& arguments)
{
	 
	// Parse the command line parameters.
	for (int i = 0; i < args.size(); ++i)
	{
		const std::string arg(args[i]);

		if (arg == "--help" || arg == "help" || arg == "?")
		{
			printUsage();
			return false;				 
		}

		// largura
		else if (arg == "-w" || arg == "--width")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return 0;
			}
			arguments.windowWidth = stoi(args[++i]);
		}

		//altura
		else if (arg == "-h" || arg == "--height")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return 0;
			}
			arguments.windowHeight = stoi(args[++i]);
		}

		//dispositivos
		else if (arg == "-d" || arg == "--devices")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			arguments.devices = stoi(args[++i]);
		}


		else if (arg == "-s" || arg == "--stack")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			arguments.stackSize = stoi(args[++i]);
		}


		else if (arg == "-n" || arg == "--nopbo")
		{
			arguments.interop = false;
		}


		else if (arg == "--nogui")
		{
			// TODO: Evaluate if we should set interop=false when hasGUI=false
			arguments.hasGUI = false;
		}

		//recebe o endereço da malha alvo
		else if (arg == "-m" || arg == "--mesh")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}

			std::string absol = args[++i];
			toAbsoluteIfNotEmpty(absol);
			//std::cerr << absol << "/n";
			ModelInfo newModel = ModelInfo();
			newModel.meshfilePath = absol;
			arguments.models.push_back(newModel);
			//std::cerr << "iD default: " << arguments.models.back().id << "\n";
			//std::cerr << "Position x default: " << arguments.models.back().pose.position.x << "\n";
		}

		//Endereço de saída
		else if (arg == "-o" || arg == "--outputPath")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			
			std::string absol = args[++i];// +"scans";
			arguments.outputPath = absol;
			std::cerr << "outputPath: " << arguments.outputPath << "\n";
			toAbsoluteIfNotEmpty(arguments.outputPath);
			
		}

		//Endereço do txt com caminho de camera
		else if (arg == "-c" || arg == "--camera")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}

			arguments.cameraFilePath = args[++i];
			toAbsoluteIfNotEmpty(arguments.cameraFilePath);
		}

		//recebe o arquivo com comandos de execução
		else if (arg == "--configure")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}

			std::string configFilePath = args[++i];
			toAbsoluteIfNotEmpty(configFilePath);
			std::vector<std::string> commands;

			CommandConfig::Parse(configFilePath, commands); //Função de configuração

			//Recursão com CommandlineParse()
			CommandlineParser::Parse(commands, arguments);
			
		}

		//Posição da malha no mundo
		else if (arg == "--position")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			optix::float3 new_position;
			std::string mesh_position = args[++i];
			std::vector<std::string> floats{ stringHelper::explode(mesh_position, ',') }; //split string em comma

			if (floats.size() != 3)
			{
				std::cerr << "Could not parse vector " << mesh_position;
				return false;
			}

			try
			{
				
				new_position.x = std::stof(floats[0], nullptr);
				new_position.y = std::stof(floats[1], nullptr);
				new_position.z = std::stof(floats[2], nullptr);
				arguments.models.back().pose.position = new_position;
			}
			catch (std::invalid_argument ex)
			{
				std::cerr << "Could not parse vector " << mesh_position;
			}

		}

		//Escala da malha
		else if (arg == "--scale")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			float new_scale;
			std::string mesh_scale = args[++i];
			new_scale = std::stof(mesh_scale);
			arguments.models.back().pose.scale = new_scale;
		}

		//parametros de rotação
		else if (arg == "--rotation")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			optix::float4 new_rotation;
			std::string mesh_rotation = args[++i];
			std::vector<std::string> floats{ stringHelper::explode(mesh_rotation, ',') }; //split string em comma

			if (floats.size() != 4)
			{
				std::cerr << "Could not parse vector " << mesh_rotation;
				return false;
			}

			try
			{

				new_rotation.x = std::stof(floats[0], nullptr);
				new_rotation.y = std::stof(floats[1], nullptr);
				new_rotation.z = std::stof(floats[2], nullptr);
				new_rotation.w = std::stof(floats[3], nullptr);
				arguments.models.back().pose.rotation = new_rotation;
			}
			catch (std::invalid_argument ex)
			{
				std::cerr << "Could not parse vector " << mesh_rotation;
			}

		}

		//classificação
		else if (arg == "--id")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			int tag;
			std::string mesh_tag = args[++i];
			tag = std::stoi(mesh_tag);
			arguments.models.back().id = tag;
		}

		//Parametros adicionais na nuvem de saída. Caso não seja chamada, o formato default (0) é xyz,normal
		else if (arg == "--optParam")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}

			std::string Format = args[++i];
			std::vector<std::string> param{ stringHelper::explode(Format, ',') }; //split string em comma
			
			
			if (param.size() > 2)
			{
				std::cerr << "Only optional parameters are color and/or id.\n";
				printUsage();
				return false;
			}
			else if (param.size() == 1) 
			{
				param.push_back("not");
			}

			//Verifica opções improprias 
			if (param[0] == "color" || param[0] == "not" || param[0] == "id")
			{
				
				if (param[1] == "color" || param[1] == "not" || param[1] == "id")
				{
					
					if(((std::find(param.begin(),param.end(),"color")) != param.end()) && ((std::find(param.begin(), param.end(), "id")) != param.end()))
						arguments.format = 3;//formato xyz,normal,color,id
					else 
						if (((std::find(param.begin(), param.end(), "color")) != param.end())) arguments.format = 1; //formato xyz,normal,color
						else if (((std::find(param.begin(), param.end(), "id")) != param.end())) arguments.format = 2; //formato xyz,normal,id
					
				}
				else
				{
					std::cerr << "Only optional parameters are color and/or id.\n";
					printUsage();
					return false;
				}
			}
			else
			{
				std::cerr << "Only optional parameters are color and/or id.\n";
				printUsage();
				return false;
			}

			
			
				std::cerr << "Optional parameter " << param[0] << " chosen.\n";
				std::cerr << "Optional parameter " << param[1] << " chosen.\n";
				std::cerr << "Optional parameter " << arguments.format << " chosen.\n";
			
		}
		else
		{
			std::cerr << "Unknown option '" << arg << "'\n";
			printUsage();
			return false;
		}
	}

	return true;
}
