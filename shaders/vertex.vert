#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 coordTex;

void main(void)
{
  gl_Position = vec4(vertex, 0, 1.0);
  coordTex = texCoord;    
}
