#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec2 id;

layout(location = 0) out vec4 color;


void main()
{
  if (fragTexCoord.x < 0 && id.x == 0) {
    color = vec4(0.6, 0.3, 0, 1);
  } else if(fragTexCoord.x < 0) {
    color = vec4(0, 0, 0, 0);
  } else {
    color = texture(texSampler, fragTexCoord);
  } 
  if (color.w < 1) { discard; }



}