#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <assert.h>

#include "vd.h"

int main(int argc, char ** argv)
{
	if(argc < 3)
		return 0;
	auto vd = VDOps::read_from_file(argv[1]);
	if(!vd)
		printf("error opening file!\n");
	else
		VDOps::write_to_file(vd,argv[2]);
	return  0;
};
