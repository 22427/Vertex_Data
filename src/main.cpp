#ifndef BUILDING_DLL
#ifdef BUILD_VD_TOOL
#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <vd_mesh.h>
#include "../include/vd.h"

using namespace  vd;

void print_help()
{
	printf("vd <op> [args] input output\n");
	printf("\t -a <name> <#elements> <type> <normalize?> to add attribute.\n");
	printf("\t\t<name> = {pos,tex,nrm,clr,tan,bnm}\n");
	printf("\t\t<#elements> = {1,2,3,4}\n");
	printf("\t\t<type> = {byte,ubyte,short,ushort,int,uint,float,double}\n");
	printf("\t\t<normalize?> = {t,f}\n");
}

void parse_args(int argc, char** argv,VertexConfiguration& cfg,  std::string& path_in,std::string& path_out)
{
	path_in.clear();
	path_out.clear();
	bool reset_cfg = true;
	for(int i =1 ; i< argc;i++)
	{
		if(std::string(argv[i]) == "-a")
		{
			if(reset_cfg)
			{
				cfg.clear();
				reset_cfg = false;
			}
			std::string aname = argv[++i];
			AttributeID aid = AID_POSITION;

			if(aname == "pos") aid = AID_POSITION;
			if(aname == "tex") aid = AID_TEXCOORD;
			if(aname == "nrm") aid = AID_NORMAL;
			if(aname == "clr") aid = AID_COLOR;
			if(aname == "tan") aid = AID_TANGENT;
			if(aname == "bnm") aid = AID_BITANGENT;

			int nelem = atoi(argv[++i]);
			Type type = std::string(argv[++i]);
			bool nrm = argv[++i][0]=='t';
			cfg.set_attribute(aid,nelem,type,nrm);
		}
		else
		{
			auto it = std::string(argv[i]);
			if(path_in.empty())
			{
				path_in = it;
			}
			else if(path_out.empty())
			{
				path_out = it;
			}
			else
			{
				print_help();
				exit(1);
			}

		}

	}
}

int main(int argc, char ** argv)
{
	if(argc<3)
	{
		print_help();
		return 1;
	}
	std::string path_input="";
	std::string path_output="";
	VertexConfiguration cfg;
	cfg.set_attribute(AID_POSITION,3,FLOAT,false);
	cfg.set_attribute(AID_TEXCOORD,2,FLOAT,false);
	cfg.set_attribute(AID_NORMAL,3,FLOAT,false);
	cfg.set_attribute(AID_COLOR,4,UNSIGNED_BYTE,true);

	parse_args(argc,argv,cfg,path_input,path_output);

	std::string ext_in = path_input.substr(path_input.find_last_of('.')+1);
	for(char &c : ext_in) c= toupper(c);
	std::string ext_out = path_output.substr(path_output.find_last_of('.')+1);
	for(char &c : ext_out) c= toupper(c);

	Mesh m;

	if(ext_in != "VD")
	{
		if(!MeshOPS::read(m, path_input))
		{
			fprintf(stderr,"[ERR] Reading mesh:\n\t%s\n",
					MeshOPS::error_msg().c_str());
			print_help();
			return 2;
		}
	}
	else
	{
		VertexData vd;
		if(!VertexDataOPS::read(vd,path_input))
			return 2;
		VertexDataOPS::to_mesh(vd,m);
	}

	if((cfg.active_mask & AM_TANGENT) && !(m.active_mask & AM_TANGENT))
	{
		if(!MeshOPS::recalculate_tan_btn(m))
			fprintf(stderr,"[ERR] (re)calculate tangent:\n\t%s\n",
					MeshOPS::error_msg().c_str());
	}
	if((cfg.active_mask & AM_BITANGENT) && !(m.active_mask & AM_BITANGENT))
	{
		if(!MeshOPS::recalculate_tan_btn(m))
			fprintf(stderr,"[ERR] (re)calculate tangent:\n\t%s\n",
					MeshOPS::error_msg().c_str());
	}
	if((cfg.active_mask & AM_NORMAL) && !(m.active_mask & AM_NORMAL))
	{
		MeshOPS::recalculate_normals(m);
	}

	m.active_mask &= cfg.active_mask;

	if(ext_out != "VD")
	{
		if(!MeshOPS::write(m, path_output))
		{
			fprintf(stderr,"[ERR] Writing mesh:\n\t%s\n",
					MeshOPS::error_msg().c_str());
			print_help();
			return 3;
		}
	}
	else
	{
		VertexData vd(PRIM_TRIANGLES,cfg);
		VertexDataOPS::from_mesh(vd,m);
		if(!VertexDataOPS::write(vd,path_output))
		{
			fprintf(stderr,"[ERR] Writing VertexData:\n\t%s\n",
					VertexDataOPS::error_msg().c_str());
			return 3;
		}
	}


	return  0;
}

#endif // BUILD_VD_TOOL
#endif
