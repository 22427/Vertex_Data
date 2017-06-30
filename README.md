vd : Vertex Data
=====================================
A simple toolkit to read, write, convert ertex data of different kinds. This little toolset allows you to read .obj .off and .ply, apply different opperations on the vertex data and store it in different formats.

Operations
----------------
The main purpose of this toolset is to provide vertex data in a OpenGL friendly format (eg. an index array and a block of interleaved vertex data), supporting different representations for different attributes. On top of that the structure of the vertex data shall be transparent. Uploding a VertexData structure into OpenGL-buffers as well as setting the vertex attribute pointers is really easy:
```C
VertexData vd;
GLuint buffers[2];
glGenBuffers(2,buffers);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[0]);
glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);

glBufferData(GL_ELEMENT_ARRAY_BUFFER, vd.indices_size(), vd.indices(), GL_STATIC_DRAW);
glBufferData(GL_ARRAY_BUFFER, vd.vertex_data_size(), vd.vertex_data(), GL_STATIC_DRAW);

const VertexConfiguration& vcfg = vd.vertex_configuration();

for(int i =0 ; i <AID_COUNT ; i++)
{
  if(vcfg.active_mask & (1<<i))
  {
    glVertexAttribPointer(i,
 	  vcfg.attributes[i].elements,
 	  vcfg.attributes[i].type,
 	  vcfg.attributes[i].normalized,
 	  vcfg.size(),
 	  (const void*) vcfg.attributes[i].offset);
    
    glEnableVertexAttribArray(i);
  }
}
```



