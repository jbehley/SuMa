#version 330 core

layout (location = 0) in vec4 p;

out vec4 pix_coords;

void main()
{
  gl_Position = vec4(p.x, p.y, 0, 1.0);
  pix_coords = vec4(p.x, p.y, p.z, p.w);
}