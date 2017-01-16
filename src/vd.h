#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <cstdint>

template<typename T>
inline typename std::enable_if<std::is_floating_point<T>::value || std::is_class<T>::value, T>::type
float_to_int(float f)
{
	static_assert(true, "Conversion only to integer types!");
}


template<typename T>
inline typename std::enable_if<std::is_unsigned<T>::value, T>::type
float_to_int(const float f)
{
	return static_cast<T>(f*std::numeric_limits<T>::max()/*((1<<((sizeof(T)*8)))-1)*/);
}


template<typename T>
inline typename std::enable_if<std::is_signed<T>::value, T>::type
float_to_int(const float f)
{
	return static_cast<T>(f*((1<<((sizeof(T)*8)-1))-1));
}


template<typename T>
inline typename std::enable_if<std::is_unsigned<T>::value>::type
float_to_int(const float f, T& t)
{
	t = static_cast<T>(f*((1<<((sizeof(T)*8)))-1));
}


template<typename T>
inline typename std::enable_if<std::is_signed<T>::value>::type
float_to_int(const float f, T& t)
{
	t = static_cast<T>(f*((1<<((sizeof(T)*8)-1))-1));
}



template<typename T>
inline typename std::enable_if<std::is_signed<T>::value, float>::type
int_to_float(const T x)
{
	const T t = ((1<<((sizeof(T)*8)-1))-1);
	return std::max(static_cast<float>(x)/t,-1.0f);
}


template<typename T>
inline typename std::enable_if<std::is_unsigned<T>::value, float>::type
int_to_float(const T x)
{
	const T t = std::numeric_limits<T>::max();//((1<<((sizeof(T)*8)))-1);
	return static_cast<float>(x)/t;
}


typedef glm::vec2 vec2;
typedef  glm::vec3 vec3;
typedef  glm::vec4 vec4;
typedef  uint32_t uint;
typedef  uint8_t ubyte;
typedef  int8_t byte;

enum Primitive
{
	POINTS = 0x0000,			// == GL_POINTS
	LINES = 0x0001,				// == GL_LINES
	//LINE_LOOP = 0x0002,		// == GL_LINE_LOOP
	LINE_STRIP = 0x0003,		// == GL_LINE_STRIP
	TRIANGLES = 0x0004,			// == GL_TRIANGLES
	TRIANGLE_STRIP = 0x0005,	// == GL_TRIANGLE_STRIP
	//TRIANGLE_FAN = 0x0006,	// == GL_TRIANGLE_FAN
	QUADS = 0x0007,				// == GL_QUADS
	QUAD_STRIP = 0x0008,		// == GL_QUAD_STRIP
	//POLYGON = 0x0009,			// == GL_POLYGON
};

enum class AttributeID : uint8_t
{
	POSITION =0,
	NORMAL,
	TEXCOORD,
	TANGENT,
	COLOR,
	UDEF_0,
	UDEF_1,
	UDEF_2,
	UDEF_3,
	UDEF_4,
	UDEF_5,
	UDEF_6,
	UDEF_7,
	UDEF_8,
	UDEF_9,
	UDEF_10
};

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

class Type
{
public:
 TypeID id;

Type(const unsigned int id)
{
	this->id = static_cast<TypeID>(id);
}

Type(const TypeID id = TypeID::INVALID)
{
	this->id = id;
}

operator TypeID() const
{
	return  id;
}

operator unsigned int() const
{
	return  static_cast<unsigned int>(id);
}
bool is_integer() const
{
	return !(id == TypeID::DOUBLE || id == TypeID::FLOAT);
}

bool is_float() const
{
	return (id == TypeID::DOUBLE || id == TypeID::FLOAT);
}

bool is_signed() const
{
	return (id == TypeID::UNSIGNED_BYTE || id == TypeID::UNSIGNED_SHORT|| id == TypeID::UNSIGNED_INT);
}

bool operator ==(const Type& o)const
{
	return  o.id == id;
}

bool operator ==(const TypeID id)const
{
	return  this->id == id;
}

bool operator !=(const TypeID id)const
{
	return  this->id != id;
}

bool operator !=(const Type& o) const
{
	return  o.id != id;
}

size_t size() const
{
	switch (id)
	{
	case TypeID::BYTE:
	case TypeID::UNSIGNED_BYTE:
		return  1;
	case TypeID::SHORT:
	case TypeID::UNSIGNED_SHORT:
		return  2;
	case TypeID::INT:
	case TypeID::UNSIGNED_INT:
	case TypeID::FLOAT:
		return  4;
	case TypeID::DOUBLE:
		return  8;
	default: static_assert(true,"unknown type!");
	}
	return  0;
}
};

/*
inline size_t type_size(const TypeID type)
{
	switch (type)
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
	default: static_assert(true,"unknown type!");
	}
	return  0;
}*/


class Attribute
{
public:
	AttributeID attribute_id;
	uint8_t elements;
	Type type;

	bool normalized;
	bool use_constant;
	uint offset;

	ubyte constant[4*sizeof(uint64_t)];

	uint size() const;

	Attribute(const Attribute& atr)
	{
		attribute_id = atr.attribute_id;
		elements = atr.elements;
		type = atr.type;
		normalized = atr.normalized;
		use_constant = atr.use_constant;
		offset = atr.offset;
		memcpy(constant,atr.constant,4*sizeof(uint64_t));


	}

	Attribute(const AttributeID id,
			  const uint elements,
			  const Type type,
			  const bool normalized,
			  bool use_constant = false,
			  const void* constant = nullptr);

	bool read(const void* zero, vec4& res) const;
	bool read(const void* zero, vec3& res) const;
	bool read(const void* zero, vec2& res) const;
	bool read(const void* zero, float& res) const;

	bool write(void* zero,const vec4& v) const;
	bool write(void* zero,const vec3& v) const;
	bool write(void* zero,const vec2& v) const;
	bool write(void* zero,const float v) const;

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

class VertexConfiguration
{
protected:
	std::vector<Attribute> m_attributes;
	std::map<AttributeID, uint32_t> m_attrib_from_id;
	uint m_size;

	uint m_minimum_size()
	{
		uint r_size = 0;
		for(auto& a : m_attributes)
		{
			a.offset = r_size;
			r_size+= a.size();
		}
		return r_size;
	}
public:
	~VertexConfiguration()
	{

	}

	VertexConfiguration()
		:m_size (0)
	{

	}
	uint vertex_size()const{return m_size;}

	const Attribute& get_attribute_by_id(AttributeID id) const
	{
		auto aa =*m_attrib_from_id.find(id);

		return m_attributes[aa.second];
	}

	const Attribute& get_attribute(const uint i) const
	{

		return m_attributes[i];
	}

	uint32_t attribute_count() const{return m_attributes.size();}

	void add_attribute(const Attribute& a)
	{
		m_attributes.push_back(a);
		m_attrib_from_id[a.attribute_id] = (m_attributes.size()-1);
		m_size = m_minimum_size();
	}

	void wrap_up()
	{
		vertex_size();
	}

	bool has_attribute(const AttributeID id) const
	{
		return m_attrib_from_id.find(id) != m_attrib_from_id.end();
	}



	bool operator == (const VertexConfiguration& o)const
	{
		if(m_attributes.size() != o.m_attributes.size())
			return false;
		for(uint i = 0; i< m_attributes.size();i++)
		{
			if(m_attributes[i] != o.m_attributes[i])
				return false;
		}
		return true;
	}

	bool operator != (const VertexConfiguration& o)const
	{
		return  !(*this==o);
	}

};


class Vertex
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
	Vertex(const Vertex& o):m_data(nullptr),cfg(o.cfg)
	{
		if(o.m_data)
		{
			m_data = static_cast<ubyte*>(malloc(cfg.vertex_size()));
			memcpy(m_data,o.m_data,cfg.vertex_size());
			m_destroy_data = true;
		}
	}

	void set_data(ubyte* d)
	{
		if(m_data && m_destroy_data)
			free(m_data);
		m_data = d;
	}
	void load_data(ubyte* d)
	{
		if(m_data && !m_destroy_data)
			m_data = static_cast<ubyte*>(malloc(cfg.vertex_size()));
		memcpy(m_data,d,cfg.vertex_size());
	}

	void* data(){return  m_data;}
	const void* data()const{return  m_data;}

	bool getAttribute(const AttributeID id, vec4& v)const ;
	bool getAttribute(const AttributeID id, vec3& v) const;
	bool getAttribute(const AttributeID id, vec2& v) const;
	bool getAttribute(const AttributeID id, float& v) const;

	bool setAttribute(const AttributeID id,const vec4& v) const;
	bool setAttribute(const AttributeID id,const vec3& v) const;
	bool setAttribute(const AttributeID id,const vec2& v) const;
	bool setAttribute(const AttributeID id,const float v) const;

	void* get_attribute_ptr(const AttributeID id)
	{
		if(cfg.has_attribute(id))
			return static_cast<ubyte*>(m_data)+cfg.get_attribute_by_id(id).offset;
		else
			return nullptr;
	}

	bool has_attribute(const AttributeID id) const
	{
		return cfg.has_attribute(id);
	}


	bool is_equal(const Vertex& o)const
	{
		for(uint i = 0 ; i< cfg.attribute_count();i++)
		{
			const Attribute& a = cfg.get_attribute(i);
			if(o.has_attribute(a.attribute_id))
			{
				vec4 me, you;
				getAttribute(a.attribute_id,me);
				o.getAttribute(a.attribute_id,you);

				if(me!=you)
					return false;
			}
		}
	}

	bool operator == (const Vertex& o)const
	{
		if(cfg != o.cfg)
			return false;
		return memcmp(m_data,o.m_data,cfg.vertex_size());
	}

	bool operator != (const Vertex& o)const
	{
		return  !(*this==o);
	}

	bool operator < (const Vertex& o)	const
	{
		const ubyte* a = static_cast<ubyte*>(m_data);
		const ubyte* b = static_cast<ubyte*>(o.m_data);
		for(uint i =0; i <cfg.vertex_size();i++)
		{
			if(a[i]<b[i])
				return true;
			if(a[i]>b[i])
				return false;
		}
		return false;
	}
};



/**
 * @brief The VertexData class represents vertex data in a renderable form.
 */
class VertexData
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
	friend class VertexDataTools;
	class VertexIterator
	{
	protected:
		Vertex v;
	public:
		VertexIterator(VertexConfiguration cfg, void* b)
			:v(cfg,b){}
		VertexIterator& operator++(){v.m_data = static_cast<ubyte*>(v.m_data)+v.cfg.vertex_size();return *this;}
		bool operator!=(const VertexIterator & other){return v.m_data != other.v.m_data;}
		const Vertex& operator*() const { return v; }
		Vertex& operator*() { return v; }
	};


	VertexData(Primitive primitive,
			   VertexConfiguration cfg,
			   const uint res_vtx = 0,
			   const Type index_type = Type(UNSIGNED_SHORT),
			   const uint res_idx = 0);
	virtual ~VertexData()
	{
		if(m_vertex_data)
			free(m_vertex_data);
		if(m_index_data)
			free(m_index_data);
	}

	const VertexConfiguration& vertex_configuration() const {return m_cfg;}
	uint vertex_count()const{return m_vertex_count;}
	void vertices_reserve(const uint c);
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




	Type index_type()const {return  m_index_type;}
	uint index_count()const{return m_index_count;}
	void indices_reserve(const uint c);
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
	 * @brief data gives write access to the Vertex list
	 * @return The vertex list.
	 */
	void* vertex_data(){return  m_vertex_data;}
	const void* vertex_data()const{return  m_vertex_data;}

	void* indices(){return m_index_data;}
	const void* indices()const{return m_index_data;}




	uint32_t get_index(const uint32_t i) const
	{
		assert(m_index_type.is_integer() && !m_index_type.is_signed());

		switch (m_index_type.id)
		{
		case UNSIGNED_BYTE: return static_cast<uint8_t* >(m_index_data)[i];
		case UNSIGNED_SHORT:return static_cast<uint16_t*>(m_index_data)[i];
		case UNSIGNED_INT: return  static_cast<uint32_t*>(m_index_data)[i];
		default: return 0;
		}
		return  0;
	}


	void set_index(const uint32_t i, const uint32_t v)
	{
		assert(m_index_type.is_integer() && !m_index_type.is_signed());

		switch (m_index_type.id)
		{
		case UNSIGNED_BYTE:  static_cast<uint8_t* >(m_index_data)[i]= v;break;
		case UNSIGNED_SHORT: static_cast<uint16_t*>(m_index_data)[i] = v;break;
		case UNSIGNED_INT:  static_cast<uint32_t*>(m_index_data)[i] = v;break;
		default: break;
		}
	}


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

	Vertex get_vertex(const uint id) const
	{
		ubyte* dat = static_cast<ubyte*>(m_vertex_data) + id*m_cfg.vertex_size();
		auto v = Vertex(m_cfg,nullptr);
		v.load_data(dat);
		return v;
	}

};


class VertexDataTools
{
protected:
public:
	static VertexData* readVD(FILE* f);
	static VertexData* readOBJ(FILE* f);
	static VertexData* readPLY(FILE* f);
	static VertexData* readOFF(FILE* f);

	static VertexData* readVD(const void* mem);
	static VertexData* readOBJ(const void* mem);
	static VertexData* readPLY(const void* mem);
	static VertexData* readOFF(const void* mem);

	static bool writeVD(const VertexData* vd, FILE* f);
	static bool writeOBJ(const VertexData* vd, FILE* f);
	static bool writePLY(const VertexData* vd, FILE* f);
	static bool writeOFF(const VertexData* vd, FILE* f);

	static bool writeVD( const VertexData* vd, void** mem);
	static bool writeOBJ(const VertexData* vd, void** mem);
	static bool writePLY(const VertexData* vd, void** mem);
	static bool writeOFF(const VertexData* vd, void** mem);


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
	static bool writeToFile(const VertexData* vd,const std::string& path, FileFormat f=FROM_PATH);

	/**
	 * @brief readFromFile reads VertexData from a file
	 * @param path Path to the source file.
	 * @param f The format of the source file
	 * @return the VertexData read, or a nullptr, if something went wrong.
	 */
	static VertexData* readFromFile(const std::string& path, FileFormat f = FROM_PATH);

	static bool recalculate_normals(VertexData* vd, AttributeID to_attribute=AttributeID::NORMAL);
	static bool recalculate_tangents(VertexData* vd,AttributeID to_attribute=AttributeID::TANGENT);

	static VertexData* reconfigure(VertexData* data, const VertexConfiguration& new_config, bool null=false);

};

