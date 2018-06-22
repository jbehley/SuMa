#version 330 core

in vec4 pix_coords;
out vec4 color;

void main()
{
  color = pix_coords;
}