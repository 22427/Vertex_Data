#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <string>
#include "vd.h"
#include <iostream>

#include <glm/glm.hpp>


int main(int argc, char ** argv)
{
	glm::vec3 v;
	std::string str = "1  2";
	std::stringstream sstr(str);

	sstr>>v.x>>v.y>>v.z;

	std::cout<<v.x<<","<<v.y<<","<<v.z<<std::endl;
	return  0;
}
