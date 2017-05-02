#pragma once
#include "vd_mesh.h"
#include "vd_dll.h"
#include <cstdint>

namespace vd {
struct VD_DLL_PUBLIC MiniVertex
{
	int16_t pos[3];
	int16_t nrm[3];
	uint16_t tex[2];
	void set(const glm::vec3& p,const glm::vec3& n,const glm::vec3& t);
};


class VD_DLL_PUBLIC MiniVD
{
public:
	uint32_t data_size;
	uint32_t index_size;
	glm::vec3 pos_scale;
	glm::vec3 pos_offset;

	MiniVertex* data;
	uint16_t* index;

	MiniVD();
	void read_mesh(const Mesh& m);
};


class VD_DLL_PUBLIC MiniVertexDataOPS
{
public:

	static bool write(const MiniVD &vd, std::ofstream& f);
	static bool write(const MiniVD &vd, const std::string& path);

	static bool read(MiniVD &vd, std::ifstream& f);
	static bool read(MiniVD &vd, const std::string& path);
};
}
