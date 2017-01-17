#pragma once
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <iostream>
#include <ostream>
#include <istream>
#include <cstring>
#include <cmath>
#include <glm/glm.hpp>



typedef glm::vec2 vec2;
typedef  glm::vec3 vec3;
typedef  glm::vec4 vec4;
typedef  uint32_t uint;
typedef  uint8_t ubyte;
typedef  int8_t byte;


/**
 * The type toolset is used to manage data types and their properties.
 */
namespace typ {
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

	Type(const TypeID id = INVALID):id(id){}
	Type(const unsigned int id)	:id(static_cast<TypeID>(id)){}
	Type(const int id)	:id(static_cast<TypeID>(id)){}
	Type(const std::string& str);

	operator TypeID() const {return  id;}
	operator unsigned int() const {	return  static_cast<unsigned int>(id);}
	operator int() const {	return  static_cast<int>(id);}

	/**
	 * @brief to_string converts this type to a string.
	 * @return string representing this type for example: "int", "ushort" etc.
	 */
	std::string to_string();


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
}




namespace stru
{

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



/**
 * @brief The Tokenizer class is a tool to parse strings. It works linke strtok,
 * but it has a local state.
 * It works by a simple principle. There is a head pointer. Everytime you ask
 * for a token. the current head will be returned, and the first appearing
 * seperator character is searched and set to zero.
 * NOTE: Only _ONE_ character will be set to zero. Even if there are three
 * seperators in a row, only the first one will be set to zero.
 */
class Tokenizer
{
protected:
	char* m_base;
	char* m_rest;
	bool m_delete_base;
public:
	static std::string whitespaces;
	Tokenizer(const std::string& base);
	Tokenizer(char* base);
	~Tokenizer();

	void set_base(char* base)
	{
		if(m_delete_base)
			free(base);
		m_base = base;
		m_rest = base;
		m_delete_base = false;
	}
	/**
	 * @brief reset Will free the current base and set a new one.
	 * @param base The new base.
	 */
	void reset(const std::string& base);

	/**
	 * @brief getToken Will return the string until and without the seperator
	 * character:
	 * For example:
	 * "foo,bar..." ---getToken(',')--> "foo"
	 *
	 * @param separator The seperator character
	 * @return string untill the first appearence of seperator, or nullptr.
	 */
	char* get_token(char separator);

	/**
	 * @brief Will return the string till and without one! of the seperators!
	 * "foo;,.bar..." ---getToken(",.;",c)--> "foo",c=;
	 * @param separators String contianing all possible seperatos.
	 * @param sep Will contain the seperator actually found.
	 * @return string till the first appearence of a seperator, or nullptr.
	 */
	char* get_token(const std::string& separators = whitespaces,
				   char* sep = nullptr);


	/**
	 * @brief getTokenAs Will read a string untill one of the seperators appear
	 * and return it interpreted.
	 * @param separators String contianing all possible seperatos.
	 * @param sep Will contain the seperator actually found.
	 * @return
	 */
	template<typename T>
	bool get_token_as(T& res,const std::string &separators = whitespaces,
					char *sep = nullptr)
	{
		throw "TYPE NOT SUPPORTED!!!";
		return false;
	}

	/**
	 * @brief skipOverAll Skipps all consecutive appearences of char in seps.
	 * Example:
	 * ".,;..:,;,,.foo...." ---skipOverAll(";.,:")--> "foo...."
	 * @param separators String contianing all possible seperatos.
	 */
	void skip_over_all(const std::string& seps);

	/**
	 * @brief skipOverAll Skipps all consecutive appearences of whitespaces.
	 * Example:
	 * "           foo...." ---skipWhiteSpaces()--> "foo...."
	 */
	void skip_white_spaces();

	/**
	 * @brief getRest The remaining string,
	 * @return
	 */
	char* get_rest(){ return m_rest; }


	/**
	 * @brief readEscString munches off an escaped string.
	 * >"hello \"World\" !"< -> >hello "World" !<
	 * @return
	 */
	bool read_esc_string(std::string& res)
	{
		std::string r;
		while(*m_rest != '"')
			m_rest++;
		if(!*m_rest)
			return false;
		while(*m_rest!= '"' && *m_rest)
		{
			if(*m_rest == '\\')
					m_rest++;
			r+=*m_rest;
			m_rest++;
		}

		if(!*m_rest)
			return false;

		res = r;
		return true;
	}

	/**
	 * @brief getTokenTillClosing Returns the inlay of an area marked by opening and closing chars:
	 * "  { 1,2,{3}}" --getTokenTillClosing('{','}')--> " 1,2,{3}"
	 * @param opening
	 * @param closing
	 * @return
	 */
	char* get_token_till_closing(char opening, char closing)
	{

		while(*m_rest && *m_rest!=opening)
		{
			m_rest++;
		}
		m_rest++;
		char* res = m_rest;
		if(!m_rest)
			return nullptr;


		int count = 1;
		while(count > 0)
		{
			m_rest++;
			if(!*m_rest)
				return nullptr;
			if(*m_rest == opening)
				count++;
			if(*m_rest == closing)
				count--;
		}

		if(!*m_rest)
			return nullptr;
		*m_rest = 0;
		m_rest++;
		return res;
	}
};


template<> inline bool Tokenizer::get_token_as<int>(
		int& res,
		const std::string &seps,
		char *sep )
{
	char* c = get_token(seps,sep);
	if(c)
		res = atoi(c);
	return c;
}

template<> inline bool Tokenizer::get_token_as<uint>(
		uint& res,
		const std::string &seps,
		char *sep )
{
	char* c = get_token(seps,sep);
	if(c)
		res = static_cast<uint>(atoi(c));
	return c;
}

template<> inline bool Tokenizer::get_token_as<float>(
		float& res,
		const std::string &seps ,
		char *sep )
{
	char* c = get_token(seps,sep);
	if(c)
		res = atof(c);
	return c;
}


template<> inline bool Tokenizer::get_token_as<double>(
		double& res,
		const std::string &seps ,
		char *sep )
{
	char* c = get_token(seps,sep);
	if(c)
		res = atof(c);
	return c;
}

template<> inline bool Tokenizer::get_token_as<bool>(
		bool& res,
		const std::string &seps ,
		char *sep )
{
	char* c=  get_token(seps,sep);
	if(c)
		res = strcmp(c,"false");
	return c;
}


template<> inline bool Tokenizer::get_token_as<glm::vec4>(
		glm::vec4& res,
		const std::string &seps,
		char *sep )
{
	// should look like "(x,y,z,w)"
	char* c = get_token(seps,sep);
	if(!c)
		return  false;

	Tokenizer t(c);

	t.get_token('(');
	bool r =  t.get_token_as(res.x,",") && t.get_token_as(res.y,",") && t.get_token_as(res.z,",") && t.get_token_as(res.w,")");
	t.set_base(nullptr);
	return r;
}


template<> inline bool Tokenizer::get_token_as<glm::mat4>(
		glm::mat4& res,
		const std::string &seps,
		char *sep )
{
	// should look like "((x,y,z,w),(x,y,z,w),(x,y,z,w),(x,y,z,w))"
	char* c = get_token(seps,sep);
	if(!c)
		return  false;

	Tokenizer t(c);

	t.get_token('(');
	bool r =  t.get_token_as(res[0],",") && t.get_token_as(res[1],",") && t.get_token_as(res[2],",") && t.get_token_as(res[3],")");
	t.set_base(nullptr);
	return r;
}





namespace paths
{


static inline bool is_directory(const std::string& p)
{
	return p.at(p.length()-1) == '/';
}
static inline bool is_relative(const std::string& p)
{
	if(p.length()<2)
		return false;
	return p.at(0) != '/' && p.at(1) != ':';
}

static inline std::string file(const std::string& p)
{
	if(is_directory(p))
		return "";

	return p.substr(p.find_last_of('/')+1);
}

static inline std::string filename(const std::string& p)
{
	if(is_directory(p))
		return "";
	auto locd = p.find_last_of('.');
	auto locs =  p.find_last_of('/');
	if(locs == p.npos)
		locs = 0;
	else
		locs++;
	if(locd < locs)
		locd = p.npos;

	return p.substr(locs,locd-locs);
}

static inline std::string extension(const std::string& p)
{
	if(is_directory(p))
		return "";
	auto loc = p.find_last_of('.');
	auto sloc = loc > p.find_last_of('/');
	if(loc > sloc || sloc == p.npos)
		return p.substr(loc+1);
	return "";
}

static inline std::string without_extension(const std::string& p)
{
	if(is_directory(p))
		return p;
	auto loc = p.find_last_of('.');
	return p.substr(0,loc);
}

}
}

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

enum AttributeID
{
	ATTRIB_POSITION =0,
	ATTRIB_NORMAL,
	ATTRIB_TEXCOORD,
	ATTRIB_TANGENT,
	ATTRIB_COLOR,
	ATTRIB_COUNT
};


/**
 * @brief The Attribute class represents the configuration of a value stored for
 * each vertex. This is designed to make it easy to set an Vertex attribute
 * pointer in opengl.
 */
class Attribute
{
public:
	AttributeID attribute_id;
	uint32_t elements;
	typ::Type type;
	uint32_t offset;
	bool normalized;
	bool use_constant;
	ubyte constant[4*sizeof(uint64_t)];

	short _padding[1];

	uint32_t size() const;

	Attribute()
	{
		attribute_id = ATTRIB_POSITION;
		elements = 0;
		type = typ::FLOAT;
		offset = 0;
		normalized = false;
		use_constant = false;
		memset(constant,0,4*sizeof(uint64_t));
	}

	Attribute(const AttributeID id,
			  const uint elements,
			  const typ::Type type,
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

	Attribute& get_attribute(const uint i)
	{
		return m_attributes[i];
	}

	uint32_t attribute_count() const
	{
		return static_cast<uint32_t>(m_attributes.size());
	}

	void add_attribute(const Attribute& a)
	{
		m_attributes.push_back(a);
		m_attrib_from_id[a.attribute_id] =	static_cast<uint32_t>(m_attributes.size()-1);
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

	bool operator == (const Vertex& o)const;
	bool operator != (const Vertex& o)const;
	bool operator < (const Vertex& o)	const;
};



/**
 * @brief The VertexData class represents vertex data in a renderable form.
 */
class VertexData
{
private:

	Primitive m_render_primitive;
	typ::Type m_index_type;

	uint m_index_count;
	uint m_index_reserve;
	void* m_index_data;

	VertexConfiguration m_cfg;
	void* m_vertex_data;
	uint m_vertex_count;
	uint m_vertex_reserve;

public:
	friend class VDOps;

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
			   const typ::Type index_type = typ::Type(typ::UNSIGNED_SHORT),
			   const uint res_vtx = 0,			   
			   const uint res_idx = 0);

	~VertexData()
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
	typ::Type index_type()const {return  m_index_type;}

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

};


class VDOps
{
protected:
public:

	static VertexData* read_vd(FILE* f);
	static VertexData* read_obj(FILE* f);
	static VertexData* read_ply(FILE* f);
	static VertexData* read_off(FILE* f);

	static VertexData* read_vd(const void* mem);
	static VertexData* read_obj(const void* mem);
	static VertexData* read_ply(const void* mem);
	static VertexData* read_off(const void* mem);


	static bool write_vd(const VertexData* vd, FILE* f);
	static bool write_obj(const VertexData* vd, FILE* f);
	static bool write_ply(const VertexData* vd, FILE* f);
	static bool write_off(const VertexData* vd, FILE* f);

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
