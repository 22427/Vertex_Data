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


#include "types.h"




typedef glm::vec2 vec2;
typedef  glm::vec3 vec3;
typedef  glm::vec4 vec4;
typedef  uint32_t uint;
typedef  uint8_t ubyte;
typedef  int8_t byte;


namespace typ {


template<typename T>
inline typename std::enable_if<std::is_floating_point<T>::value || std::is_class<T>::value, T>::type
float_to_int(float /*f*/)
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

	Type(const std::string& str)
	{
		if(str == "float" || str == "FLOAT"|| str == "single")
			id = FLOAT;
		else if(str == "double" || str == "DOUBLE")
			id = DOUBLE;
		else if(str == "int" || str == "INT")
			id = INT;
		else if(str == "uint" || str == "UNSIGNED_INT" || str == "unsigned int" || str == "UNSIGNED INT")
			id = UNSIGNED_INT;

		else if(str == "short" || str == "SHORT")
			id = SHORT;
		else if(str == "ushort" || str == "UNSIGNED_SHORT" || str == "unsigned short" || str == "UNSIGNED SHORT")
			id = UNSIGNED_SHORT;

		else if(str == "char" || str == "byte"|| str == "BYTE")
			id = BYTE;
		else
			id = UNSIGNED_BYTE;

	}

	operator TypeID() const
	{
		return  id;
	}

	std::string to_string()
	{
		switch (id)
		{
		case TypeID::BYTE: return  "char";
		case TypeID::UNSIGNED_BYTE: return  "uchar";
		case TypeID::SHORT: return  "short";
		case TypeID::UNSIGNED_SHORT: return  "ushort";
		case TypeID::INT: return  "int";
		case TypeID::UNSIGNED_INT: return  "uint";
		case TypeID::FLOAT: return  "float";
		case TypeID::DOUBLE: return  "double";
		case INVALID: return "INVALID";
		}
		return  "";
	}


	operator uint32_t() const
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

	bool is_unsigned() const
	{
		return (id == TypeID::UNSIGNED_BYTE || id == TypeID::UNSIGNED_SHORT|| id == TypeID::UNSIGNED_INT);
	}

	bool is_signed() const
	{
		return !(id == TypeID::UNSIGNED_BYTE || id == TypeID::UNSIGNED_SHORT|| id == TypeID::UNSIGNED_INT);
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
		case INVALID: static_assert(true,"unknown type!");
		}
		return  0;
	}



	double max() const
	{
		switch (id)
		{
		case TypeID::BYTE: return std::numeric_limits<int8_t>::max();
		case TypeID::UNSIGNED_BYTE: return std::numeric_limits<uint8_t>::max();
		case TypeID::SHORT: return std::numeric_limits<int16_t>::max();
		case TypeID::UNSIGNED_SHORT: return std::numeric_limits<uint16_t>::max();
		case TypeID::INT: return std::numeric_limits<int32_t>::max();
		case TypeID::UNSIGNED_INT: return std::numeric_limits<uint32_t>::max();
		case TypeID::FLOAT: return static_cast<double>(std::numeric_limits<float>::max());
		case TypeID::DOUBLE: return std::numeric_limits<double>::max();
		case INVALID: static_assert(true,"unknown type!");
		}
		return  0;
	}
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
public:
	static std::string whitespaces;
	Tokenizer(const std::string& base);
	Tokenizer(char* base);
	~Tokenizer();

	void setBase(char* base)
	{
		m_base = base;
		m_rest = base;
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
	char* getToken(char separator);

	/**
	 * @brief Will return the string till and without one! of the seperators!
	 * "foo;,.bar..." ---getToken(",.;",c)--> "foo",c=;
	 * @param separators String contianing all possible seperatos.
	 * @param sep Will contain the seperator actually found.
	 * @return string till the first appearence of a seperator, or nullptr.
	 */
	char* getToken(const std::string& separators = whitespaces,
				   char* sep = nullptr);


	/**
	 * @brief getTokenAs Will read a string untill one of the seperators appear
	 * and return it interpreted.
	 * @param separators String contianing all possible seperatos.
	 * @param sep Will contain the seperator actually found.
	 * @return
	 */
	template<typename T>
	bool getTokenAs(T& res,const std::string &separators = whitespaces,
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
	void skipOverAll(const std::string& seps);

	/**
	 * @brief skipOverAll Skipps all consecutive appearences of whitespaces.
	 * Example:
	 * "           foo...." ---skipWhiteSpaces()--> "foo...."
	 */
	void skipWhiteSpaces();

	/**
	 * @brief getRest The remaining string,
	 * @return
	 */
	char* getRest(){ return m_rest; }


	/**
	 * @brief readEscString munches off an escaped string.
	 * >"hello \"World\" !"< -> >hello "World" !<
	 * @return
	 */
	bool readEscString(std::string& res)
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
	char* getTokenTillClosing(char opening, char closing)
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


template<> inline bool Tokenizer::getTokenAs<int>(
		int& res,
		const std::string &seps,
		char *sep )
{
	char* c = getToken(seps,sep);
	if(c)
		res = atoi(c);
	return c;
}

template<> inline bool Tokenizer::getTokenAs<uint>(
		uint& res,
		const std::string &seps,
		char *sep )
{
	char* c = getToken(seps,sep);
	if(c)
		res = static_cast<uint>(atoi(c));
	return c;
}

template<> inline bool Tokenizer::getTokenAs<float>(
		float& res,
		const std::string &seps ,
		char *sep )
{
	char* c = getToken(seps,sep);
	if(c)
		res = atof(c);
	return c;
}


template<> inline bool Tokenizer::getTokenAs<double>(
		double& res,
		const std::string &seps ,
		char *sep )
{
	char* c = getToken(seps,sep);
	if(c)
		res = atof(c);
	return c;
}

template<> inline bool Tokenizer::getTokenAs<bool>(
		bool& res,
		const std::string &seps ,
		char *sep )
{
	char* c=  getToken(seps,sep);
	if(c)
		res = strcmp(c,"false");
	return c;
}


template<> inline bool Tokenizer::getTokenAs<glm::vec4>(
		glm::vec4& res,
		const std::string &seps,
		char *sep )
{
	// should look like "(x,y,z,w)"
	char* c = getToken(seps,sep);
	if(!c)
		return  false;

	Tokenizer t(c);

	t.getToken('(');
	bool r =  t.getTokenAs(res.x,",") && t.getTokenAs(res.y,",") && t.getTokenAs(res.z,",") && t.getTokenAs(res.w,")");
	t.setBase(nullptr);
	return r;
}


template<> inline bool Tokenizer::getTokenAs<glm::mat4>(
		glm::mat4& res,
		const std::string &seps,
		char *sep )
{
	// should look like "((x,y,z,w),(x,y,z,w),(x,y,z,w),(x,y,z,w))"
	char* c = getToken(seps,sep);
	if(!c)
		return  false;

	Tokenizer t(c);

	t.getToken('(');
	bool r =  t.getTokenAs(res[0],",") && t.getTokenAs(res[1],",") && t.getTokenAs(res[2],",") && t.getTokenAs(res[3],")");
	t.setBase(nullptr);
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

	uint32_t attribute_count() const{return static_cast<uint32_t>(m_attributes.size());}

	void add_attribute(const Attribute& a)
	{
		m_attributes.push_back(a);
		m_attrib_from_id[a.attribute_id] = static_cast<uint32_t>(m_attributes.size()-1);
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

	bool getAttribute(const AttributeID id, vec4& v)const ;
	bool getAttribute(const AttributeID id, vec3& v) const;
	bool getAttribute(const AttributeID id, vec2& v) const;
	bool getAttribute(const AttributeID id, float& v) const;

	bool setAttribute(const AttributeID id,const vec4& v) const;
	bool setAttribute(const AttributeID id,const vec3& v) const;
	bool setAttribute(const AttributeID id,const vec2& v) const;
	bool setAttribute(const AttributeID id,const float v) const;

	void* get_attribute_ptr(const AttributeID id);

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
			   const typ::Type index_type = typ::Type(typ::UNSIGNED_SHORT),
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




	typ::Type index_type()const {return  m_index_type;}
	uint32_t index_count()const{return m_index_count;}
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




	uint32_t get_index(const uint32_t i) const;


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

	static bool recalculate_normals(VertexData* vd, AttributeID to_attribute=ATTRIB_NORMAL);
	static bool recalculate_tangents(VertexData* vd,AttributeID to_attribute=ATTRIB_TANGENT);

	static VertexData* reconfigure(VertexData* data, const VertexConfiguration& new_config, bool null=false);

};

