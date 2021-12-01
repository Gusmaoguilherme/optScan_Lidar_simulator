#include "CommandlineParser.h"
#include "CommandConfig.h"
#include "stringHelper.h"

#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>

using namespace std::experimental::filesystem;

void read_directory(const std::string& name, std::vector<std::string>& v)
{
	std::string pattern(name);
	std::string fullpath = pattern;
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			//std::cout << fullpath+"\\"+data.cFileName << std::endl;
			v.push_back(fullpath + "\\" + data.cFileName);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}


void printUsage()
{
	//std::cerr << "\nUsage: " << argv0 << " [options]\n";
	std::cerr <<
		"App Options:\n"
		"   ? | help | --help					Print this usage message and exit.\n"
		"  -m | --merge <dir_of_merge_files>	Directory of files to be merged.\n"
		"  -t | --tolerance <float>				Tolerance of minimum distance between points.\n"
		"  -w | --write <output_file.xyz>	    Saves the merged cloud in specified .xyz output.\n"
		"  -c | --configure <config_file.txt>	File with configurations commands.\n"
		"  -n | --normalize						Fit cloud in a cube of edge length 2.\n"
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

		//recebe endereço dos arquivos para fusão
		else if (arg == "-m" || arg == "--merge")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return 0;
			}
			arguments.dir_meshFilesPath = args[++i];
			toAbsoluteIfNotEmpty(arguments.dir_meshFilesPath);
			read_directory(arguments.dir_meshFilesPath, arguments.meshFilesPath);
			arguments.Number_scans = arguments.meshFilesPath.size() - 2;	//numero de scans para merge
		}

		//recebe tolerância
		else if (arg == "-t" || arg == "--tolerance")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			std::string chosen_tolerance = args[++i];
			arguments.tolerance = std::stof(chosen_tolerance);
		}

		//endereço de saída
		else if (arg == "-w" || arg == "--write")
		{
			if (i == args.size() - 1)
			{
				std::cerr << "Option '" << arg << "' requires additional argument.\n";
				printUsage();
				return false;
			}
			arguments.outputPath = args[++i];
			toAbsoluteIfNotEmpty(arguments.outputPath);
		}

		//arquivo de comandos
		else if (arg == "-c" || arg == "--configure")
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
		else if (arg == "-n" || arg == "--normalize")
		{
			arguments.normalize = 'y';
		}
		else if (arg == "--ply")
		{
			arguments.ply = 'y';
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
