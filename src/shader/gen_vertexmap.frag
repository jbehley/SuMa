#version 330 core

in vec4 vertex_coord;
out vec4 color;

void main()
{
  color = vertex_coord;
}