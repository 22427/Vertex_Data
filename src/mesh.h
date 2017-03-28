#pragma once
#include <vector>
#include <map>
#include <cstdint>
#include <cstring>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <limits>
#include <unordered_map>
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
}


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
			uint32_t nrn_id;
			uint32_t tex_id;
			uint32_t clr_id;
			uint32_t tan_id;
			uint32_t bnm_id;
		};
		uint32_t att_id[AID_COUNT];
	};
	uint32_t active_mask;

	bool operator<(const Vertex& o) const
	{
		for(uint32_t i  = 0 ; i< AID_COUNT;i++)
		{
			if(active_mask& 1<<i || o.active_mask& 1<<i)
				continue;
			if(att_id[i] == o.att_id[i])
				continue;
			else
				return  att_id[i] < o.att_id[i];
		}
		return  false;
	}
};

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
};

class Mesh
{
public:

	std::vector<glm::vec4> attribute_data[AID_COUNT];
	std::map<glm::vec4,uint32_t,std::less<glm::vec4> > attribute_id[AID_COUNT];

	std::vector<Triangle> triangles;

	std::vector<glm::vec4>& get_pos_data(){return attribute_data[AID_POSITION];}
	std::vector<glm::vec4>& get_nrm_data(){return attribute_data[AID_NORMAL];}
	std::vector<glm::vec4>& get_tex_data(){return attribute_data[AID_TEXCOORD];}
	std::vector<glm::vec4>& get_clr_data(){return attribute_data[AID_COLOR];}
	std::vector<glm::vec4>& get_tan_data(){return attribute_data[AID_TANGENT];}
	std::vector<glm::vec4>& get_bnm_data(){return attribute_data[AID_BINORMAL];}


	const std::vector<glm::vec4>& get_pos_data()const {return attribute_data[AID_POSITION];}
	const std::vector<glm::vec4>& get_nrm_data()const {return attribute_data[AID_NORMAL];}
	const std::vector<glm::vec4>& get_tex_data()const {return attribute_data[AID_TEXCOORD];}
	const std::vector<glm::vec4>& get_clr_data()const {return attribute_data[AID_COLOR];}
	const std::vector<glm::vec4>& get_tan_data()const {return attribute_data[AID_TANGENT];}
	const std::vector<glm::vec4>& get_bnm_data()const {return attribute_data[AID_BINORMAL];}

	uint32_t insert_attrib_value(AttributeID aid,const glm::vec4& val)
	{
		auto& ad= attribute_id[aid];
		auto it = ad.find(val);
		if(it != ad.end())
			return (*it).second;
		const uint32_t id = static_cast<uint32_t>(attribute_data[aid].size());
		ad[val] = id;
		attribute_data[aid].push_back(val);
		return id;
	}

};



class MeshOPS
{
public:
	static bool load_OBJ(Mesh& m, const std::string& path);
	static void recalculate_normals(Mesh& m);
	static void recalculate_tan_bnm(Mesh& m);
};
