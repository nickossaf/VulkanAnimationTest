#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 color;


void main()
{
  color = texture(texSampler, fragTexCoord);
  if (color.w < 1) { discard; }

}