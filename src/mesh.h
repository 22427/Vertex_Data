#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <cstdint>
#include <cstring>

#include <vector>
#include <map>
#include <limits>
#include <fstream>
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



struct Vertex
{
	Vertex()
	{
		for(uint32_t i = 0 ; i < AID_COUNT;i++)
			att_id[i] = UINT32_MAX;
		active_mask = (1<<AID_COUNT)-1;
	}
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
	uint32_t active_mask;

	bool operator<(const Vertex& o) const
	{
		for(uint32_t i  = 0 ; i< AID_COUNT;i++)
		{
			if(att_id[i] == o.att_id[i])
				continue;
			else
				return  att_id[i] < o.att_id[i];
		}
		return  false;
	}
};


template<uint32_t M = AM_ALL>
struct vertex_masked_comperator
{
	size_t operator()(const Vertex &a,const Vertex &b) const
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

struct vertex_active_comperator
{
	size_t operator()(const Vertex &a,const Vertex &b) const
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



namespace std {
template<>
struct less<glm::vec4>
{
	inline bool eq(const float& a,const float& b) const
	{
		return std::nextafter(a, std::numeric_limits<float>::lowest()) <= b
			&& std::nextafter(a, std::numeric_limits<float>::max()) >= b;
	}

	size_t operator()(const glm::vec4 &a,const glm::vec4 &b) const
	{
		for(int i = 0 ; i < 4; i++)
		{
			if(eq(a[i],b[i])) continue;

			return  a[i]<b[i];
		}
		return false;
	}
};


template<>
struct hash<glm::vec4>
{
	size_t operator()(const glm::vec4 &s) const
	{
		const size_t h1 = std::hash<float>()(s.x);
		const size_t h2 = std::hash<float>()(s.y);
		const size_t h3 = std::hash<float>()(s.z);
		const size_t h4 = std::hash<float>()(s.w);

		return h1 ^ ( h2 << 1 )^ ( h3 << 4 )^ ( h4 << 8 );
	}
};

template<>
struct less<Vertex>
{
	size_t operator()(const Vertex &a,const Vertex &b) const
	{
		for(uint32_t i  = 0 ; i< AID_COUNT;i++)
		{
			if(a.active_mask& b.active_mask & (1<<i))
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



class Triangle
{
protected:
	Vertex vertices[3];
public:
	Vertex& operator[](const uint32_t i){return  vertices[i];}
	const Vertex& operator[](const uint32_t i) const{return  vertices[i];}
	Triangle()
	{

	}

	Vertex* begin()
	{
		return vertices;
	}

	Vertex* end()
	{
		return vertices+3;
	}


	const Vertex* begin() const
	{
		return vertices;
	}

	const Vertex* end() const
	{
		return vertices+3;
	}
};

class Mesh
{
public:

	std::vector<glm::vec4> attribute_data[AID_COUNT];


	std::vector<Triangle> triangles;

	uint32_t active_mask;

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



class MeshOPS
{
public:
	static bool load_OBJ(Mesh& m, const std::string& path);
	static void recalculate_normals(Mesh& m);
	static void recalculate_tan_btn(Mesh& m);

	static Mesh remove_double_attributes(const Mesh& m);
};
