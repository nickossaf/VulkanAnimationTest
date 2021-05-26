#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;

layout(binding = 1) uniform UniformBufferObject
{
  int time;
} ubo;


layout(location = 0) out vec2 coordTex;

void main(void)
{
  mat4 PTr = mat4 (
    -1.0, 0.0, 0.0, 0.0,
    0.0, -1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0
  );

  vec4 pos = vec4(vertex, 1, 1.0);
  gl_Position = PTr * pos;

  coordTex = texCoord;    
}
