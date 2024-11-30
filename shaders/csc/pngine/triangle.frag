#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D image_sampler;


void main() {
    // float idx = texture(image_sampler, fragTexcoord).r;
    // outColor = vec4(idx, idx, idx, 1.f);
    outColor = texture(image_sampler, fragTexcoord); // - стандартный rgba 
}
