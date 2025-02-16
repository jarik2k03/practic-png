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

const float xr = 0.64f, yr = 0.33f;
const float xg = 0.3f, yg = 0.6f;
const float xb = 0.15f, yb = 0.06f;
const float Xw = 0.3127f, Yw = 0.329f, Zw = 1.f - (Xw + Yw);

const float Xr = xr / yr, Yr = 1.f, Zr = (1.f - xr - yr) / yr;
const float Xg = xg / yg, Yg = 1.f, Zg = (1.f - xg - yg) / yg;
const float Xb = xb / yb, Yb = 1.f, Zb = (1.f - xb - yb) / yb;

const mat3x3 XYZ = mat3x3(
    Xr, Xg, Xb,
    Yr, Yg, Yb,
    Zr, Zg, Zb);
mat3x3 XYZ_inverted = inverse(mat3x3(transpose(XYZ)));

vec3 S = XYZ_inverted * vec3(3.f * Xw, 3.f * Yw, 3.f * Zw);

// vec3 S = vec3(0.2126729f, 0.7151522f, 0.0721750f);

// mat3x3 data_encode = mat3x3(S * XYZ[0u], S * XYZ[1u], S * XYZ[2u]);

mat3x3 data_encode = mat3x3(
    S.r * Xr, S.g * Xg, S.b * Xb,
    S.r * Yr, S.g * Yg, S.b * Yb,
    S.r * Zr, S.g * Zg, S.b * Zb);

//mat3x3 data_decode = inverse(data_encode);


vec3 sRGB_to_XYZ(vec3 unorm) {
  const mat3x3 data = mat3x3(
  0.4124564,  0.3575761,  0.1804375,
  0.2126729,  0.7151522,  0.0721750,
  0.0193339,  0.1191920,  0.9503041);
  return vec3(unorm * cnv.image_colorspace);
}

vec3 XYZ_to_sRGB(vec3 xyz) {
 const mat3x3 data = mat3x3(
  3.2404542f, -1.5371385f, -0.4985314f,
  -0.9692660f,  1.8760108f,  0.0415560f,
  0.0556434f, -0.2040259f,  1.0572252f);
 return vec3(xyz * cnv.monitor_colorspace);

}
void main() {
    const vec4 unorm_pixel = texture(sampler2D(image_texture, image_sampler), fragTexcoord); // - стандартный rgba
    const vec3 encoded_pixel = sRGB_to_XYZ(unorm_pixel.xyz);
    const vec3 decoded_pixel = XYZ_to_sRGB(encoded_pixel.xyz);

    outColor = vec4(decoded_pixel, unorm_pixel.w);
    // outColor = unorm_pixel;
}
