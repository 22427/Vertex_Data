#ifndef BUILDING_DLL
#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <string>
#include "vd.h"
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
	Mesh m;	
	if(!MeshOPS::load(m, argv[1]))
	{
		print_help();
		return 2;			
	}
	if(!MeshOPS::write(m, argv[2]))
	{
		print_help();
		return 3;
	}


	return  0;
}

#endif
