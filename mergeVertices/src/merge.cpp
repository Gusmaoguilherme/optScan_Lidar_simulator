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
//ferramentas para merge

bool merge::pointclouds(CommandlineArguments & arguments, std::vector<PCvertice>& cloudOutput, GEO::index_t & nb_pointsout, point_store& all_points)
{
	try{	
		for (index_t i = 1; i <= arguments.Number_scans; i++)
		{
			if (i == 1)
			{
				//std::vector<PCvertice> cloud1;
				std::vector<PCvertice> cloud2;

				//index_t nb_points1;
				index_t nb_points2;
				//carrega arquivos xyz

				
				cout << "loading point clouds" << endl;
				pointsHelper::load_points(cloudOutput, arguments.meshFilesPath[i + 1], nb_pointsout,arguments.format1);
				pointsHelper::load_points(cloud2, arguments.meshFilesPath[i + 2], nb_points2, arguments.format2);

				if (arguments.format1 != arguments.format2)
				{
					Logger::err("I/O")
						<< "Differents parameters format of point clouds"
						<< std::endl;
					return false;
				}

				cout << "DONE" << endl;

				///////////////////////////////////////////////////////////


				double tolerance = arguments.tolerance;
				//cout << "tolerance: " << tolerance << endl;


				//FILL DATA FROM XYZ FILE
				//FILL POINTS
				//FILL NORMALS
				//FILL COLORS
				//FILL IDS

				pointsHelper::fill_data(cloudOutput, cloud2, nb_pointsout, nb_points2);

				//Apenas exibindo valores
				/*cout <<"3d point: " <<output.vertices.point(251236).x << " " << output.vertices.point(251236).y << " "<<output.vertices.point(251236).z << endl;
				//cout << "3d point: " <<cloudOutput[251236].normals.x << " " << cloudOutput[251236].colors.green << " " << cloudOutput[251236].id << endl;
				cout << "N point: " << outputNormals[251236].x << " " << outputNormals[251236].y << " " << outputNormals[251236].z << endl;
				cout << "C point: " << outputColors[251236].red << " " << outputColors[251236].green << " " << outputColors[251236].blue << endl;
				cout << "id point: " << outputIDs[251236] << endl;	*/

				//DETECT DUPLICATED VERTICES

				cout << "detecting duplicated vertices" << endl;
				cout << "Total vertices: " << cloudOutput.size() << endl;
				/*cout << "sizeof(PCvertice) = " << sizeof(PCvertice) << endl;
				cout << "stride = " << sizeof(PCvertice) / sizeof(double) << endl;	 */

				GEO::vector<index_t> old2new(cloudOutput.size());
				Geom::colocate(&cloudOutput[0].position.x, 3, cloudOutput.size(), old2new, tolerance, sizeof(PCvertice) / sizeof(double));
				cout << "stride: " <<sizeof(PCvertice) / sizeof(double) << endl;
				cout << "DONE" << endl;

				cout << "deleting duplicated vertices" << endl;
				pointsHelper::delete_duplicates(cloudOutput, old2new, nb_pointsout,arguments.Number_scans, arguments.processo_atual, all_points);
				cout << "DONE" << endl;
			}
			else
			{
				std::vector<PCvertice> cloud2;

				index_t  nb_points2;

				//carrega arquivos xyz

				cout << "loading point clouds" << endl;
				pointsHelper::load_points(cloud2, arguments.meshFilesPath[i + 2], nb_points2, arguments.format2);

				if (arguments.format1 != arguments.format2)
				{
					Logger::err("I/O")
						<< "Differents parameters format of point clouds"
						<< std::endl;
					return false;
				}
				
				cout << "DONE" << endl;

				///////////////////////////////////////////////////////////


				double tolerance = arguments.tolerance;
				//cout << "tolerance: " << tolerance << endl;


				//FILL DATA FROM XYZ FILE
				//FILL POINTS
				//FILL NORMALS
				//FILL COLORS
				//FILL IDS

				pointsHelper::fill_data(cloudOutput, cloud2, nb_pointsout, nb_points2);

				//Apenas exibindo valores
				/*cout <<"3d point: " <<output.vertices.point(251236).x << " " << output.vertices.point(251236).y << " "<<output.vertices.point(251236).z << endl;
				//cout << "3d point: " <<cloudOutput[251236].normals.x << " " << cloudOutput[251236].colors.green << " " << cloudOutput[251236].id << endl;
				cout << "N point: " << outputNormals[251236].x << " " << outputNormals[251236].y << " " << outputNormals[251236].z << endl;
				cout << "C point: " << outputColors[251236].red << " " << outputColors[251236].green << " " << outputColors[251236].blue << endl;
				cout << "id point: " << outputIDs[251236] << endl;	*/

				//DETECT DUPLICATED VERTICES

				cout << "detecting duplicated vertices" << endl;
				cout << "Total vertices: " << cloudOutput.size() << endl;
				/*cout << "sizeof(PCvertice) = " << sizeof(PCvertice) << endl;
				cout << "stride = " << sizeof(PCvertice) / sizeof(double) << endl; */

				GEO::vector<index_t> old2new(cloudOutput.size());
				Geom::colocate(&cloudOutput[0].position.x, 3, cloudOutput.size(), old2new, tolerance, sizeof(PCvertice) / sizeof(double));

				cout << "DONE" << endl;

				cout << "deleting duplicated vertices" << endl;
				pointsHelper::delete_duplicates(cloudOutput, old2new, nb_pointsout, arguments.Number_scans, arguments.processo_atual, all_points);
				cout << "DONE" << endl;

			}
		}

	}
	catch (const std::exception& ex)
	{
		Logger::err("I/O") << ex.what() << std::endl;
		return false;
	}
	return true;
}
