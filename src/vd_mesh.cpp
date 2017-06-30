#include "../include/vd_mesh.h"
#include <glm/geometric.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <cstring>

#include <algorithm>
using namespace glm;
///*****************************************************************************
///*   UTILITIES                                                               *
///*****************************************************************************
namespace std
{

template<>
struct less<vec4>
{
	inline bool eq(const float& a,const float& b) const
	{
		return std::nextafter(a, std::numeric_limits<float>::lowest()) <= b
				&& std::nextafter(a, std::numeric_limits<float>::max()) >= b;
	}

	size_t operator()(const vec4 &a,const vec4 &b) const
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
								   std::not1(
									   std::ptr_fun<int,int>(std::isspace))));
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


///*****************************************************************************
///*   INPUT                                                                   *
///*****************************************************************************


bool MeshOPS::read(Mesh& m, const std::string& path)
{
	std::string ext = path.substr(path.find_last_of('.')+1);
	for(char &c : ext) c= toupper(c);

	std::ifstream f(path);
	if(!f.is_open())
	{
		m_errcde = CANNOT_OPEN_FILE;
		m_errmsg = "Cannot open file '"+path+"'";
		return false;
	}
	else if(ext == "OFF")
		return MeshOPS::read_OFF(m,f);
	else if(ext=="OBJ" || ext == "OBJP" || ext == "OBJ+")
		return MeshOPS::read_OBJP(m,f);
	return false;
}


//   OFF-Loader
//------------------------------------------------------------------------------


bool MeshOPS::read_OFF(Mesh &m, std::ifstream &f)
{
	std::string line;
	vec4 v(0,0,0,1);
	Triangle t;
	m.active_mask = AM_POSITION;

	std::getline(f,line);
	trim(line);
	if(line != "OFF")
	{
		m_errcde = PARSING_FILE;
		m_errmsg = "OFF files should start with 'OFF'";
		return false;
	}
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
			m_errcde = PARSING_FILE;
			m_errmsg = "Not enough vertices to read.";
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
			m.triangles.clear();
			m_errcde = PARSING_FILE;
			m_errmsg = "Not enough faces to read.";
			return false;
		}
		std::stringstream ls(line);
		int q=0;
		ls>>q;
		if(q!=3)
		{
			m_errcde = NOT_SUPPORTED;
			m_errmsg = "Only triangle meshes are supported for now!";
			return false;
		}
		for(int j = 0; j<3;j++)
			ls>>t[j].pos_id;
		m.triangles.push_back(t);
	}
	return true;
}

//   OBJP-Loader
//------------------------------------------------------------------------------


bool MeshOPS::read_OBJP(Mesh &m, std::ifstream &f)
{

	std::string line;
	vec4 v;
	m.active_mask = AM_POSITION;
	int max_attr = 0;
	while (std::getline(f,line))
	{
		auto first_hash = line.find_first_of('#');
		if(line[first_hash+1]=='+')
			line[first_hash]='+';
		line = line.substr(0,line.find_first_of('#')-1);
		trim(line);
		if(line.empty()) continue;

		std::stringstream line_str(line);
		v = vec4(0,0,0,0);
		std::string lt = read_word(line_str);

		int attr_id = -1;
		if(lt == "v")		attr_id = 0;
		else if(lt == "vt")	attr_id = 1;
		else if(lt == "vn")	attr_id = 2;
		else if(lt == "++vc")attr_id = 3;
		else if(lt == "++vtn")attr_id = 4;
		else if(lt == "++vbtn")attr_id = 5;
		if(attr_id >=0)
		{
			if(attr_id != AID_POSITION || AID_COLOR)
				v.w =1.0;
			m.active_mask |= (1<<attr_id);
			line_str>>v.x>>v.y;
			if(attr_id != AID_TEXCOORD)
				line_str>>v.z;
			if(attr_id == AID_COLOR)
				line_str>>v.w;
			m.attribute_data[attr_id].push_back(v);
			max_attr = std::max(max_attr,attr_id);
		}
		else if(lt == "f")
		{
			Triangle t;
			for( uint32_t v = 0; v < 3; v++)
			{
				for(int a =0 ; a<=std::min(max_attr,3);a++)
				{
					if(m.active_mask & (1<<a))
					{
						line_str>>t[v].att_id[a];
						t[v].att_id[a]-=1;
					}
					char c = '@';
					if(a!= max_attr && a!= 2)
						while(c != '/') line_str>>c;
				}

				if(max_attr>3)
					read_word(line_str);

				for(int a =3 ; a<=max_attr;a++)
				{
					if(m.active_mask & (1<<a))
					{
						line_str>>t[v].att_id[a];
						t[v].att_id[a]-=1;
					}
					char c = '@';
					if(a!= max_attr)
						while(c != '/') line_str>>c;
				}


				t[v].active_mask = m.active_mask ;
			}

			m.triangles.push_back(t);
		}
	}
	return true;

}


///*****************************************************************************
///   OUTPUT                                                                   *
///*****************************************************************************


bool MeshOPS::write(const Mesh&m, const std::string& path)
{
	std::string ext = path.substr(path.find_last_of('.')+1);
	for(char  &c : ext) c= toupper(c);

	std::ofstream f(path);
	if(!f.is_open())
	{
		m_errcde = CANNOT_OPEN_FILE;
		m_errmsg = "Cannot open file to write '"+path+"'";
		return false;
	}
	if(ext == "OFF")
	{
		return MeshOPS::write_OFF(m,f);
	}

	if(ext == "OBJ+" || ext == "OBJP" || ext=="OBJ")
	{
		return MeshOPS::write_OBJP(m,f);
	}
	return false;
}







//   OFF-Writer
//------------------------------------------------------------------------------
bool MeshOPS::write_OFF(const Mesh &m, std::ofstream &f)
{
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




//   OBJ+-Writer
//------------------------------------------------------------------------------
void write_OBJp(const Mesh&m, uint32_t active_mask, std::ofstream& f);
bool MeshOPS::write_OBJP(const Mesh &m, std::ofstream &f)
{
	f<<"# OBJ+ extends OBJ, there are now six attributes\n";
	f<<"# in order make it OBJ compatible '#+' does not indicate a comment "
	   "anymore\n";
	f<<"# v  - vertex position\n";
	f<<"# vn - vertex normal\n";
	f<<"# vt - vertex textrue coord\n";
	f<<"# #+vc - vertex color\n";
	f<<"# #+vtn - vertex tangent\n";
	f<<"# #+vbtn - vertex bitangent\n";
	f<<"# a face consists of up to 6 indices, the usual 3 obj indices,\n";
	f<<"# followed by the new obj+ indices, introduced by #+ \n";
	f<<"# Example:\n";
	f<<"# f 1/2/3 2/3/3 3/4/3 #+ 1 2 3 # with normals,texcoords & colors\n";
	f<<"# f 1 2 3 #+ 3 7 2             # with colors\n";
	f<<"# f 1 2 3 #+ 3/2 7/4 2/12      # with colors & tangents\n\n";
	f<<"# Note any .obj is an .obj+, and the other way arround if\n";
	f<<"# and _ONLY_ if there are no commends '#+ ...' in the .obj-file\n";

	write_OBJp(m,m.active_mask,f);
	return true;
}

void write_OBJp(const Mesh&m, uint32_t active_mask, std::ofstream& f)
{
	std::string a_names[AID_COUNT] = {
		std::string("v"),
		std::string("vt"),
		std::string("vn"),
		std::string("#+vc"),
		std::string("#+vtn"),
		std::string("#+vbtn")};

	int max_active =0 ;
	for(int a =0 ; a< AID_COUNT;a++)
		if(active_mask & (1<<a))
		{
			for( const auto& v : m.attribute_data[a])
			{
				f<<a_names[a]<<" "<<v.x<<" "<<v.y;
				if(a != AID_TEXCOORD)
					f<<" "<<v.z;
				if(a == AID_COLOR)
					f<<" "<<v.w;
				f<<"\n";
			}
			max_active = a;
		}
	for(const auto& t : m.triangles)
	{
		f<<"f";
		for(int v = 0 ; v< 3; v++)
		{
			f<<" ";
			for(int a =0 ; a<= max_active && a < 3;a++)
			{
				if(active_mask & (1<<a))
				{
					f<<t[v].att_id[a]+1;
				}
				if(a<max_active && a != 2)
					f<<"/";
			}
		}
		if(max_active > 2)
		f<<" #+ ";
		for(int v = 0 ; v< 3; v++)
		{
			f<<" ";
			for(int a =3 ; a<= max_active;a++)
			{
				if(active_mask & (1<<a))
				{
					f<<t[v].att_id[a]+1;
				}
				if(a<max_active )
					f<<"/";
			}
		}

		f<<"\n";
	}
}


///*****************************************************************************
///*   PROCESSING                                                              *
///*****************************************************************************


//   Normal calculation
//------------------------------------------------------------------------------
vec4 get_weighted_normal(const Mesh&m, const uint32_t t_id)
{
	const auto t  = m.triangles[t_id];
	const auto& pd = m.get_pos_data();
	const vec4 A = (pd[t[1].pos_id])-pd[t[0].pos_id];
	const vec4 B = (pd[t[2].pos_id])-pd[t[0].pos_id];

	const vec3 a(A);
	const vec3 b(B);

	const vec3 cr = cross(a,b);
	float area = 0.5f* length(cr);
	auto n = normalize(cr);
	const float w = area/((dot(a,a)*dot(b,b)));
	return vec4(n*w,0.0f);
}

void MeshOPS::recalculate_normals(Mesh &m)
{
	auto& norms = m.get_nrm_data();
	norms.clear();
	norms.resize(m.get_pos_data().size(), vec4(0.0f));

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
		a = normalize(a);
	}
	m.active_mask |= AM_NORMAL;
}


//  Tangent and Bitangent calculation
//------------------------------------------------------------------------------

bool MeshOPS::recalculate_tan_btn(Mesh &m)
{
	uint32_t req = AM_TEXCOORD|AM_NORMAL|AM_POSITION;
	if((m.active_mask&req) != req)
	{
		m_errcde = NEED_MORE_DATA;
		m_errmsg = "Calculating tangents and bitangents needs texcoords,"
				   "normals and positions.";
		return false;
	}
	m.get_tan_data().clear();
	m.get_btn_data().clear();

	std::map<MeshVertex,uint32_t,
			vertex_masked_comperator<AM_POSITION|AM_NORMAL|AM_TEXCOORD>> ids;
	auto& tans = m.get_tan_data();
	auto& btns = m.get_btn_data();

	for (auto& t : m.triangles)
	{
		auto& vtx = t[0];
		const vec4& v0 = m.get_pos_data()[vtx.pos_id];
		const vec4& uv0 = m.get_tex_data()[vtx.pos_id];

		vtx = t[1];
		const vec4& v1 = m.get_pos_data()[vtx.pos_id];
		const vec4& uv1 = m.get_tex_data()[vtx.pos_id];

		vtx = t[2];
		const vec4& v2 = m.get_pos_data()[vtx.pos_id];
		const vec4& uv2 = m.get_tex_data()[vtx.pos_id];

		vec4 dPos1 = v1 - v0;
		vec4 dPos2 = v2 - v0;
		vec4 dUV1 =uv1 -uv0;
		vec4 dUV2 =uv2 -uv0;

		float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

		vec4 tan = (dPos1 * dUV2.y - dPos2 * dUV1.y)*r;
		vec4 btn = (dPos2 * dUV1.x - dPos1 * dUV2.x)*r;
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

	m.active_mask |= AM_TANGENT | AM_BITANGENT;
	return true;
}


uint32_t insert_attrib_value(const vec4& val,
							 std::map<vec4,uint32_t,std::less<vec4>>& ad,
							 std::vector<vec4> &attribute_data)
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
	std::map<vec4,uint32_t,std::less<vec4> > attribute_id[AID_COUNT];

	for(uint32_t a = 0 ; a < AID_COUNT; a++)
	{
		if(!(m.active_mask & (1<<a)))
			continue;
		const auto& iad = m.attribute_data[a];
		auto oad = r.attribute_data[a];
		auto& aid = attribute_id[a];
		for(auto t:m.triangles)
		{
			for(auto& v  : t)
			{
				v.att_id[a] = insert_attrib_value(iad[v.att_id[a]],aid,oad);
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


std::string MeshOPS::m_errmsg = "";
ErrorCode MeshOPS::m_errcde=NO_ERROR;
}
