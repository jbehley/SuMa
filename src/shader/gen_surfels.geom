#version 400 core

#include "shader/color.glsl"

layout(points) in;
layout(points, max_vertices = 1) out;

uniform int timestamp;

in SURFEL {
  bool valid;
  vec2 img_coords;
} gs_in[];

out vec4 sfl_position_radius;
out vec4 sfl_normal_confidence;
out int sfl_timestamp;
out vec3  sfl_color_weight_count;

uniform sampler2DRect vertex_map;
uniform sampler2DRect normal_map;
uniform sampler2DRect radiusConfidence_map;

uniform mat4 pose; // current pose of laser scanner.
uniform float log_prior;


void main()
{
  // filter invalid surfels.
  if(gs_in[0].valid)
  {
    vec2 img_coords = gs_in[0].img_coords;
  
    vec4 v = vec4(texture(vertex_map, img_coords).xyz, 1.0);   
    vec4 n = vec4(texture(normal_map, img_coords).xyz, 0.0);
    vec4 v_global = v;
    vec4 n_global = normalize(n); 
    float radius = texture(radiusConfidence_map, img_coords).x;

    float weight = 1.0f;
  
    // all surfels are outputed to the transform feedback buffer.
    sfl_position_radius = vec4(v_global.xyz, radius);  
    sfl_normal_confidence = vec4(n_global.xyz, log_prior);
    sfl_timestamp = timestamp;
    sfl_color_weight_count = vec3(pack(vec3(0,0,1)), weight, timestamp);
    
    EmitVertex();
    EndPrimitive();
  }
}
