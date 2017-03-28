#include <mesh.h>

#include <string>
#include <sstream>
#include <algorithm>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


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

bool MeshOPS::load_OBJ(Mesh &m, const std::string &path)
{
	std::fstream f;
	f.open(path);
	if(!f.is_open())
		return  false;

	std::string line;
	glm::vec4 v;
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
			m.insert_attrib_value(AID_POSITION,v);
		}
		else if(lt == "vn")
		{
			line_str>>v.x>>v.y>>v.z>>v.w;
			m.insert_attrib_value(AID_NORMAL,v);
		}
		else if(lt == "vt")
		{
			line_str>>v.x>>v.y>>v.z>>v.w;
			m.insert_attrib_value(AID_TEXCOORD,v);
		}
		else if(lt == "f")
		{
			std::string v;
			Triangle t;
			for( uint32_t i = 0; i < 3; i++)
			{
				v = read_word(line_str);
				if(m.attribute_data[AID_NORMAL].empty() && m.attribute_data[AID_TEXCOORD].empty())  // only positions
				{
					sscanf(v.c_str(),"%d",
						   &(t[i].pos_id));
				}
				else if (m.attribute_data[AID_NORMAL].empty())	// positions and texcoords
				{
					sscanf(v.c_str(),"%d/%d",
						   &(t[i].pos_id),
						   &(t[i].tex_id));
				}
				else if (m.attribute_data[AID_TEXCOORD].empty())	 // positions and normals
				{
					sscanf(v.c_str(),"%d//%d",
						   &(t[i].pos_id),
						   &(t[i].nrn_id));
				}
				else		// all three
				{
					sscanf(v.c_str(),"%d/%d/%d",
						   &(t[i].pos_id),
						   &(t[i].tex_id),
						   &(t[i].nrn_id));
				}
			}
			m.triangles.push_back(t);
		}
	}
	return true;
}


glm::vec4 get_weighted_normal(const Mesh&m, const uint32_t t_id)
{
	const auto t  = m.triangles[t_id];
	const glm::vec4 A = (m.get_pos_data()[t[1].pos_id])- m.get_pos_data()[t[0].pos_id];
	const glm::vec4 B = (m.get_pos_data()[t[2].pos_id])- m.get_pos_data()[t[0].pos_id];

	const glm::vec3 a(A);//.x,A.y,A.z);
	const glm::vec3 b(B);//.x,B.y,B.z);


	const glm::vec3 cr = glm::cross(a,b);
	float area = 0.5f* glm::length(cr);
	auto n = glm::normalize(cr);
	const float w = area/((glm::dot(a,a)*glm::dot(b,b)));
	return glm::vec4(n*w,0.0f);
}

void MeshOPS::recalculate_normals(Mesh &m)
{
	std::vector<glm::vec4> norms(m.get_pos_data().size(),
								 glm::vec4(0.0f));
	for(uint i = 0 ; i < m.triangles.size();i++)
	{
		auto& t = m.triangles[i];
		auto n = get_weighted_normal(m,i);
		for(uint j = 0 ;j < 3;j++)
		{
			norms[t[j].pos_id] += n;
		}
	}

	for(auto& a : norms)
	{
		a = glm::normalize(a);
	}

	for(uint i = 0 ; i < m.triangles.size();i++)
	{
		auto& t = m.triangles[i];
		for(uint j = 0 ;j < 3;j++)
		{
			t[j].nrn_id = m.insert_attrib_value(AID_NORMAL,norms[t[j].pos_id]);
		}
	}
}

void MeshOPS::recalculate_tan_bnm(Mesh &m)
{

	std::vector<glm::vec4> tans(m.get_pos_data().size(),
								 glm::vec4(0.0f));
	std::vector<glm::vec4> bnms(m.get_pos_data().size(),
								 glm::vec4(0.0f));

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
		glm::vec4 bnm = (dPos2 * dUV1.x - dPos1 * dUV2.x)*r;
		tan.w = bnm.w = 0.0f;
		tan = glm::normalize(tan);
		bnm  = glm::normalize(bnm);
		for(uint j = 0 ;j < 3;j++)
		{
			tans[t[j].pos_id] += tan;
			bnms[t[j].pos_id] += bnm;
		}
	}

	for(auto& a : tans)
	{
		a = glm::normalize(a);
	}

	for(auto& a : bnms)
	{
		a = glm::normalize(a);
	}

	for(uint i = 0 ; i < m.triangles.size();i++)
	{
		auto& t = m.triangles[i];
		for(uint j = 0 ;j < 3;j++)
		{
			t[j].tan_id = m.insert_attrib_value(AID_TANGENT,tans[t[j].pos_id]);
			t[j].bnm_id = m.insert_attrib_value(AID_BINORMAL,bnms[t[j].pos_id]);
		}
	}

}
