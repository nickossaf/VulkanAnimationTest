#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;

layout(binding = 1) uniform UniformBufferObject
{
  mat4 model;
  mat4 view;
  mat4 proj;
  float time;
} ubo;


layout(location = 0) out vec2 coordTex;
layout(location = 1) out vec2 id;

void main(void)
{
  vec4 pos = vec4(vertex, 0, 1.0);
  if (gl_VertexIndex < 4) {
    if(gl_VertexIndex < 2) {
      pos.z = -2;
    } else {
      pos.z = 6;
    }
  } else {
    pos.xy /= 2;

    float len = pos.y;
    pos.z += sin((ubo.time + 5 * gl_InstanceIndex + 17 * pos.x) / 75 + pos.y * 10) / 10 * pos.y;
    //float angle = pos.y + ubo.time / 90;
    //pos.y += len * asin(angle);

    vec3 offset = vec3(-1.95, 0, -1.95); 
    pos.xyz += offset;
    pos.z += 0.78 * (gl_InstanceIndex % 10);
    int shift = gl_InstanceIndex / 10;
    pos.x += 0.39 * shift;
    //pos.z = 6 - 0.7. * gl_InstanceIndex;
    //pos.y = -pos.y + 1;
  }
  
  gl_Position = ubo.proj * ubo.view * ubo.model * pos;
  coordTex = texCoord;   
  id = vec2(gl_InstanceIndex, gl_InstanceIndex);
}
