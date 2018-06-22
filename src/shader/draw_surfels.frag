#version 330 core

uniform int surfelDrawMethod; // 0 - texCoord-based fragement discard, 2 - hexagon based.

in vec2 texCoords;
in vec4 gs_color;
out vec4 color;
in float radius;

// phong shading?

void main()
{
  if(surfelDrawMethod == 0)
  {
    if(dot(texCoords, texCoords) > 1.0f) discard;
    color = gs_color;
  }
  else
  {
    color = gs_color;
  }
}