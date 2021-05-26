#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;

layout(binding = 1) uniform UniformBufferObject
{
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;


layout(location = 0) out vec2 coordTex;

void main(void)
{
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertex, 0.5 * gl_InstanceIndex, 1.0);
  //gl_Position.x += 0.5 * gl_InstanceIndex;
  coordTex = texCoord;    
}
