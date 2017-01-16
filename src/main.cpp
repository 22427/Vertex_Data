#include <cstdio>
#include <cstdint>

#include <algorithm>
#include <assert.h>



#include "vd.h"
int main(int argc, char ** argv)
{
	if(argc < 3)
		return 0;
	auto vd = VertexDataTools::readFromFile(argv[1]);
	if(!vd)
		printf("error opening file!\n");
	else
		VertexDataTools::writeToFile(vd,argv[2]);
	return  0;
};
