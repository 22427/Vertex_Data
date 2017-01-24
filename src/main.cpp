#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <assert.h>

#include "vd.h"


int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		auto fac = vd::Factory();
		auto vd = fac.create_box();
		vd::Operations::write_to_file(vd,"box.obj");
		delete vd;

		vd = fac.create_cone();
		vd::Operations::write_to_file(vd,"cone.obj");
		delete vd;

		vd = fac.create_cylinder();
		vd::Operations::write_to_file(vd,"cylinder.obj");
		delete vd;

		vd = fac.create_disk();
		vd::Operations::write_to_file(vd,"disk.obj");
		delete vd;

		vd = fac.create_plane();
		vd::Operations::write_to_file(vd,"plane.obj");
		delete vd;

		vd = fac.create_uv_sphere();
		vd::Operations::write_to_file(vd,"uv_sphere.obj");
		delete vd;
		return  0;

	}
	auto vd = vd::Operations::read_from_file(argv[1]);
	if(!vd)
		printf("error opening file!\n");
	else
		vd::Operations::write_to_file(vd,argv[2]);
	return  0;
};
