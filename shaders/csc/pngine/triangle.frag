#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform conversion {
  mat3x3 image_colorspace;
  mat3x3 monitor_colorspace;
} cnv;
layout(set = 1, binding = 1) uniform sampler image_sampler;

layout(set = 0, binding = 1) uniform texture2D image_texture;

void main() {
    const vec4 unorm_pixel = texture(sampler2D(image_texture, image_sampler), fragTexcoord); // - стандартный rgba
    const vec3 encoded = vec3(unorm_pixel.rgb * cnv.image_colorspace);
    const vec3 decoded = vec3(encoded * cnv.monitor_colorspace);

    outColor = vec4(decoded, unorm_pixel.a);
    // outColor = unorm_pixel;
}
