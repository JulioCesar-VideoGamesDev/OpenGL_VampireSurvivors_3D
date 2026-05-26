#ifdef VERTEX_SHADER

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;

layout(std140, binding = 0) uniform Shader_Data {
  mat4 u_projection;
  mat4 u_transform;
  vec2 u_uv_size;
  vec2 u_uv_offset;
  vec4 u_tint;
  int u_tex_unit;
};

out vec2 v_uv;
out vec3 v_normal;
out vec4 v_color;
out flat int v_tex_unit; 

void main() {
    gl_Position = u_projection * u_transform * vec4(a_pos.xyz, 1.0);
    v_tex_unit = u_tex_unit;
    v_uv = a_uv;
    v_normal = a_normal;
    v_color = a_color;
}
#endif

#ifdef FRAGMENT_SHADER

in vec2 v_uv;
in vec3 v_normal;
in vec4 v_color;
in flat int v_tex_unit; 

layout(location = 0) out vec4 o_col;

#define MAX_TEXTURES 32

uniform sampler2D u_samplers[MAX_TEXTURES];

void main() {
  o_col = v_color * texture(u_samplers[v_tex_unit], v_uv);
}

#endif