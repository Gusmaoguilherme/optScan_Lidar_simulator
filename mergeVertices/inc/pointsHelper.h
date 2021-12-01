#ifndef POINTSHELPER_H
#define POINTSHELPER_H

#include <geogram/basic/common.h>
#include <geogram/basic/command_line.h>
#include <geogram/basic/command_line_args.h>
#include <geogram/mesh/mesh.h>
#include <geogram/mesh/mesh_compare.h>
#include <geogram/mesh/mesh_io.h>
#include <geogram/basic/logger.h>
#include <geogram/mesh/mesh_repair.h>
#include <geogram/mesh/mesh_fill_holes.h>
#include <geogram/mesh/mesh_degree3_vertices.h>
#include <geogram/mesh/mesh_intersection.h>
#include <geogram/mesh/mesh_geometry.h>
#include <geogram/mesh/mesh_preprocessing.h>
#include <geogram/mesh/mesh_remesh.h>
#include <geogram/mesh/mesh_decimate.h>
#include <geogram/mesh/mesh_tetrahedralize.h>
#include <geogram/mesh/mesh_topology.h>
#include <geogram/mesh/mesh_AABB.h>
#include <geogram/basic/common.h>
#include <geogram/mesh/mesh.h>
#include <geogram/basic/geometry.h>
#include <geogram/basic/geometry_nd.h>
#include <geogram/points/nn_search.h>
#include <geogram/voronoi/CVT.h>
#include <geogram/mesh/mesh_smoothing.h>
#include <geogram/mesh/mesh_subdivision.h>
#include <geogram/points/colocate.h>
#include <geogram/basic/line_stream.h>
#include <vector>

#include <geogram/delaunay/LFS.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <algorithm>
#include "PCvertice.h"
//Ferramentas de manipulação de pontos

struct point_store
{
	std::vector<float> x;
	std::vector<float> y;
	std::vector<float> z;
};

namespace pointsHelper
{
	static PCvertice alloc_points(const LineInput& in);
	bool load_points(std::vector<PCvertice>& points, const std::string& filename, GEO::index_t& nb_points, int& format);   //carrega os dados do arquivo xyz/txt
	bool fill_data(std::vector<PCvertice>& PCout, std::vector<PCvertice> PC2,
		GEO::index_t nb_points1, GEO::index_t nb_points2); //Preenche nuvem de saída
	bool delete_duplicates(std::vector<PCvertice>& PCout, GEO::vector<index_t> old2new, GEO::index_t& nb_points,
		int number_scans, int& proc_atual, point_store& all);//Cria nuvem com pontos duplicados eliminados
	bool write_cloud(std::vector<PCvertice>& PCout, const std::string& filename, int format, char normalize,char ply, point_store all); //Escreve o arquivo xyz da nuvem 
	
}
#endif

//GEO::Mesh& output