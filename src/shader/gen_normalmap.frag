#version 330

in vec2 texCoords;

uniform int width;
uniform int height;

uniform sampler2DRect vertex_map;

layout (location = 0) out vec4 normal;

uniform int normal_radius;

float wrap(float x, float dim)
{
  float value = x;
  
  while(value >= dim) value = (value - dim);
  while(value < 0) value = (value + dim);
  
  return value;
}

bool valid(vec4 normal)
{
  return (normal.w > 0.5);
}

void main()
{
  float width = textureSize(vertex_map).x;
  vec2 pos = texCoords * textureSize(vertex_map);
  normal = vec4(0.0, 0.0, 0.0, 0.0);
  
  if(texture(vertex_map, pos).w > 0.0f)
  {
    normal.w = 1.0f;

    vec4 p = texture(vertex_map, pos);
    vec4 u = texture(vertex_map, vec2(wrap(pos.x + 1, width), pos.y));
    vec4 v = texture(vertex_map, vec2(pos.x, pos.y + 1));
    vec4 s = texture(vertex_map, vec2(wrap(pos.x - 1, width), pos.y));
    vec4 t = texture(vertex_map, vec2(pos.x, pos.y - 1));

    u.xyz = normalize(u.xyz - p.xyz);
    v.xyz = normalize(v.xyz - p.xyz);
    s.xyz = normalize(p.xyz - s.xyz);
    t.xyz = normalize(p.xyz - t.xyz);
    
    if(u.w < 1.0f && v.w < 1.0f) normal.w = 0;
    if(s.w < 1.0f && t.w < 1.0f) normal.w = 0;
    
    
    if(!valid(u) || !valid(v)) normal.w = 0;

    
    // TODO: check if distances in x/y-direction are similar.
    //if(abs(length(u) - length(s)) / max(length(u), length(s)) > 0.5) normal.w = 0;
    //if(abs(length(v) - length(t)) / max(length(t), length(v)) > 0.5) normal.w = 0;
       
    if(normal.w > 0.0f)
    {
      vec3 w = cross(u.xyz, v.xyz);
      float len = length(w);
      normal = vec4(w / len, 1.0);
      normal.w = int(len > 0.0000001);
    }
  }  
}