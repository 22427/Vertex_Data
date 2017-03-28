#pragma once
#include <glm/glm.hpp>
#include <cstring>
#include <fstream>
#ifndef OFL_DLL_PUBLIC

#if defined _WIN32 || defined __CYGWIN__
#ifdef PT_BUILDING_DLL
#ifdef __GNUC__
#define OFL_DLL_PUBLIC __attribute__ ((dllexport))
#else
#define OFL_DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define OFL_DLL_PUBLIC __attribute__ ((dllimport))
#else
#define OFL_DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define OFL_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define OFL_DLL_PUBLIC __attribute__ ((visibility ("default")))
#define OFL_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define OFL_DLL_PUBLIC
#define OFL_DLL_LOCAL
#endif
#endif

#endif




#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <mesh.h>

namespace ofl
{
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef uint8_t ubyte;
typedef int8_t byte;

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
	return static_cast<T>(f*std::numeric_limits<T>::max()/*((1<<((sizeof(T)*8)))-1)*/);
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
	const T t = std::numeric_limits<T>::max();//((1<<((sizeof(T)*8)))-1);
	return static_cast<float>(x)/t;
}

/********* TYPE class dealing with different data types************************/

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

class OFL_DLL_PUBLIC Type
{
public:
	TypeID id;

	Type(const TypeID id = INVALID):id(id){}
	Type(const unsigned int id)	:id(static_cast<TypeID>(id)){}
	Type(const int id)	:id(static_cast<TypeID>(id)){}
	Type(const std::string& str);

	operator TypeID() const {return  id;}
	operator unsigned int() const {	return  static_cast<unsigned int>(id);}
	operator int() const {	return  static_cast<int>(id);}
	std::string to_string() const
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
	 * @brief min gives the minimal value a variable of this type can store
	 * @return the min value in double
	 */
	double min() const;
};




/**
 * @brief The Attribute class represents the configuration of a value stored for
 * each vertex. This is designed to make it easy to set an Vertex attribute
 * pointer in opengl.
 */
class  OFL_DLL_PUBLIC Attribute
{
public:
	AttributeID attribute_id;
	uint32_t elements;
	Type type;
	uint32_t offset;
	bool normalized;
	bool use_constant;
	ubyte constant[4*sizeof(uint64_t)];

	short _padding[1];

	uint32_t size() const;

	Attribute()
	{
		attribute_id=AID_COUNT;
		elements = 0;
		type = FLOAT;
		offset = 0;
		normalized = false;
		use_constant = false;
		memset(constant,0,4*sizeof(uint64_t));
	}

	Attribute(const AttributeID id,
			  const uint elements,
			  const Type type,
			  const bool normalized,
			  bool use_constant = false,
			  const void* constant = nullptr);

	bool write_constant(const vec4& v) ;
	bool write_constant(const vec3& v) ;
	bool write_constant(const vec2& v) ;
	bool write_constant(const float v) ;

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

};

/**
 * @brief The VertexConfiguration class Is a set of attributes resembling a
 * vertex.
 */
class OFL_DLL_PUBLIC VertexConfiguration
{
protected:
	std::vector<Attribute> m_attributes;
	uint32_t m_size;

	uint32_t OFL_DLL_LOCAL m_minimum_size()
	{
		uint32_t r_size = 0;
		for(auto& a : m_attributes)
		{
			if(!a.use_constant)
			{
				a.offset = r_size;
				r_size+= a.size();
			}
			else
				a.offset = 0;
		}
		return r_size;
	}
public:
	VertexConfiguration()
		:m_size (0)
	{

	}
	uint32_t vertex_size()const{return m_size;}



	void wrap_up()
	{
		vertex_size();
	}

	bool has_attribute(const AttributeID id) const
	{

	}


};



class OFL_DLL_PUBLIC Vertex
{
	bool m_destroy_data;
	void* m_data;
public:
	friend class VertexData;
	VertexConfiguration cfg;

	~Vertex()
	{
		if(m_destroy_data && m_data)
			free(m_data);
	}
	Vertex(const VertexConfiguration&c,void* b);
	Vertex(const Vertex& o);

	void set_data(ubyte* d);
	void load_data(ubyte* d)
	{
		if(m_data && !m_destroy_data)
			m_data = static_cast<ubyte*>(malloc(cfg.vertex_size()));
		memcpy(m_data,d,cfg.vertex_size());
	}

	void* data(){return  m_data;}
	const void* data()const{return  m_data;}

	bool get_value(const AttributeID id, vec4& v)const ;
	bool get_value(const AttributeID id, vec3& v) const;
	bool get_value(const AttributeID id, vec2& v) const;
	bool get_value(const AttributeID id, float& v) const;

	bool set_value(const AttributeID id,const vec4& v) const;
	bool set_value(const AttributeID id,const vec3& v) const;
	bool set_value(const AttributeID id,const vec2& v) const;
	bool set_value(const AttributeID id,const float v) const;

	void* get_value_ptr(const AttributeID id);

	bool has_attribute(const AttributeID id) const;

	bool is_equal(const Vertex& o)const;

	Vertex& operator =(const Vertex& o)
	{
		cfg = o.cfg;
		if(m_data)
		{
			if(m_destroy_data)
				free(m_data);
		}

		m_data = malloc(cfg.vertex_size());
		memcpy(m_data,o.data(),cfg.vertex_size());
		m_destroy_data = true;
		return *this;
	}

	bool operator == (const Vertex& o)const;
	bool operator != (const Vertex& o)const;
	bool operator < (const Vertex& o)	const;

	std::string value_to_string() const
	{
		std::string res;

		int i = 0 ;
		for(const auto& a : cfg)
		{
			res+=(i?" ":"")+a.value_to_string(m_data);
			i++;
		}
		return res;
	}
};



/**
 * @brief The VertexData class represents vertex data in a renderable form.
 */
class OFL_DLL_PUBLIC VertexData
{
private:

	Primitive m_render_primitive;
	Type m_index_type;

	uint m_index_count;
	uint m_index_reserve;
	void* m_index_data;

	VertexConfiguration m_cfg;
	void* m_vertex_data;
	uint m_vertex_count;
	uint m_vertex_reserve;

public:
	friend class VertexDataOPS;

	/**
	 * @brief The VertexIterator class an iterator used to iterate over all
	 * vertices.
	 */
	class VertexIterator
	{
	protected:
		Vertex v;
	public:
		VertexIterator(VertexConfiguration cfg, void* b)
			:v(cfg,b){}
		VertexIterator& operator++()
		{
			v.m_data = static_cast<ubyte*>(v.m_data)+v.cfg.vertex_size();
			return *this;
		}
		bool operator!=(const VertexIterator & other)
		{
			return v.m_data != other.v.m_data;
		}
		const Vertex& operator*() const { return v; }
		Vertex& operator*() { return v; }
	};

	/**
	 * @brief VertexData Constructor
	 * @param primitive The primitive type
	 * @param cfg The vertex configuratio
	 * @param index_type the index type
	 * @param res_vtx number of vertices you wish to reserve for
	 * @param res_idx number of indices you wish to reserve for
	 */
	VertexData(Primitive primitive,
			   VertexConfiguration cfg,
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
	const VertexConfiguration& vertex_configuration() const {return m_cfg;}
	/**
	 * @brief vertex_count
	 * @return number of vertices stored.
	 */
	uint vertex_count()const{return m_vertex_count;}
	/**
	 * @brief vertices_reserve will allocate memory to holf at least c vertices
	 * @param c
	 */
	void vertices_reserve(const uint c);

	/**
	 * @brief verties_null will set the whole reserved vertex space to 0!
	 */
	void verties_null()
	{
		memset(m_vertex_data,0,m_vertex_reserve*m_cfg.vertex_size());
	}

	/**
	 * @brief push_back adds a Vertex to the vertex list.
	 * @param v The new Vertex
	 * @return the index of the newly added vertex.
	 */
	uint push_back(const Vertex& v);

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

	VertexIterator begin() const
	{return VertexIterator(m_cfg,m_vertex_data);}
	VertexIterator end() const
	{return VertexIterator(m_cfg,static_cast<ubyte*>(m_vertex_data)+m_vertex_count*m_cfg.vertex_size());}

	/**
	 * @brief get_vertex returns a specific index.
	 * Note this will return a __COPY__ of this index.
	 * @param id the index you want to access.
	 * @return A copy of this index.
	 */
	Vertex get_vertex(const uint id) const
	{
		ubyte* dat = static_cast<ubyte*>(m_vertex_data) + id*m_cfg.vertex_size();
		auto v = Vertex(m_cfg,nullptr);
		v.load_data(dat);
		return v;
	}
	/**
	 * @brief get_all_attributes converts all vertices attributes of one given id
	 * into vec4 and returns them as a std::vector. Note this is not fast!
	 * @param attrib_id the attribute id to read
	 * @return returns an array of this attributes values.
	 */
	std::vector<vec4> get_all_attributes(const AttributeID attrib_id) const
	{
		std::vector<vec4> res;
		if(!m_cfg.has_attribute(attrib_id))
			return res;
		auto a = m_cfg.get_attribute_by_id(attrib_id);
		vec4 r;
		res.reserve(m_vertex_count);
		for(uint32_t i = 0 ; i< m_vertex_count*m_cfg.vertex_size();i+=m_cfg.vertex_size())
		{
			a.read(static_cast<ubyte*>(m_vertex_data) + i,r);
			res.push_back(r);
		}
		return res;
	}

	std::vector<uint32_t> get_all_indices() const
	{
		std::vector<uint32_t> res;
		res.reserve(m_index_count);
		for(uint32_t i = 0 ; i<m_index_count;i++)
		{
			res.push_back(get_index(i));
		}
		return res;
	}

};

#ifndef RESET_PRIMITIVE
#define RESET_PRIMITIVE ~0
#endif



class OFL_DLL_PUBLIC VertexDataOPS
{
protected:
public:

	static VertexData* read_vd(std::ifstream& f);
	static VertexData* read_obj(std::ifstream& f);
	static VertexData* read_ply(std::ifstream& f);
	static VertexData* read_off(std::ifstream& f);

	static VertexData* read_vd(const void* mem);
	static VertexData* read_obj(const void* mem);
	static VertexData* read_ply(const void* mem);
	static VertexData* read_off(const void* mem);


	static bool write_vd(const VertexData* vd, std::ofstream& f);
	static bool write_obj(const VertexData* vd, std::ofstream& f);
	static bool write_ply(const VertexData* vd, std::ofstream& f);
	static bool write_off(const VertexData* vd, std::ofstream& f);

	static bool write_vd( const VertexData* vd, void** mem);
	static bool write_obj(const VertexData* vd, void** mem);
	static bool write_ply(const VertexData* vd, void** mem);
	static bool write_off(const VertexData* vd, void** mem);

	enum FileFormat
	{
		OBJ,
		PLY,
		VD,
		OFF,
		FROM_PATH
	};
	/**
	 * @brief writeToFile writes the given vertexdaa into a file.
	 * @param vd Vertex data to write from
	 * @param path Path to write to
	 * @param f Format of the outpu file. if FROM_PATH is used, the format will
	 * be determined from the file ending
	 * @return true if everything went well, false if there was a problem.
	 */
	static bool write_to_file(const VertexData* vd,const std::string& path,
							  FileFormat f=FROM_PATH);

	/**
	 * @brief readFromFile reads VertexData from a file
	 * @param path Path to the source file.
	 * @param f The format of the source file
	 * @return the VertexData read, or a nullptr, if something went wrong.
	 */
	static VertexData* read_from_file(const std::string& path,
									  FileFormat f = FROM_PATH);

	/**
	 * @brief recalculate_normals will calculate new normal for the given vd.
	 * Note if the given attribute, that shall store the normals does not exist
	 * the function will return false, and no normals are calculated.
	 * @param vd VertexData to work on.
	 * @param to_attribute The attribute to store the resulting normals
	 * @return true if new normals were calculated and stored.
	 */
	static bool recalculate_normals(VertexData* vd,
									AttributeID to_attribute=ATTRIB_NORMAL);

	/**
	 * @brief recalculate_tangents will calculate new tangents for the given vd.
	 * Note if the given attribute, that shall store the tangent does not exist
	 * the function will return false, and no tangents are calculated.
	 * @param vd VertexData to work on.
	 * @param to_attribute The attribute to store the resulting tangents.
	 * @return true if new tangents were calculated and stored.
	 */
	static bool recalculate_tangents(VertexData* vd,
									 AttributeID to_attribute=ATTRIB_TANGENT);

	/**
	 * @brief reconfigure canges the VertexConfiguration for a given VertexData.
	 * The conversion of data types is done via floats. attributes with no
	 * counterpart in the new configuration will be lost.
	 * New attributes will be uninizialized memory. If null is set new
	 * attributes will be set to zero.
	 * @param data The input data.
	 * @param new_config The new configuration.
	 * @param null True if you wish to initilize new vertex data with 0
	 * @return The new reconfigured vertex data. you have to delete it!!!!!!
	 */
	static VertexData* reconfigure(VertexData* data,
								   const VertexConfiguration& new_config,
								   bool null=false);
};

}
