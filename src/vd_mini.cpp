#include "../include/vd_mini.h"
#include <glm/common.hpp>

namespace vd {

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

void MiniVertex::set(const glm::vec3 &p, const glm::vec3 &n, const glm::vec3 &t)
{
	for(int  i = 0 ; i< 3;i++)
	{
		pos[i] = float_to_nint<int16_t>(p[i]);
		nrm[i] = float_to_nint<int16_t>(n[i]);
		if(i<2)
		{
			tex[i] = float_to_nint<uint16_t>(t[i]);
		}
	}
}

MiniVD::MiniVD()
{
	pos_scale= glm::vec3(1.0f);
	index_size = data_size = 0;
	index = nullptr;
	data = nullptr;

}

void MiniVD::read_mesh(const Mesh &m)
{
	// get scale and offset
	glm::vec3 bb_min(+1000000);
	glm::vec3 bb_max(-1000000);
	for(const auto& v : m.get_pos_data())
	{
		pos_offset = pos_offset+glm::vec3(v);
		bb_min = glm::min(bb_min,glm::vec3(v));
		bb_max = glm::max(bb_min,glm::vec3(v));
	}
	pos_offset = pos_offset/static_cast<float>(m.get_pos_data().size());

	bb_min -= pos_offset;
	bb_max -= pos_offset;
	bb_min = glm::abs(bb_min);
	bb_max = glm::abs(bb_max);
	pos_scale = glm::max(bb_max,bb_min);

	std::map<MeshVertex,uint16_t> vid;
	for(const auto& t : m.triangles)
	{
		for(const auto& v: t)
		{
			vid[v]=UINT16_MAX;
		}
	}

	index_size = static_cast<uint32_t>(m.triangles.size()*2*3);
	index = static_cast<uint16_t*>(realloc(index,index_size));

	data_size = static_cast<uint32_t>(vid.size()*sizeof(MiniVertex));
	data = static_cast<MiniVertex*>(realloc(data,data_size));
	uint32_t curr_index  =0 ;
	uint16_t curr_vertex  =0 ;

	for(const auto& t : m.triangles)
	{
		for(const auto& v: t)
		{
			const auto id = vid[v];
			if(id == UINT16_MAX)
			{
				vid[v] = curr_vertex;
				data[curr_index].set((glm::vec3(m.get_pos_data()[v.pos_id])-pos_offset)/pos_scale,
						glm::vec3(m.get_nrm_data()[v.nrm_id]),
						glm::vec3(m.get_tex_data()[v.tex_id]));
				index[curr_index] = curr_vertex;
				curr_vertex ++;
			}
			else
			{
				index[curr_index] = id;
			}
			curr_index ++;
		}
	}
}

bool MiniVertexDataOPS::write(const MiniVD &vd, std::ofstream &f)
{
	if(!f.is_open())
		return false;
#define write_fixed(f,x) (f).write(reinterpret_cast<const char*>(&( x )),sizeof( x ) )
	write_fixed(f,vd.data_size);
	write_fixed(f,vd.index_size);
	write_fixed(f,vd.pos_offset);
	write_fixed(f,vd.pos_scale);
#undef write_fixed
	f.write(reinterpret_cast<const char*>(&(vd.data)),vd.data_size);
	f.write(reinterpret_cast<const char*>(&(vd.index)),vd.index_size);
	return  true;
}

bool MiniVertexDataOPS::write(const MiniVD &vd, const std::string &path)
{
	std::ofstream f(path, std::ofstream::binary);
	return  write(vd,f);
}

bool MiniVertexDataOPS::read(MiniVD &vd, std::ifstream &f)
{
	if(!f.is_open())
		return false;
#define read_fixed(f,x) (f).read(reinterpret_cast<char*>(&( x )),sizeof( x ) )
	read_fixed(f,vd.data_size);
	read_fixed(f,vd.index_size);
	read_fixed(f,vd.pos_offset);
	read_fixed(f,vd.pos_scale);
#undef read_fixed
	f.read(reinterpret_cast<char*>(&(vd.data)),vd.data_size);
	f.read(reinterpret_cast<char*>(&(vd.index)),vd.index_size);
	return  true;
}

bool MiniVertexDataOPS::read(MiniVD &vd, const std::string &path)
{
	std::ifstream f(path, std::ifstream::binary);
	return  read(vd,f);
}
}
