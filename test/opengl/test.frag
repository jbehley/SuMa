#version 330

in vec2 texCoords;

uniform sampler2D tex_input;
uniform float offset;

out vec3 color;

void main()
{
  vec2 coords = vec2(texCoords.x + offset, texCoords.y);
  vec2 tex = texture(tex_input, coords.xy).rg;
  color = vec3(tex.xy, 0);  
}