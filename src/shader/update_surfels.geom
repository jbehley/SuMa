#version 330 core

layout(points) in;
layout(points, max_vertices = 1) out;

in SURFEL 
{
  bool valid;
  vec4 position_radius;
  vec4 normal_confidence;
  int timestamp;
  vec3 color_weight_count;
} gs_in[];

out vec4 sfl_position_radius;
out vec4 sfl_normal_confidence;
out int sfl_timestamp; 
out vec3 sfl_color_weight_count;

void main()
{
  if(gs_in[0].valid)
  {
    gl_Position = gl_in[0].gl_Position;
    
    sfl_position_radius = gs_in[0].position_radius;
    sfl_normal_confidence = gs_in[0].normal_confidence;
    sfl_timestamp = gs_in[0].timestamp;
    sfl_color_weight_count =  gs_in[0].color_weight_count;
    
    EmitVertex();
    EndPrimitive();
  }
}