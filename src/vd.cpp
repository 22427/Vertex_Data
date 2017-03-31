#include "vd.h"
#define GLM_FORCE_RADIANS
#include <glm/gtx/norm.hpp>
#include <algorithm>
#include <sstream>



/********* MATHS and other operations *****************************************/


template<typename T>
inline typename std::enable_if<std::is_floating_point<T>::value || std::is_class<T>::value, T>::type
float_to_nint(float /*f*/)
{
	static_assert(true, "Conversion only to integer types!");
}

/**
 * @brief float_to_nint converts a float in [0,1] to an unsigned integer in [0,max].
 * This is done with respect to the OpenGL spec.
 * @param f float to confert
 * @return the integer representing f
 */
template<typename T>
inline typename std::enable_if<std::is_unsigned<T>::value, T>::type
float_to_nint(const float f)
{
	return static_cast<T>(f*std::numeric_limits<T>::max());
}


/**
 * @brief float_to_nint converts a float in [-1,1] to an integer in [min,max].
 * This is done with respect to the OpenGL spec.
 * @param f float to confert
 * @return the integer representing f
 */
template<typename T>
inline typename std::enable_if<std::is_signed<T>::value, T>::type
float_to_nint(const float f)
{
	return static_cast<T>(f*((1<<((sizeof(T)*8)-1))-1));
}


template<typename T>
/**
 * @brief float_to_nint converts a float in [-1,1] to an integer t in [min,max],
 * or converts a float in [0,1] to an unsigned integer in [0,max].
 * @param f float to convert
 * @param t int/uint to convert into.
 */
inline void float_to_nint(const float f, T& t)
{
	t = float_to_nint<T>(f);// static_cast<T>(f*((1<<((sizeof(T)*8)))-1));
}


/**
 * @brief nint_to_float converts a nint in [min,max] to the corresponding float in [-1,1]
 * @param x nint to convert
 * @return converted float
 */
template<typename T>
inline typename std::enable_if<std::is_signed<T>::value, float>::type
nint_to_float(const T x)
{
	const T t = ((1<<((sizeof(T)*8)-1))-1);
	return std::max(static_cast<float>(x)/t,-1.0f);
}


/**
 * @brief nint_to_float converts a unsigned nint in [0,max] to the corresponding float in [0,1]
 * @param x unsigned nint to convert
 * @return converted float
 */
template<typename T>
inline typename std::enable_if<std::is_unsigned<T>::value, float>::type
nint_to_float(const T x)
{
	const T t = std::numeric_limits<T>::max();
	return static_cast<float>(x)/t;
}



/**
 * @brief ltrim removes all whitespaces on the left side of s
 * @param s
 */
static inline void ltrim(std::string &s)
{
	s.erase(s.begin(),std::find_if(s.begin(), s.end(),
								   std::not1(std::ptr_fun<int, int>(std::isspace))));
}

/**
 * @brief ltrim removes all whitespaces on the right side of s
 * @param s
 */
static inline void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
						 std::not1(std::ptr_fun<int,int>(std::isspace))).base(),
			s.end());
}

/**
 * @brief ltrim removes all whitespaces on the both sides of s
 * @param s
 */
static inline void trim(std::string &s){ltrim(s);rtrim(s);}



std::string Type::to_string() const
{
	switch (id)
	{
	case BYTE: return "byte";
	case UNSIGNED_BYTE: return "ubyte";
	case SHORT: return "short";
	case UNSIGNED_SHORT: return "ushort";
	case INT: return "int";
	case UNSIGNED_INT: return "uint";
	case FLOAT: return "float";
	case DOUBLE: return "double";
	case INVALID: return "invalid";
	}
	return "invalid";
}

size_t Type::size() const
{
	switch (id)
	{
	case BYTE:
	case UNSIGNED_BYTE:
		return  1;
	case SHORT:
	case UNSIGNED_SHORT:
		return  2;
	case INT:
	case UNSIGNED_INT:
	case FLOAT:
		return  4;
	case DOUBLE:
		return  8;
	case INVALID: static_assert(true,"unknown type!");
	}
	return  0;
}

double Type::max() const
{
	switch (id)
	{
	case BYTE: return std::numeric_limits<int8_t>::max();
	case UNSIGNED_BYTE: return std::numeric_limits<uint8_t>::max();
	case SHORT: return std::numeric_limits<int16_t>::max();
	case UNSIGNED_SHORT: return std::numeric_limits<uint16_t>::max();
	case INT: return std::numeric_limits<int32_t>::max();
	case UNSIGNED_INT: return std::numeric_limits<uint32_t>::max();
	case FLOAT: return static_cast<double>(std::numeric_limits<float>::max());
	case DOUBLE: return std::numeric_limits<double>::max();
	case INVALID: static_assert(true,"unknown type!");
	}
	return  0;
}

double Type::min() const
{
	switch (id)
	{
	case BYTE: return std::numeric_limits<int8_t>::min();
	case UNSIGNED_BYTE: return std::numeric_limits<uint8_t>::min();
	case SHORT: return std::numeric_limits<int16_t>::min();
	case UNSIGNED_SHORT: return std::numeric_limits<uint16_t>::min();
	case INT: return std::numeric_limits<int32_t>::min();
	case UNSIGNED_INT: return std::numeric_limits<uint32_t>::min();
	case FLOAT: return static_cast<double>(std::numeric_limits<float>::min());
	case DOUBLE: return std::numeric_limits<double>::min();
	case INVALID: static_assert(true,"unknown type!");
	}
	return  0;
}


size_t Attribute::size() const
{
	return  (type.size())*elements;
}


Attribute::Attribute(
		const AttributeID id,
		const uint16_t elements,
		const Type type,
		const bool normalized,
		bool use_constant,
		const void* conststant)

	:attribute_id(id),
	  encoding(EN_NONE),
	  type(type),
	  offset(0),
	  elements(elements),
	  normalized(normalized),
	  use_constant(use_constant)

{
	if(use_constant)
	{
		memcpy(this->constant,conststant,size());
	}
	offset = 0;
}

void Attribute::write_constant(const vec4 &v)
{
	convert(constant,v);
}

void *Attribute::convert(void *dst, const glm::vec4 &v) const
{
	if( type == BYTE)
	{
		byte* d = reinterpret_cast<byte*>(dst);
		for(int32_t i = 0; i < elements; i++)
		{
			*d= normalized? float_to_nint<byte>(v[i]): static_cast<byte>(v[i]);
			d++;
		}
		return d;
	}
	if( type == UNSIGNED_BYTE)
	{
		uint8_t* d = reinterpret_cast<uint8_t*>(dst);
		for(int32_t i = 0; i < elements; i++)
		{
			*d= normalized? float_to_nint<uint8_t>(v[i]): static_cast<uint8_t>(v[i]);
			d++;
		}
		return d;
	}
	if( type == SHORT)
	{
		int16_t* d = reinterpret_cast<int16_t*>(dst);
		for(int32_t i = 0; i < elements; i++)
		{
			*d= normalized? float_to_nint<int16_t>(v[i]): static_cast<int16_t>(v[i]);
			d++;
		}
		return d;
	}
	if( type == UNSIGNED_SHORT)
	{
		uint16_t* d = reinterpret_cast<uint16_t*>(dst);
		for(int32_t i = 0; i < elements; i++)
		{
			*d= normalized? float_to_nint<uint16_t>(v[i]): static_cast<uint16_t>(v[i]);
			d++;
		}
		return d;
	}
	if( type == INT)
	{
		uint32_t* d = reinterpret_cast<uint32_t*>(dst);
		for(int32_t i = 0; i < elements; i++)
		{
			*d= normalized? float_to_nint<uint32_t>(v[i]): static_cast<uint32_t>(v[i]);
			d++;
		}
		return d;
	}
	if( type == UNSIGNED_INT)
	{
		int32_t* d = reinterpret_cast<int32_t*>(dst);
		for(int32_t i = 0; i < elements; i++)
		{
			*d= normalized? float_to_nint<int32_t>(v[i]): static_cast<int32_t>(v[i]);
			d++;
		}
		return d;
	}
	if( type == FLOAT)
	{
		float* d = reinterpret_cast<float*>(dst);
		for(int32_t i = 0; i < elements; i++)
		{
			*d= v[i];
			d++;
		}
		return d;
	}

	return dst;
}

VertexData::VertexData(Primitive primitive,
					   VertexConfiguration cfg,
					   const Type index_type ,
					   const uint res_vtx,
					   const uint res_idx )
	:
	  m_vtx_configuration(cfg),
	  m_render_primitive(primitive),
	  m_index_type(index_type),
	  m_index_count(0u),
	  m_vertex_count(0u),
	  m_index_data(nullptr),
	  m_index_reserve(0u),
	  m_vertex_data(nullptr),
	  m_vertex_reserve(0u)
{
	if(res_vtx)
		vertices_reserve(res_vtx);


	if(res_idx)
		indices_reserve(res_idx);

}

void VertexData::vertices_reserve(const uint c)
{
	if(m_vertex_reserve < c)
	{
		m_vertex_reserve = c;
		if(!m_vertex_data)
		{
			m_vertex_data = static_cast<ubyte*>(malloc(c*m_vtx_configuration.size()));
		}
		else
		{
			m_vertex_data = static_cast<ubyte*>(realloc(m_vertex_data,m_vertex_reserve*m_vtx_configuration.size()));
		}
	}
}

void VertexData::indices_reserve(const uint c)
{
	if(m_index_reserve < c)
	{
		m_index_reserve = c;
		if(!m_index_data)
		{
			m_index_data = static_cast<ubyte*>(malloc(c*m_index_type.size()));
		}
		else
		{
			m_index_data = static_cast<ubyte*>(realloc(m_index_data,m_index_reserve*m_index_type.size()));
		}
	}
}

void VertexData::push_back(const uint32_t &i)
{
	indices_reserve(m_index_count+1);


	if(m_index_reserve <= m_index_count+1)
	{
		if(!m_index_data)
		{
			indices_reserve(1u);
		}
		else
		{
			const auto to_reserve = std::min(m_index_reserve*2u,m_index_reserve+8u);
			indices_reserve(to_reserve);
		}
	}

	switch (m_index_type.id)
	{
	case UNSIGNED_BYTE: static_cast<uint8_t*>(m_index_data)[m_index_count] = static_cast<uint8_t>(i);break;
	case UNSIGNED_SHORT: static_cast<uint16_t*>(m_index_data)[m_index_count] = static_cast<uint16_t>(i);break;
	case UNSIGNED_INT: static_cast<uint32_t*>(m_index_data)[m_index_count] = static_cast<uint32_t>(i);break;
	case BYTE:case SHORT:case INT: case FLOAT: case DOUBLE: case INVALID:
		static_assert(true, "invalid index type!!!");
	}

	m_index_count++;
}

uint32_t VertexData::get_index(const uint32_t i) const
{
	assert(m_index_type.is_integer() && m_index_type.is_unsigned());

	switch (m_index_type.id)
	{
	case UNSIGNED_BYTE: return static_cast<uint8_t* >(m_index_data)[i];
	case UNSIGNED_SHORT:return static_cast<uint16_t*>(m_index_data)[i];
	case UNSIGNED_INT:  return static_cast<uint32_t*>(m_index_data)[i];
	case BYTE:case SHORT:case INT: case FLOAT: case DOUBLE: case INVALID:
		return ~0u;
	}
	return  ~0u;
}

void VertexData::set_index(const uint32_t i, const uint32_t v)
{
	assert(m_index_type.is_integer() && m_index_type.is_unsigned());

	switch (m_index_type.id)
	{
	case UNSIGNED_BYTE:  static_cast<uint8_t* >(m_index_data)[i] =
				static_cast<uint8_t >(v);break;
	case UNSIGNED_SHORT: static_cast<uint16_t*>(m_index_data)[i] =
				static_cast<uint16_t>(v);break;
	case UNSIGNED_INT:   static_cast<uint32_t*>(m_index_data)[i] =
				static_cast<uint32_t>(v);break;
	case BYTE:case SHORT:case INT: case FLOAT: case DOUBLE: case INVALID:
		break;
	}
}


Primitive VertexData::primitive() const {return  m_render_primitive;}

void VertexData::set_primitive(const Primitive &p){m_render_primitive = p;}



struct VtxComperator
{
	size_t vs;

	bool operator()(const void* const & a,const void* const & b) const
	{
		return memcmp(a,b,vs) <0;
	}
};

bool VertexDataOPS::read(VertexData &vd,std::ifstream &f)
{
	f.read((char*)(&(vd.m_vtx_configuration)),sizeof(VertexConfiguration));
	f.read((char*)&(vd.m_render_primitive),sizeof(Primitive));
	f.read((char*)&(vd.m_index_type),sizeof(Type));
	f.read((char*)&(vd.m_index_count),sizeof(uint32_t));
	f.read((char*)&(vd.m_vertex_count),sizeof(uint32_t));
	vd.indices_reserve(vd.m_index_count);
	vd.vertices_reserve(vd.m_vertex_count);
	f.read((char*)&(vd.m_index_data),vd.index_count()*vd.index_type().size());
	f.read((char*)&(vd.m_vertex_data),vd.vertex_count()*vd.m_vtx_configuration.size());
	return true;
}

bool VertexDataOPS::read(VertexData &vd, const std::string &path)
{
	std::ifstream f;
	f.open(path,std::ifstream::binary);
	if(!f.is_open())
		return  false;
	return read(vd,f);
}



bool VertexDataOPS::write(const VertexData &vd, std::ofstream &f)
{
	f.write((char*)(&(vd.m_vtx_configuration)),sizeof(VertexConfiguration));
	f.write((char*)&(vd.m_render_primitive),sizeof(Primitive));
	f.write((char*)&(vd.m_index_type),sizeof(Type));
	f.write((char*)&(vd.m_index_count),sizeof(uint32_t));
	f.write((char*)&(vd.m_vertex_count),sizeof(uint32_t));
	f.write((char*)&(vd.m_index_data),vd.index_count()*vd.index_type().size());
	f.write((char*)&(vd.m_vertex_data),vd.vertex_count()*vd.m_vtx_configuration.size());
	return  true;
}

bool VertexDataOPS::write(const VertexData &vd, const std::string &path)
{
	std::ofstream f;
	f.open(path,std::ofstream::binary);
	if(!f.is_open())
		return  false;
	return write(vd,f);
}

void VertexDataOPS::pack_from_mesh(VertexData &vd, const Mesh *m)
{
	const size_t vertex_size = vd.m_vtx_configuration.size();


	VtxComperator cmprtr;
	cmprtr.vs = vertex_size;
	// we use two maps one is coarse maping vertices by their indices to
	// ids the oder is fine to map converted vertex values to indices.
	std::map<void*,uint32_t,VtxComperator> fine(cmprtr);
	std::map<Vertex,uint32_t,vertex_active_comperator> coarse;


	// set all coareses to a unvalid index
	for(const auto& t: m->triangles)
	{
		for(const auto& v : t)
		{
			coarse[v]=UINT32_MAX;
		}
	}
	// check for an upper limit of vertices we will deal with
	if(coarse.size() <= UCHAR_MAX)
		vd.index_type() = UNSIGNED_BYTE;
	else if(coarse.size() <= USHRT_MAX)
		vd.index_type() = UNSIGNED_SHORT;
	else
		vd.index_type() = UNSIGNED_INT;

	// ok lets reserve some memory based on the upper limit
	vd.vertices_reserve(static_cast<uint32_t>(coarse.size()));
	// reset vertex count
	vd.vertex_count() = 0;
	// the index count is fixed there will be 3 indices per triangle
	vd.indices_reserve(static_cast<uint32_t>(m->triangles.size())*3);
	// the only supported primitve is triangles so go with it!
	vd.set_primitive(PRIM_TRIANGLES);

	// allocate a proxy vertex for conversion to
	void *vv = malloc(vertex_size);
	// the final data will be written to vertex data.
	char *s = static_cast<char*>(vd.vertex_data());

	// lets go for all triangles ...
	for(const auto& t: m->triangles)
	{
		// ... and all vertices inside the triangles
		for(const auto& v : t)
		{
			// do we know this index allready ?
			if(coarse[v]== UINT32_MAX) // no
			{
				// create a working pointer and convert the vertex data into vv
				void *vs = vv;
				for(int j = 0 ; j< AID_COUNT;j++)
				{
					if(!(vd.m_vtx_configuration.active_mask & (1<<j))) // not interested
						continue;
					if(!(m->active_mask & (1<<j))) // not given by the mesh
					{
						vs=vd.m_vtx_configuration.attributes[j].convert(vs,glm::vec4(0,0,0,0));
					}
					else if(vd.m_vtx_configuration.active_mask & (1<<j)) // available
						vs=vd.m_vtx_configuration.attributes[j].convert(vs,m->attribute_data[j][v.att_id[j]]);
				}
				// do we know this converted vertex?
				if(fine.find(vv) == fine.end()) // no
				{

					// ok write it to s
					memcpy(s,vv,vertex_size);
					s+=vertex_size;

					// remember its id in both maps
					coarse[v]=vd.vertex_count();
					fine[vv]=vd.vertex_count();

					vd.vertex_count()+=1;
				}
				else // yes
				{
					//remenber its coarse id for future use
					coarse[v] = fine[vv];
				}
			}
			// ok now push back the index!
			vd.push_back(coarse[v]);
		}
	}

}
