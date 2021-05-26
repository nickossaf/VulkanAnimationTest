#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 coordTex;

void main(void)
{
  gl_Position = vec4(vertex, 0.5, 1.0);
  gl_Position.x = gl_Position.x / 2;
  gl_Position.y = gl_Position.y / 2;
  gl_Position.x = gl_Position.x - 0.5 * gl_InstanceIndex;
  gl_Position.y = -gl_Position.y;	
  coordTex = texCoord;    
}
