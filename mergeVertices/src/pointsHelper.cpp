#include "pointsHelper.h"


using namespace std;

using namespace GEO;

//ferramentas para manuseio da nuvem de pontos

//Aloca as caracteristicas dos pontos
static PCvertice pointsHelper::alloc_points(const LineInput& in)
{
	PCvertice vertex;
	int f = in.nb_fields();

	if (f == 6)	   //parametros xyz, normal
	{
		vertex.set3Dpoint(in.field_as_double(0), in.field_as_double(1), in.field_as_double(2));
		vertex.setNormals(in.field_as_double(3), in.field_as_double(4), in.field_as_double(5));
		vertex.setColors(0,0,0);
		vertex.setID(0);
	}
	else if (f == 7)   //parametros xyz, normal,id
	{
		vertex.set3Dpoint(in.field_as_double(0), in.field_as_double(1), in.field_as_double(2));
		vertex.setNormals(in.field_as_double(3), in.field_as_double(4), in.field_as_double(5));
		vertex.setColors(0,0,0);
		vertex.setID(in.field_as_int(6));
	}
	else if (f == 9)	 //parametros xyz, normal, color
	{
		vertex.set3Dpoint(in.field_as_double(0), in.field_as_double(1), in.field_as_double(2));
		vertex.setNormals(in.field_as_double(3), in.field_as_double(4), in.field_as_double(5));
		vertex.setColors(in.field_as_double(6), in.field_as_double(7), in.field_as_double(8));
		vertex.setID(0);
	}
	else if (f == 10)	 //parametros xyz, normal, color, id
	{
		vertex.set3Dpoint(in.field_as_double(0), in.field_as_double(1), in.field_as_double(2));
		vertex.setNormals(in.field_as_double(3), in.field_as_double(4), in.field_as_double(5));
		vertex.setColors(in.field_as_double(6), in.field_as_double(7), in.field_as_double(8));
		vertex.setID(in.field_as_double(9));
	}
	
	return vertex;
}

//Carrega arquivos
bool pointsHelper::load_points(std::vector<PCvertice>& points, const std::string& filename, index_t& nb_points, int& format)
{
	try
	{
		LineInput in(filename);
		
		if (!in.OK())
		{
			return false;
		}
		nb_points = 0;
		in.get_line();

		while (strncmp(in.current_line(), "//", 2) == 0) //verifica presença de cabeçarios
			in.get_line();

		in.get_fields();
		if (in.nb_fields() == 1)   //caso arquivo forneça número de vértices
		{
			index_t cur = 0;
			nb_points = in.field_as_uint(0);
			cout << nb_points << endl;  
			points.resize(nb_points);
			while (!in.eof() && in.get_line())
			{
				in.get_fields();
				if (cur >= nb_points)
				{
					Logger::err("I/O")
						<< "too many points in .xyz file"
						<< std::endl;
					return false;
				}

				cout << in.nb_fields() << endl;
				format = in.nb_fields();
				points[cur++] = alloc_points(in);
				
			}
		}
		else	 //caso arquivo não forneça número de vértices
		{
			PCvertice v3d;
			cout << in.nb_fields() << endl;
			format = in.nb_fields();
			v3d = alloc_points(in);
			points.push_back(v3d);
			while (!in.eof() && in.get_line())
			{
				in.get_fields();
				//cout << in.nb_fields() << endl;
				PCvertice v3d;
				v3d = alloc_points(in);
				points.push_back(v3d);
			}
			nb_points = points.size();
			cout << nb_points << endl;
		}
	}
	catch (const std::exception& ex)
	{
		Logger::err("I/O") << ex.what() << std::endl;
		return false;
	}
	return true;
}


//preenche nuvem de pontos 
bool pointsHelper::fill_data(std::vector<PCvertice>& PCout, std::vector<PCvertice> PC2, GEO::index_t nb_points1,
	GEO::index_t nb_points2)
{
	try
	{


		cout << "merging point clouds" << endl;
		PCout.insert(PCout.end(), PC2.begin(), PC2.end());

		cout << "DONE" << endl;

	}
	catch (const std::exception& ex)
	{
		Logger::err("I/O") << ex.what() << std::endl;
		return false;
	}
	return true;
}

//elimina duplicatas dentro de um nivel de tolerancia dado
bool pointsHelper::delete_duplicates(std::vector<PCvertice>& PCout, GEO::vector<index_t> old2new, GEO::index_t& nb_points, int number_scans, int& proc_atual, point_store& all)
{
	try
	{
		int written = 0;
		std::vector<float> all_x;
		std::vector<float> all_y;
		std::vector<float> all_z;
		index_t j = 0;
		for (index_t i = 0; i < PCout.size(); ++i)
		{
			if (old2new[i] == i)
			{
				PCout[written++] = PCout[i];

				if (proc_atual == number_scans-1)
				{
					all_x.push_back(PCout[i].position.x);
					all_y.push_back(PCout[i].position.y);
					all_z.push_back(PCout[i].position.z);

				}
				
			}
		}
		
		if (proc_atual == number_scans - 1)
		{
			all.x = all_x;
			all.y = all_y;
			all.z = all_z;
		}
		proc_atual++;
		
		PCout.resize(written);
		nb_points = PCout.size();
		cout << "Written vertices: " << written << endl;

	}
	catch (const std::exception& ex)
	{
		Logger::err("I/O") << ex.what() << std::endl;
		return false;
	}
	return true;
}

//escreve arquivo de saída
bool pointsHelper::write_cloud(std::vector<PCvertice>& PCout, const std::string& filename, int format, char normalize,char ply, point_store all)
{
	try
	{

		FILE* file;
		file = fopen(filename.c_str(), "w");
		//fprintf(file, "%s", "//X Y Z Nx Ny Nz Rf Gf Bf Scalar_field\n");
		int written = 0;
		float max_x, min_x, max_y, min_y, max_z, min_z;
		if (ply == 'y')
		{
			fprintf(file, "%s", "ply\n");
			fprintf(file, "%s", "format ascii 1.0\n");
			//fprintf(file, "%s", "comment Created by Tecgraf/PUC-Rio\n");
			fprintf(file, "%s %d\n", "element vertex",PCout.size());
			fprintf(file, "%s", "property float x\n");
			fprintf(file, "%s", "property float y\n");
			fprintf(file, "%s", "property float z\n");
			fprintf(file, "%s", "property float nx\n");
			fprintf(file, "%s", "property float ny\n");
			fprintf(file, "%s", "property float nz\n");
			fprintf(file, "%s", "end_header\n");
		}
		if (normalize == 'y')
		{
			std::vector<float>::iterator result;
			result = std::max_element(std::begin(all.x), std::end(all.x));
			max_x = *result._Ptr;
			std::cout << "max x element: " << max_x << std::endl;

			result = std::min_element(std::begin(all.x), std::end(all.x));
			min_x = *result._Ptr;
			std::cout << "min x element: " << min_x << std::endl;

			result = std::max_element(std::begin(all.y), std::end(all.y));
			max_y = *result._Ptr;
			std::cout << "max y element: " << max_y << std::endl;

			result = std::min_element(std::begin(all.y), std::end(all.y));
			min_y = *result._Ptr;
			std::cout << "min y element: " << min_y << std::endl;

			result = std::max_element(std::begin(all.z), std::end(all.z));
			max_z = *result._Ptr;
			std::cout << "max z element: " << max_z << std::endl;

			result = std::min_element(std::begin(all.z), std::end(all.z));
			min_z = *result._Ptr;
			std::cout << "min z element: " << min_z << std::endl;

		}


		for (index_t i = 0; i < PCout.size(); ++i)
		{
				written++;

			
				vec3 currentPoint;
				
				if (normalize == 'y')
				{
					currentPoint.x = ((2.0*(PCout[i].position.x - min_x)) / (max_x - min_x)) - 1.0;
					currentPoint.y = ((2.0*(PCout[i].position.y - min_y)) / (max_y - min_y)) - 1.0;
					currentPoint.z = ((2.0*(PCout[i].position.z - min_z)) / (max_z - min_z)) - 1.0;
				}
				else
				{  
					currentPoint = PCout[i].position;
				}

				
				vec3 currentNormal = PCout[i].normals;
				RGB currentColor = PCout[i].colors;
				int currentID = PCout[i].id;

				if ((currentPoint.x != 0) && (currentPoint.y != 0) && (currentPoint.z != 0))
				{  

					/*cout << "x: " << currentPoint.x << endl;
					cout << "y: " << currentPoint.y << endl;
					cout << "z: " << currentPoint.z << endl;*/
					if (format == 6)  //xyz, normal  
					{
						
						fprintf(file, "%g %g %g %g %g %g\n",
							currentPoint.x, currentPoint.y, currentPoint.z,
							currentNormal.x, currentNormal.y, currentNormal.z);
					}
					else if (ply == 'y' && format != 6)
					{

						fprintf(file, "%g %g %g %g %g %g\n",
							currentPoint.x, currentPoint.y, currentPoint.z,
							currentNormal.x, currentNormal.y, currentNormal.z);
					}
					else if (format == 7)  //xyz, normal, id
					{
						fprintf(file, "%g %g %g %g %g %g %d\n",
							currentPoint.x, currentPoint.y, currentPoint.z,
							currentNormal.x, currentNormal.y, currentNormal.z, currentID);
					}
					else if (format == 9) //3 - xyz, normal, color
					{
						fprintf(file, "%g %g %g %g %g %g %g %g %g\n",
							currentPoint.x, currentPoint.y, currentPoint.z,
							currentNormal.x, currentNormal.y, currentNormal.z,
							currentColor.red, currentColor.green, currentColor.blue);
					}
					else if (format == 10) // xyz, normal, color, id
					{
						fprintf(file, "%g %g %g %g %g %g %g %g %g %d\n",
							currentPoint.x, currentPoint.y, currentPoint.z,
							currentNormal.x, currentNormal.y, currentNormal.z,
							currentColor.red, currentColor.green, currentColor.blue, currentID);
					}
					
			} 
			
		}
		fclose(file);
		cout << "Written vertices: " << written << endl;

	}
	catch (const std::exception& ex)
	{
		Logger::err("I/O") << ex.what() << std::endl;
		return false;
	}
	return true;
}


