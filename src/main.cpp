#include <cstdio>
#include <cstdint>

#include <algorithm>
#include <assert.h>




//template <typename T>
//typename std::enable_if<std::is_unsigned<T>::value, bool>::type f(T n)
//{
//	return n <= 100;
//}

//template <typename T>
//typename std::enable_if<!std::is_unsigned<T>::value, bool>::type f(T n)
//{
//	return n >= 0 && n <= 100;
//}

#include "vd.h"
int main()
{

	auto vd = VertexDataTools::readFromFile("test.obj");
	if(!vd)
		printf("error opening file!\n");
	else
		VertexDataTools::writeOBJ(vd,stdout);
	return  0;
};
