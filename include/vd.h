#pragma once
#include <cstring>
#include <fstream>

#include <map>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "vd_mesh.h"
#include "vd_dll.h"
#include <limits>

namespace vd {

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef uint8_t ubyte;
typedef int8_t byte;
}
namespace std {
/**
 * vecN hashes according to
 * Optimized Spatial Hashing for Collision Detection of Deformable Objects.
 * ------------------------------------------------------------------------
 * M. Teschner, B. Heidelberger, M. Müller, D. Pomerantes, und M. Gross. VMV,
 * Pages 47-54. Aka GmbH, (2003)
 *
 * this is a good way to go for hashing spacial vectors.
 *
 */
template <> struct hash<glm::vec2>
{
	size_t operator()(const glm::vec2 & x) const
	{
		const uint32_t* d = reinterpret_cast<const uint32_t*>(&(x.x));
		if(sizeof (size_t) >= 8)
		{
			size_t r =(d[0]);
			r= r<<32;
			r |= d[1];
			return r;
		}
		return ((73856093ULL*d[0])^(19349663ULL*d[1]))
				%(numeric_limits<size_t>::max());
	}
};

template <> struct hash<glm::vec3>
{
	size_t operator()(const glm::vec3 & x) const
	{
		const uint32_t* d = reinterpret_cast<const uint32_t*>(&(x.x));
		if(sizeof (size_t) >= 8)
		{
			size_t r =(d[0]);
			r=r<<32;
			r |= d[1];
			r ^= d[2];
			return r;
		}
		return (73856093ULL*(d[0])^
				(19349663ULL*d[1])^
				(83492791ULL*d[2]))
				%(numeric_limits<size_t>::max());
	}
};


template <> struct hash<glm::vec4>
{
	size_t operator()(const glm::vec4 & x) const
	{
		const uint32_t* d = reinterpret_cast<const uint32_t*>(&(x.x));
		if(sizeof (size_t) >= 8)
		{
			size_t r =(d[0]^d[1]);
			r = r<<32;
			r |= d[2]^d[3];
			return r;
		}
		return ((73856093ULL*d[0])^
				(19349663ULL*d[1])^
				(83492791ULL*d[2])^
				(15495323ULL*d[3]))
				%(numeric_limits<size_t>::max());
	}
};

}

namespace vd {
///*****************************************************************************
/// TYPE                                                                       *
/// ****************************************************************************

enum TypeID
{
	INVALID = 0x00,
	BYTE =0x1400,				// == GL_BYTE
	UNSIGNED_BYTE =0x1401,		// == GL_UNSIGNED_BYTE

	SHORT =0x1402,				// == GL_SHORT
	UNSIGNED_SHORT= 0x1403,		// == GL_UNSIGNED_SHORT

	INT= 0x1404,				// == GL_INT
	UNSIGNED_INT= 0x1405,		// == GL_UNSIGNED_INT

	FLOAT= 0x1406,				// == GL_FLOAT
	DOUBLE =0x140A,				// == GL_DOUBLE
};

/**
 * @brief The Type class dealing with different data types
 */
class DLL_PUBLIC Type
{
public:
	TypeID id;

	Type(const TypeID id = INVALID):id(id){}
	Type(const unsigned int id)	:id(static_cast<TypeID>(id)){}
	Type(const int id)	:id(static_cast<TypeID>(id)){}
	Type(std::string str);

	operator TypeID() const {return  id;}
	operator unsigned int() const {	return  static_cast<unsigned int>(id);}
	operator int() const {	return  static_cast<int>(id);}

	std::string to_string() const;
	/**
	 * @brief is_integer test if this type is an integer
	 * @return true if this is an integer
	 */
	bool is_integer() const { return !(id == DOUBLE || id == FLOAT);}

	/**
	 * @brief is_float test if this type is a floating point data type
	 * @return true if this is a float or a double
	 */
	bool is_float() const{return (id == DOUBLE || id == FLOAT);}

	/**
	 * @brief is_unsigned tests for a sign
	 * @return true if this has no sign
	 */
	bool is_unsigned() const
	{
		return (id==UNSIGNED_BYTE||id==UNSIGNED_SHORT||id==UNSIGNED_INT);
	}
	/**
	 * @brief is_unsigned tests for a sign
	 * @return true if this has a sign
	 */
	bool is_signed() const
	{
		return !(id==UNSIGNED_BYTE||id==UNSIGNED_SHORT||id==UNSIGNED_INT);
	}

	bool operator ==(const Type& o)const{return  o.id == id;}
	bool operator ==(const TypeID id)const{return  this->id == id;}
	bool operator !=(const TypeID id)const{	return  this->id != id;}
	bool operator !=(const Type& o) const{return  o.id != id;}

	/**
	 * @brief size returns the size of one element of this type.
	 * @return
	 */
	size_t size() const;

	/**
	 * @brief max gives the maximal value a variable of this type can store
	 * @return the max value in double
	 */
	double max() const;

	/**
	 * @brief min gives the minimal positive value a variable of this type can
	 * store
	 * @return the min value in double
	 */
	double min() const;
	/**
	 * @brief lowest gives the absolute lowest value a variable of this type can
	 * store
	 * @return  the lowest value in double
	 */
	double lowest() const;
};






///*****************************************************************************
/// ATTRIBUTE                                                                  *
/// ****************************************************************************

enum Encoding
{
	EN_NONE,
	EN_NORM_SCALE, // if this encoding is set, the data is encoded as normalized
				   // int, but it needs to be scaled by the constant, which is
				   // then encoded as floats!
};

/**
 * @brief The Attribute class represents the configuration of a value stored for
 * each vertex. This is designed to make it easy to set an Vertex attribute
 * pointer in opengl.
 */
class  DLL_PUBLIC Attribute
{
public:
	AttributeID attribute_id;
	Encoding encoding;
	Type type;
	uint32_t offset;
	uint16_t elements;
	uint8_t normalized; // defacto bool
	uint8_t use_constant; // defacto bool
	ubyte constant[4*sizeof(float)];

	size_t size() const;
	Attribute()
	{
		encoding= EN_NONE;
		attribute_id=AID_COUNT;
		elements = 0;
		type = FLOAT;
		offset = 0;
		normalized = false;
		use_constant = false;
		memset(constant,0,4*sizeof(float));
	}

	Attribute(const AttributeID id,
			  const uint16_t elements =0,
			  const Type type = FLOAT,
			  const bool normalized= false,
			  bool use_constant = false,
			  const void* constant = nullptr);

	void write_constant(const vec4& v);

	void write_constant(const vec3& v)
	{
		write_constant(vec4(v,0.0f));
	}
	void write_constant(const vec2& v)
	{
		write_constant(vec4(v,0.0f,0.0f));
	}
	void write_constant(const float v)
	{
		write_constant(vec4(v,0.0f,0.0f,0.0f));
	}


	bool operator == (const Attribute& o)const
	{
		bool same = true;
		same &= attribute_id == o.attribute_id;
		same &= elements == o.elements;
		same &= type == o.type;
		same &= normalized == o.normalized;
		same &= offset == o.offset;
		same &= use_constant == o.use_constant;
		if(use_constant)
			same &= memcmp(constant,o.constant,4*sizeof(uint64_t));
		return  same;
	}

	bool operator != (const Attribute& o)const
	{
		return  !(*this==o);
	}

	void* convert(void* dst, const glm::vec4& v) const;
	const void* convert(const void* src, glm::vec4& v) const;

};

///*****************************************************************************
///* VERTEX CONFIGURATION                                                      *
///*****************************************************************************

/**
 * @brief The VertexConfiguration class Is a set of attributes resembling a
 * vertex.
 */
class DLL_PUBLIC VertexConfiguration
{
public:
	Attribute attributes[AID_COUNT];
	uint32_t active_mask;
	uint32_t m_size;
	uint32_t size() const
	{
		return m_size;
	}

	void set_attribute(const AttributeID id,
					   const uint16_t elements,
					   const Type type,
					   const bool normalized,
					   bool use_constant = false,
					   const void* constant = nullptr)
	{
		active_mask= active_mask | (1<<id);
		auto& a = attributes[id];
		a.attribute_id = id;
		a.elements = elements;
		a.type = type;
		a.normalized = normalized;
		a.use_constant = use_constant;
		if(constant)
			memcpy(a.constant,constant,a.size());

		m_size = 0;
		for(auto& a : attributes)
		{
			if(!a.use_constant)
			{
				a.offset = m_size;
				m_size+= a.size();
			}
			else
				a.offset = 0;
		}
	}

	VertexConfiguration()
	{
		clear();
	}

	void clear()
	{
		m_size=0;
		for(uint32_t i = 0 ; i< AID_COUNT;i++)
		{
			attributes[i]= Attribute(static_cast<AttributeID>(i));
		}
		active_mask=0;
	}


};

///*****************************************************************************
///* VERTEX DATA                                                               *
///*****************************************************************************

enum Primitive
{
	PRIM_POINTS = 0x0000,			// == GL_POINTS
	PRIM_LINES = 0x0001,			// == GL_LINES
	PRIM_LINE_LOOP = 0x0002,		// == GL_LINE_LOOP
	PRIM_LINE_STRIP = 0x0003,		// == GL_LINE_STRIP
	PRIM_TRIANGLES = 0x0004,		// == GL_TRIANGLES
	PRIM_TRIANGLE_STRIP = 0x0005,	// == GL_TRIANGLE_STRIP
	PRIM_TRIANGLE_FAN = 0x0006,		// == GL_TRIANGLE_FAN
};

/**
 * @brief The VertexData class represents vertex data in a renderable form.
 */
class DLL_PUBLIC VertexData
{
private:
	VertexConfiguration m_vtx_cfg;
	Primitive m_render_primitive;
	Type m_index_type;
	uint m_index_count;
	uint m_vertex_count;


	void* m_index_data;
	uint m_index_reserve;

	void* m_vertex_data;
	uint m_vertex_reserve;

public:
	friend class VertexDataOPS;

	/**
	 * @brief VertexData Constructor
	 * @param primitive The primitive type
	 * @param cfg The vertex configuratio
	 * @param index_type the index type
	 * @param res_vtx number of vertices you wish to reserve for
	 * @param res_idx number of indices you wish to reserve for
	 */
	VertexData(Primitive primitive = PRIM_TRIANGLES,
			   VertexConfiguration cfg = VertexConfiguration(),
			   const Type index_type = Type(UNSIGNED_SHORT),
			   const uint res_vtx = 0,
			   const uint res_idx = 0);

	virtual ~VertexData()
	{
		if(m_vertex_data)
			free(m_vertex_data);
		if(m_index_data)
			free(m_index_data);
	}

	/**
	 * @brief vertex_configuration gives access to the vertex configuration
	 * @return
	 */
	const VertexConfiguration& vertex_configuration() const {return m_vtx_cfg;}
	/**
	 * @brief vertex_count
	 * @return number of vertices stored.
	 */

	const uint& vertex_count()const{return m_vertex_count;}
	/**
	 * @brief vertices_reserve will allocate memory to hold at least c vertices
	 * @param c
	 */
	void vertices_reserve(const uint c);

	/**
	 * @brief vertices_resize will (re)allocate memory for exactly c vertices.
	 * the vertex count will be set accordingly.
	 * @param c
	 */
	void vertices_resize(const uint c);

	/**
	 * @brief verties_null will set the whole reserved vertex space to 0!
	 */
	void verties_null()
	{
		memset(m_vertex_data,0,m_vertex_reserve*m_vtx_cfg.size());
	}

	/**
	 * @brief push_back adds a Vertex to the vertex list.
	 * @param v The new Vertex
	 * @return the index of the newly added vertex.
	 */
	uint push_back(const MeshVertex& v);

	/**
	 * @brief index_type
	 * @return The type of the indices. Should be some kind of unsigned integer
	 */
	Type index_type()const {return  m_index_type;}

	/**
	 * @brief index_count
	 * @return returns the number of stored indices.
	 */
	uint32_t index_count()const{return m_index_count;}

	/**
	 * @brief indices_reserve will reserve for at least c indices
	 * @param c
	 */
	void indices_reserve(const uint c);

	/**
	 * @brief indices_clear set the index count to zero.
	 * The reserved memory stays untouched.
	 */
	void indices_clear()
	{
		m_index_count = 0;
	}

	/**
	 * @brief indices_nullwill set the whole reserved index storage to 0!
	 */
	void indices_null()
	{
		memset(m_index_data,0,m_index_reserve*m_index_type.size());
	}
	/**
	 * @brief push_back adds an index to the index list
	 * @param i the new index
	 */
	void push_back(const uint32_t& i);


	/**
	 * @brief vertex_data gives write access to the vertex data.
	 * @return The vertex data.
	 */
	void* vertex_data(){return  m_vertex_data;}

	/**
	 * @brief vertex_data gives read access to the vertex data.
	 * @return The vertex data.
	 */
	const void* vertex_data()const{return  m_vertex_data;}

	/**
	 * @brief indices_clear set the index count to zero.
	 * The reserved memory stays untouched.
	 */
	void vertex_clear()
	{
		m_vertex_count = 0;
	}

	/**
	 * @brief indices  write access to the index data.
	 * @return The index data.
	 */
	void* indices(){return m_index_data;}

	/**
	 * @brief indices  read access to the index data.
	 * @return The index data.
	 */
	const void* indices()const{return m_index_data;}

	/**
	 * @brief get_index gives the value of a specific index i.
	 * @param i Index you wish to look up.
	 * @return The index' value as an uint32_t
	 */
	uint32_t get_index(const uint32_t i) const;

	/**
	 * @brief set_index sets a specifiv index to the value v.
	 * @param i Index you wish to set.
	 * @param v The value you wish to set it to.
	 */
	void set_index(const uint32_t i, const uint32_t v);

	/**
	 * @brief primitive
	 * @return The vertex datas primitive
	 */
	virtual Primitive primitive() const;
	/**
	 * @brief setPrimitive sets the primitive mode this vertex data is
	 * constructed in.
	 * @param p new primitive
	 */
	void set_primitive(const Primitive& p);

};

///*****************************************************************************
///* VERTEX DATA OPERATIONS                                                    *
///*****************************************************************************

class DLL_PUBLIC VertexDataOPS
{
	static std::string m_errmsg;
	static ErrorCode m_errcde;
public:
	/**
	 * @brief read reads VertexData from a given stream.
	 * @param vd VertexData to read to.
	 * @param f incomming stream to read from
	 * @return false if there was any error.
	 */
	static bool read(VertexData &vd, std::ifstream& f);
	/**
	 * @brief read reads VertexData from a file at the given path.
	 * @param vd VertexData to read to.
	 * @param path to the .vd file
	 * @return false if there was any error (including opening the file)
	 */
	static bool read(VertexData &vd, const std::string& path);

	/**
	 * @brief write writes the given VertexData to a stream
	 * @param vd VertexData to write from.
	 * @param f stream to write to.
	 * @return false if there was any error.
	 */
	static bool write(const VertexData &vd, std::ofstream& f);

	/**
	 * @brief write writes the given VertexData to a file at the given path.
	 * @param vd VertexDate to write from.
	 * @param path path to the .vd file to write to.
	 * @return false if ther was any error (including opening the file)
	 */
	static bool write(const VertexData &vd, const std::string& path);

	/**
	 * @brief from_mesh converts a mesh to VertexData
	 * @param vd
	 * @param m
	 */
	static void from_mesh(VertexData &vd, const Mesh m);

	/**
	 * @brief to_mesh converts VertexData back to a mesh.
	 * @param vd
	 * @param m
	 */
	static void to_mesh(const VertexData &vd, Mesh& m);

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



}
