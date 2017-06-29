#ifndef BUILDING_DLL
#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <string>
#include "include/vd.h"
#include <iostream>

#include <glm/glm.hpp>

#include <vd_mesh.h>
using namespace  vd;

void print_help()
{
	printf("vd input output\n");
}
int main(int argc, char ** argv)
{
	if(argc<3)
	{
		print_help();
		return 1;
	}
	std::string pin = argv[1];
	std::string ext_in = pin.substr(pin.find_last_of('.')+1);
	for(char &c : ext_in) c= toupper(c);
	std::string pout = argv[2];
	std::string ext_out = pout.substr(pout.find_last_of('.')+1);
	for(char &c : ext_out) c= toupper(c);
	Mesh m;

	if(ext_in != "VD")
	{
		if(!MeshOPS::read(m, argv[1]))
		{
			print_help();
			return 2;
		}
	}
	else
	{
		VertexData vd;
		if(!VertexDataOPS::read(vd,pin))
			return 2;
		VertexDataOPS::to_mesh(vd,m);
	}
	// output
	if(ext_out != "VD")
	{
		if(!MeshOPS::write(m, argv[2]))
		{
			print_help();
			return 3;
		}
	}

	else
	{
		VertexConfiguration cfg;
		cfg.set_attribute(AID_POSITION,3,FLOAT,false);
		cfg.set_attribute(AID_NORMAL,3,FLOAT,false);
		cfg.set_attribute(AID_TEXCOORD,2,FLOAT,false);
		cfg.set_attribute(AID_COLOR,4,UNSIGNED_BYTE,true);
		VertexData vd(PRIM_TRIANGLES,cfg);
		printf("Note: outputting 'standard' VD with the following attributes:\n\n");
		printf("\tAttribute | #elem |  Type  | Normalized\n");
		printf("\t----------+-------+--------+-----------\n");
		printf("\tPOSITION  |   3   | FLOAT  | false\n");
		printf("\tNORMAL    |   3   | FLOAT  | false\n");
		printf("\tTEXCOORD  |   2   | FLOAT  | false\n");
		printf("\tCOLOR     |   4   | U_BYTE | true\n");
		VertexDataOPS::from_mesh(vd,m);
		if(!VertexDataOPS::write(vd,pout))
			return 3;
	}


	return  0;
}

#endif
