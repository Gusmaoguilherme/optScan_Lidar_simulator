#ifndef MERGE_H
#define MERGE_H

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
#include "CommandConfig.h"
#include "stringHelper.h"
#include "CommandlineParser.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include "PCvertice.h"
//Merging 

using CommandlineParser::CommandlineArguments;

namespace merge
{
	bool pointclouds(CommandlineArguments& arguments, std::vector<PCvertice>& cloudOutput, GEO::index_t& nb_pointsout, point_store& all_points); //merge
	
}
#endif

//GEO::Mesh& output