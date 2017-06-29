#include "../include/vd_mesh.h"
#include <glm/geometric.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <cstring>

#include <algorithm>
/******************************************************************************
 *   Some utilities                                                           *
 ******************************************************************************/
namespace std
{

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

}


namespace vd {

static inline void ltrim(std::string &s)
{
	s.erase(s.begin(),std::find_if(s.begin(), s.end(),
								   std::not1(std::ptr_fun<int, int>(std::isspace))));
}
static inline void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
						 std::not1(std::ptr_fun<int,int>(std::isspace))).base(),
			s.end());
}
static inline void trim(std::string &s){ltrim(s);rtrim(s);}


std::string read_word(std::stringstream& ss)
{
	std::string itm;
	do
	{
		std::getline(ss,itm,' ');
		trim(itm);
	}while(itm.empty() && ss);
	return  itm;
}



bool MeshOPS::load(Mesh& m, const std::string& path)
{
	std::string ext = path.substr(path.find_last_of('.')+1);
	for(char &c : ext) c= toupper(c);
	if(ext == "OBJ")
		return MeshOPS::load_OBJ(m,path);
	else if(ext == "OFF")
		return MeshOPS::load_OFF(m,path);
	return false;
}

/******************************************************************************
 *   OBJ-Loader                                                               *
 *****************************************************************************/

bool MeshOPS::load_OBJ(Mesh &m, const std::string &path)
{
	std::fstream f;
	f.open(path);
	if(!f.is_open())
		return  false;

	std::string line;
	glm::vec4 v;
	m.active_mask = AM_POSITION;
	while (std::getline(f,line))
	{
		line = line.substr(0,line.find_first_of('#')-1);
		trim(line);
		if(line.empty()) continue;

		std::stringstream line_str(line);
		v = glm::vec4(0,0,0,0);
		std::string lt = read_word(line_str);
		if(lt == "v")
		{
			line_str>>v.x>>v.y>>v.z>>v.w;
			m.get_pos_data().push_back(v);
		}
		else if(lt == "vn")
		{
			line_str>>v.x>>v.y>>v.z>>v.w;
			m.get_nrm_data().push_back(v);
			m.active_mask |= AM_NORMAL;
		}
		else if(lt == "vt")
		{
			line_str>>v.x>>v.y>>v.z>>v.w;
			m.get_tex_data().push_back(v);
			m.active_mask |= AM_TEXCOORD;
		}
		else if(lt == "f")
		{
			std::string v;
			Triangle t;
			for( uint32_t i = 0; i < 3; i++)
			{
				v = read_word(line_str);
				if(m.active_mask ==  AM_POSITION)  // only positions
				{
					sscanf(v.c_str(),"%d",
						   &(t[i].pos_id));
				}
				else if (m.active_mask == (AM_POSITION | AM_TEXCOORD))	// positions and texcoords
				{
					sscanf(v.c_str(),"%d/%d",
						   &(t[i].pos_id),
						   &(t[i].tex_id));
					t[i].tex_id--;
				}
				else if (m.active_mask == (AM_POSITION | AM_NORMAL))	 // positions and normals
				{
					sscanf(v.c_str(),"%d//%d",
						   &(t[i].pos_id),
						   &(t[i].nrm_id));
					t[i].nrm_id--;
				}
				else		// all three
				{
					sscanf(v.c_str(),"%d/%d/%d",
						   &(t[i].pos_id),
						   &(t[i].tex_id),
						   &(t[i].nrm_id));
					t[i].tex_id--;	t[i].nrm_id--;
				}
				t[i].pos_id--;
				t[i].active_mask = m.active_mask ;
			}

			m.triangles.push_back(t);
		}
	}
	return true;
}


/******************************************************************************
 *   OFF-Loader                                                               *
 *****************************************************************************/

bool MeshOPS::load_OFF(Mesh& m, const std::string& path)
{
	std::fstream f;
	f.open(path);
	if(!f.is_open())
		return  false;

	std::string line;
	glm::vec4 v(0,0,0,1);
	Triangle t;
	m.active_mask = AM_POSITION;

	std::getline(f,line);
	trim(line);
	if(line != "OFF")
		return false;
	std::getline(f,line);
	
	int n_vert = 0;
	int n_face = 0;
	int n_edge = 0;
	std::stringstream ls(line);
	ls>>n_vert>>n_face>>n_edge;

	m.get_pos_data().reserve(n_vert);
	m.triangles.reserve(n_face);

	for(int i = 0 ; i< n_vert;i++)
	{
		if(!std::getline(f,line))
		{
			m.get_pos_data().clear();
			return false;
		}
				
		std::stringstream ls(line);
		ls>>v.x>>v.y>>v.z;
		m.get_pos_data().push_back(v);	
	}
	for( int i = 0 ; i< n_face;i++)
	{
		if(!std::getline(f,line))
		{
			m.get_pos_data().clear();
			return false;
		}
				
		std::stringstream ls(line);
		int q=0;
		ls>>q;
		if(q!=3)
			return false;
		for(int j = 0; j<3;j++)			
			ls>>t[j].pos_id;
		m.triangles.push_back(t);
	}
	return true;

}
glm::vec4 get_weighted_normal(const Mesh&m, const uint32_t t_id)
{
	const auto t  = m.triangles[t_id];
	const glm::vec4 A = (m.get_pos_data()[t[1].pos_id])- m.get_pos_data()[t[0].pos_id];
	const glm::vec4 B = (m.get_pos_data()[t[2].pos_id])- m.get_pos_data()[t[0].pos_id];

	const glm::vec3 a(A);
	const glm::vec3 b(B);

	const glm::vec3 cr = glm::cross(a,b);
	float area = 0.5f* glm::length(cr);
	auto n = glm::normalize(cr);
	const float w = area/((glm::dot(a,a)*glm::dot(b,b)));
	return glm::vec4(n*w,0.0f);
}


bool MeshOPS::write(const Mesh&m, const std::string& path)
{
	std::string ext = path.substr(path.find_last_of('.')+1);
	for(char  &c : ext) c= toupper(c);
	if(ext == "OFF")
	{
		return MeshOPS::write_OFF(m,path);
	}
	return false;
}

bool MeshOPS::write_OFF(const Mesh& m, const std::string& path)
{
	std::ofstream f (path, std::ofstream::out);
	
	if(!f.is_open())
		return false;
	f<<"OFF\n"<<m.get_pos_data().size()<<" "<<m.triangles.size()<<" 0\n";

	for( const auto& v : m.get_pos_data())
	{
		f<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
	}
	for( const auto& t : m.triangles)
	{
		f<<"3";
		for(int i = 0 ; i< 3;i++)
			f<<" "<<t[i].pos_id;
		f<<"\n";
	}

	return true;
}
/******************************************************************************
 *   Normal calculation                                                       *
 ******************************************************************************/

void MeshOPS::recalculate_normals(Mesh &m)
{
	auto& norms = m.get_nrm_data();
	norms.clear();
	norms.resize(m.get_pos_data().size(), glm::vec4(0.0f));

	for(uint i = 0 ; i < m.triangles.size();i++)
	{
		auto& t = m.triangles[i];
		auto n = get_weighted_normal(m,i);
		for(uint j = 0 ;j < 3;j++)
		{
			t[j].nrm_id = t[j].pos_id;
			norms[t[j].pos_id] += n;
		}
	}

	for(auto& a : norms)
	{
		a = glm::normalize(a);
	}
	m.active_mask |= AM_NORMAL;
}

/******************************************************************************
 *   Tangent and Bitangent calculation                                        *
 ******************************************************************************/

void MeshOPS::recalculate_tan_btn(Mesh &m)
{
	m.get_tan_data().clear();
	m.get_btn_data().clear();

	std::map<MeshVertex,uint32_t,vertex_masked_comperator<AM_POSITION | AM_NORMAL | AM_TEXCOORD>> ids;
	auto& tans = m.get_tan_data();
	auto& btns = m.get_btn_data();

	for (auto& t : m.triangles)
	{
		auto& vtx = t[0];
		const glm::vec4& v0 = m.get_pos_data()[vtx.pos_id];
		const glm::vec4& uv0 = m.get_tex_data()[vtx.pos_id];

		vtx = t[1];
		const glm::vec4& v1 = m.get_pos_data()[vtx.pos_id];
		const glm::vec4& uv1 = m.get_tex_data()[vtx.pos_id];

		vtx = t[2];
		const glm::vec4& v2 = m.get_pos_data()[vtx.pos_id];
		const glm::vec4& uv2 = m.get_tex_data()[vtx.pos_id];

		glm::vec4 dPos1 = v1 - v0;
		glm::vec4 dPos2 = v2 - v0;
		glm::vec4 dUV1 =uv1 -uv0;
		glm::vec4 dUV2 =uv2 -uv0;

		float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

		glm::vec4 tan = (dPos1 * dUV2.y - dPos2 * dUV1.y)*r;
		glm::vec4 btn = (dPos2 * dUV1.x - dPos1 * dUV2.x)*r;
		tan.w = btn.w = 0.0f;

		for(uint j = 0 ;j < 3;j++)
		{
			auto it = ids.find(t[j]);
			if(it == ids.end())
			{
				const uint32_t new_id =  static_cast<uint32_t>(tans.size());
				ids[t[j]] = new_id;
				t[j].tan_id = t[j].btn_id =  new_id;
				tans.push_back(tan);
				btns.push_back(btn);
			}
			else
			{
				tans[(*it).second] += tan;
				btns[(*it).second] += btn;
			}
		}
	}

	m.active_mask |= AM_TANGENT | AM_BINORMAL;
}


uint32_t insert_attrib_value(const glm::vec4& val,
							 std::map<glm::vec4,uint32_t,std::less<glm::vec4>>& ad,
							 std::vector<glm::vec4> &attribute_data)
{

	auto it = ad.find(val);
	if(it != ad.end())
		return (*it).second;
	const uint32_t id = static_cast<uint32_t>(attribute_data.size());
	ad[val] = id;
	attribute_data.push_back(val);
	return id;
}


Mesh MeshOPS::remove_double_attributes(const Mesh &m)
{
	Mesh r;
	std::map<glm::vec4,uint32_t,std::less<glm::vec4> > attribute_id[AID_COUNT];

	for(uint32_t i = 0 ; i < AID_COUNT; i++)
	{
		if(!(m.active_mask & (1<<i)))
			continue;
		const auto& iad = m.attribute_data[i];
		auto oad = r.attribute_data[i];
		auto& aid = attribute_id[i];
		for(auto t:m.triangles)
		{
			for(auto& v  : t)
			{
				v.att_id[i] = insert_attrib_value(iad[v.att_id[i]],aid,oad);
			}
		}
	}
	return r;
}

MeshVertex::MeshVertex()
{
	for(uint32_t i = 0 ; i < AID_COUNT;i++)
		att_id[i] = UINT32_MAX;
	active_mask = (1<<AID_COUNT)-1;
}

bool MeshVertex::operator<(const MeshVertex &o) const
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

}
