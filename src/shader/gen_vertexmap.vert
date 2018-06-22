#version 330 core
  
layout (location = 0) in vec4 position; // x, y, z, 1

out vec4 vertex_coord; // coordinate of the vertex producing this pixel.

const float pi = 3.14159265358979323846f;
const float inv_pi = 0.31830988618379067154f;
const float pi_2 = 1.57079632679;

// Velodyne HDL-64E: 26.8 vertical fov (+2 <-> -24.8)
uniform float width;
uniform float height;
uniform float fov_up;
uniform float fov_down;
uniform float min_depth;
uniform float max_depth;

void main()
{ 
  float fov = abs(fov_up) + abs(fov_down);
  float depth = length(position.xyz);
  float yaw = atan(position.y, position.x);
  float pitch = -asin(position.z / depth); // angle = acos((0,0,1) * p/||p||) - pi/2 = pi/2 - asin(x) + pi/2 
  
  float x = (-yaw * inv_pi); // in [-1, 1]
  float y = (1.0 - 2.0 * (degrees(pitch) + fov_up) / fov); // in [-1, 1]
  float z = 2.0f * ((depth - min_depth) / (max_depth - min_depth)) - 1.0f; // in [-1, 1]

  // force that each point lies exactly inside the texel enabling reproducible results.
  x = 2.0f * ((floor(0.5f * (x + 1.0f) * width ) + 0.5f ) / width) - 1.0;
  y = 2.0f * ((floor(0.5f * (y + 1.0f) * height ) + 0.5f ) / height) - 1.0;
  
  
  
  gl_Position = vec4(x, y, z, 1.0);
  vertex_coord = vec4(position.xyz, 1.0); 
}
