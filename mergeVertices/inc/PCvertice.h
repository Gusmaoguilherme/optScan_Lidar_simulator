#ifndef PCVERTICE_H
#define PCVERTICE_H

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

using namespace std;

using namespace GEO;

//classe com caracteristicas do ponto

struct RGB
{
	float red, green, blue;
};

class PCvertice
{
public:
	
	GEO::vec3 position;
	GEO::vec3 normals;
	RGB colors;
	int id;
	
	void set3Dpoint(float x, float y, float z);
	void setNormals(float nx, float ny, float nz);
	void setColors(float r, float g, float b);
	void setID(int tag);
};

#endif