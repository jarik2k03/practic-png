#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexcoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler image_sampler;
layout(set = 0, binding = 1) uniform texture2D image_texture;

vec4 RGB_to_XYZ(vec4 unorm) {

  const mat3x3 data = mat3x3(
  0.4124564f,0.3575761f,0.1804375f,
  0.2126729f,0.7151522f,0.0721750f,
  0.0193339f,0.1191920f,0.9503041f);
  return vec4(unorm.xyz * data, unorm.w);
}

vec4 XYZ_Srgb(vec4 xyz) {
  const vec4 conv = vec4(xyz.x * 0.33f, xyz.y * 0.3f, xyz.z * 0.15f, xyz.w);
  return conv;

}
vec4 XYZ_to_RGB(vec4 xyz) {
  const mat3x3 data = mat3x3(
  3.2404542f, -1.5371385f, -0.4985314f,
  -0.9692660f, 1.8760108f, 0.0415560f,
  0.0556434f, -0.2040259f, 1.0572252f);

  return vec4(xyz.xyz * data, xyz.w);

}

void main() {
    const vec4 unorm_pixel = texture(sampler2D(image_texture, image_sampler), fragTexcoord); // - стандартный rgba
    outColor = unorm_pixel;
}
