#version 330 core

layout (location = 0) in vec2 position;

out vec4 data;

uniform int width;
uniform int height;

vec2 pix2ndc(int x, int y, int w, int h)
{
 return vec2((2.0 * x + 1.0) / w - 1.0, (2.0 * y + 1.0) / h - 1.0); 
}

void main()
{
  vec2 coords = pix2ndc(int(position.x), int(position.y), width, height);
  gl_Position = vec4(coords, 0, 1.0);
  data = vec4(position.x, position.y, coords.x, coords.y);  
}