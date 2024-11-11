#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 0) uniform MVP {
  mat4 model;
  mat4 view;
  mat4 proj;
} mvp;

layout(location = 0) in vec2 in_vtxPosition;
layout(location = 1) in vec3 in_vtxColor;
layout(location = 2) in vec2 in_vtxTexcoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexcoord;


void main() {
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(in_vtxPosition, 0.0, 1.0);
    fragColor = in_vtxColor;
    fragTexcoord = in_vtxTexcoord;
}
