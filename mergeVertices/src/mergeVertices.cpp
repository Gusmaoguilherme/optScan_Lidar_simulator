#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "pointsHelper.h"
#include "PCvertice.h"
#include "CommandlineParser.h"
#include "merge.h"

using namespace std;
using CommandlineParser::CommandlineArguments;
using namespace GEO;

int main(int argc, char** argv) {
	if (argc < 3)
	{
		cout << "./mergeVertices directory_of_files_to_merge tolerance output.xyz" << endl;
		return 1;
	}
	try 
	{
		clock_t globalBegin = clock();

		std::vector<std::string> allArgs(argv + 1, argv + argc); //conversão argv para string
		CommandlineArguments arguments;

		GEO::initialize();

		CmdLine::import_arg_group("standard");
		CmdLine::import_arg_group("algo");

		if (CommandlineParser::Parse(allArgs, arguments) == false) //argumentos de comando
			return 0;

		cout << "number of scans: " << arguments.Number_scans<< endl;
		cout << "tolerance: " << arguments.tolerance << endl;
		cout << "clouds path: " << arguments.dir_meshFilesPath << endl;
		cout << "output file: " << arguments.outputPath << endl;

		std::vector<PCvertice> cloudOutput;
		index_t nb_pointsout;

		//FILL DATA FROM XYZ FILE

		point_store all_points;

		merge::pointclouds(arguments,cloudOutput, nb_pointsout, all_points);

		//WRITE OUTPUT 	*/
		std::string outfilename = arguments.outputPath;
		cout << "writing merged point cloud" << endl;
		pointsHelper::write_cloud(cloudOutput, outfilename,arguments.format1,arguments.normalize, arguments.ply,all_points);
		cout << "DONE" << endl;
		clock_t globalEnd = clock();
		cout << "EXECUTION TIME: " << double(globalEnd - globalBegin) / CLOCKS_PER_SEC << endl;

	}
	catch (const std::exception& e) {
		std::cerr << "Received an exception: " << e.what() << std::endl;
		return 1;
	}
	std::getchar();
	return 0;
}
