#version 330 core

layout (location = 0) in vec4 position;
out vec4 coords;

void main()
{
  gl_Position = vec4(position.x, position.y, 0, 1.0);
  coords = position;
  
}