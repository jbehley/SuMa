#version 330 core

in vec2 texCoords;
in vec4 vertex;
in vec4 normal;
in float confidence;

layout (location = 0) out vec4 vertexmap;
layout (location = 1) out vec4 normalmap;

void main()
{

 if(dot(texCoords, texCoords) > 1.0f)
 {
  vertexmap = vec4(0);
  normalmap = vec4(0);
  // indexmap = 0;
  discard;
 }
 
 vertexmap = vec4(vertex.xyz, 1.0);
 normalmap = normal;
 // indexmap = index;

}