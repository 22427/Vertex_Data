#ifndef PT_BUILDING_DLL
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

int main(int argc, char ** argv)
{
	Mesh m;
	MeshOPS::load_OBJ(m,"test.obj");
	printf("cfg: %d\n", sizeof(VertexConfiguration));
	printf("attrib: %d\n", sizeof(Attribute));

	return  0;
}

#endif
