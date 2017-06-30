#pragma once
#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <map>
#include <fstream>

#include "vd_dll.h"
#include "vd_errcode.h"
namespace vd {


typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef uint8_t ubyte;
typedef int8_t byte;

enum AttributeID
{

	AID_POSITION =0,
	AID_TEXCOORD=1,
	AID_NORMAL=2,
	AID_COLOR=3,
	AID_TANGENT=4,
	AID_BITANGENT=5,
	AID_COUNT
};


enum AttributeMask
{
	AM_POSITION =1<<AID_POSITION,
	AM_TEXCOORD=1<<AID_TEXCOORD,
	AM_NORMAL=1<<AID_NORMAL,
	AM_COLOR=1<<AID_COLOR,
	AM_TANGENT=1<<AID_TANGENT,
	AM_BITANGENT=1<<AID_BITANGENT,
	AM_ALL =  (1<<AID_COUNT)-1,
};


/**
 * @brief The MeshVertex struct represents a Vertex as a set of AttributeIDs
 */
struct DLL_PUBLIC MeshVertex
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
class DLL_PUBLIC Triangle
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
class DLL_PUBLIC Mesh
{
public:
	uint32_t active_mask;
	std::vector<vec4> attribute_data[AID_COUNT];
	std::vector<Triangle> triangles;

	std::vector<vec4>& get_pos_data(){return attribute_data[AID_POSITION];}
	std::vector<vec4>& get_nrm_data(){return attribute_data[AID_NORMAL];}
	std::vector<vec4>& get_tex_data(){return attribute_data[AID_TEXCOORD];}
	std::vector<vec4>& get_clr_data(){return attribute_data[AID_COLOR];}
	std::vector<vec4>& get_tan_data(){return attribute_data[AID_TANGENT];}
	std::vector<vec4>& get_btn_data(){return attribute_data[AID_BITANGENT];}

	const std::vector<vec4>& get_pos_data()const {
		return attribute_data[AID_POSITION];}
	const std::vector<vec4>& get_nrm_data()const {
		return attribute_data[AID_NORMAL];}
	const std::vector<vec4>& get_tex_data()const {
		return attribute_data[AID_TEXCOORD];}
	const std::vector<vec4>& get_clr_data()const {
		return attribute_data[AID_COLOR];}
	const std::vector<vec4>& get_tan_data()const {
		return attribute_data[AID_TANGENT];}
	const std::vector<vec4>& get_btn_data()const {
		return attribute_data[AID_BITANGENT];}
};


/**
 * @brief Operations dealing with meshes.
 */
class DLL_PUBLIC MeshOPS
{
	static std::string m_errmsg;
	static ErrorCode m_errcde;
public:
	/**
	 * @brief read will load a mesh from a file. The file extension will
	 * determine the loader used.
	 *	OFF			 -> read_OFF
	 *	OBJ,OBJP,OBJ+ -> read_OBJP
	 * @param m	Mesh to load the data into.
	 * @param path Where to find the file
	 * @return
	 */
	static bool read(Mesh& m, const std::string& path);
	static bool read_OBJP(Mesh& m, std::ifstream& f);
	static bool read_OFF(Mesh& m, std::ifstream& f);
	static bool read_PLY(Mesh& m, std::ifstream& f);

	/**
	 * @brief read will save a mesh to a file. The file extension will
	 * determine the writer used.
	 *	OFF			 -> write_OFF
	 *	OBJ,OBJP,OBJ+ -> write_OBJP
	 * @param m	Mesh to read the data from.
	 * @param path Where to write the data to.
	 * @return
	 */
	static bool write(const Mesh& m, const std::string& path);
	static bool write_OBJP(const Mesh& m,  std::ofstream& f);
	static bool write_OFF(const Mesh& m, std::ofstream& f);

	/**
	 * @brief recalculate_normals Will calculate smooth normals.
	 * @param m
	 */
	static void recalculate_normals(Mesh& m);
	/**
	 * @brief recalculate_normals Will calculate tangents and bitangents
	 * for the mesh. This needs positions, normals and texture coordinates to
	 * work.
	 * @param m
	 */
	static bool recalculate_tan_btn(Mesh& m);

	/**
	 * @brief remove_double_attributes Will remove any doubled attributes.
	 * @param m
	 * @return
	 */
	static Mesh remove_double_attributes(const Mesh& m);

	/**
	 * @brief read_error_code accessing the last error code. Will be resetted
	 * after this access.
	 * @return
	 */
	static ErrorCode read_error_code()
	{ErrorCode r = m_errcde;m_errcde=NO_ERROR;return r;}

	/**
	 * @brief error_msg acessing the last error message.
	 * @return
	 */
	static const std::string& error_msg(){return m_errmsg;}
};



template<uint32_t M = AM_ALL>
struct DLL_PUBLIC vertex_masked_comperator
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

struct DLL_PUBLIC vertex_active_comperator
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
