#pragma once
#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <map>
#include <fstream>

#include <vd_dll.h>

namespace vd {


enum AttributeID
{

	AID_POSITION =0,
	AID_NORMAL=1,
	AID_TEXCOORD=2,
	AID_COLOR=3,
	AID_TANGENT=4,
	AID_BINORMAL=5,
	AID_COUNT
};


enum AttributeMask
{
	AM_POSITION =1,
	AM_NORMAL=2,
	AM_TEXCOORD=4,
	AM_COLOR=8,
	AM_TANGENT=16,
	AM_BINORMAL=32,

	AM_ALL =  (1<<AID_COUNT)-1,
};


/**
 * @brief The MeshVertex struct represents a Vertex as a set of AttributeIDs
 */
struct VD_DLL_PUBLIC MeshVertex
{
	uint32_t active_mask;
	union
	{
		struct
		{
			uint32_t pos_id;
			uint32_t nrm_id;
			uint32_t tex_id;
			uint32_t clr_id;
			uint32_t tan_id;
			uint32_t btn_id;
		};
		uint32_t att_id[AID_COUNT];
	};
	/**
	 * @brief MeshVertex initiates the MeshVertex with UINT32_MAX for every
	 * attribute id
	 */
	MeshVertex();
	/**
	 * @brief operator < A simple comparator. MeshVertices will be in a lot of
	 * std::maps so we will need this.
	 * @param o
	 * @return
	 */
	bool operator<(const MeshVertex& o) const;
};



/**
 * @brief The Triangle class represents a tripplet of MeshVertices
 */
class VD_DLL_PUBLIC Triangle
{
protected:
	MeshVertex vertices[3];
public:
	Triangle()	{}

	MeshVertex& operator[](const uint32_t i){return  vertices[i];}
	const MeshVertex& operator[](const uint32_t i) const{return  vertices[i];}

	MeshVertex* begin()	{return vertices;}
	MeshVertex* end()	{return vertices+3;}

	const MeshVertex* begin() const	{return vertices;}
	const MeshVertex* end() const	{return vertices+3;}
};

/**
 * @brief The Mesh class consist of a set of attribute lists, and one Triangle
 * list refering to the different attribute lists.
 */
class VD_DLL_PUBLIC Mesh
{
public:
	uint32_t active_mask;
	std::vector<glm::vec4> attribute_data[AID_COUNT];
	std::vector<Triangle> triangles;

	std::vector<glm::vec4>& get_pos_data(){return attribute_data[AID_POSITION];}
	std::vector<glm::vec4>& get_nrm_data(){return attribute_data[AID_NORMAL];}
	std::vector<glm::vec4>& get_tex_data(){return attribute_data[AID_TEXCOORD];}
	std::vector<glm::vec4>& get_clr_data(){return attribute_data[AID_COLOR];}
	std::vector<glm::vec4>& get_tan_data(){return attribute_data[AID_TANGENT];}
	std::vector<glm::vec4>& get_btn_data(){return attribute_data[AID_BINORMAL];}

	const std::vector<glm::vec4>& get_pos_data()const {return attribute_data[AID_POSITION];}
	const std::vector<glm::vec4>& get_nrm_data()const {return attribute_data[AID_NORMAL];}
	const std::vector<glm::vec4>& get_tex_data()const {return attribute_data[AID_TEXCOORD];}
	const std::vector<glm::vec4>& get_clr_data()const {return attribute_data[AID_COLOR];}
	const std::vector<glm::vec4>& get_tan_data()const {return attribute_data[AID_TANGENT];}
	const std::vector<glm::vec4>& get_btn_data()const {return attribute_data[AID_BINORMAL];}
};


/**
 * @brief Operations dealing with meshes.
 */
class VD_DLL_PUBLIC MeshOPS
{
public:
	static bool load_OBJ(Mesh& m, const std::string& path);
	static void recalculate_normals(Mesh& m);
	static void recalculate_tan_btn(Mesh& m);

	static Mesh remove_double_attributes(const Mesh& m);
};



template<uint32_t M = AM_ALL>
struct VD_DLL_PUBLIC vertex_masked_comperator
{
	size_t operator()(const MeshVertex &a,const MeshVertex &b) const
	{
		for(uint32_t i  = 0 ; i< AID_COUNT;i++)
		{
			if(!(M & (1<<i)))
				continue;
			if(a.att_id[i] == b.att_id[i])
				continue;
			else
				return a.att_id[i] < b.att_id[i];
		}
		return  false;
	}
};

struct VD_DLL_PUBLIC vertex_active_comperator
{
	size_t operator()(const MeshVertex &a,const MeshVertex &b) const
	{
		for(uint32_t i  = 0 ; i< AID_COUNT;i++)
		{
			if(!(a.active_mask & b.active_mask& (1<<i)))
				continue;
			if(a.att_id[i] == b.att_id[i])
				continue;
			else
				return a.att_id[i] < b.att_id[i];
		}
		return  false;
	}
};
}
