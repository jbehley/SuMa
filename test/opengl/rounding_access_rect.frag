#version 330 core

in vec2 texCoords;

uniform sampler2D tex_input; // texture coordinates
uniform sampler2DRect tex_values; // accessed values with texture coordinates.

out vec4 color;


void main()
{
  vec2 t = texture(tex_input, texCoords).xy;
  color = texture(tex_values, texture(tex_input, texCoords).xy);
}