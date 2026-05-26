#ifdef VERTEX_SHADER

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

layout(std140, binding = 0) uniform Shader_Data {
  mat4 u_projection;
  mat4 u_transform;
  vec2 u_uv_size;
  vec2 u_uv_offset;
  vec4 u_tint;
  int u_tex_unit;
};

out vec2 v_uv;
out vec4 v_tint;
out flat int v_tex_unit; 

void main() {
    gl_Position = u_projection * u_transform * vec4(a_pos.x, a_pos.y, 0.0, 1.0);
    v_uv = a_uv * u_uv_size + u_uv_offset;
    v_tex_unit = u_tex_unit;
    v_tint = u_tint;
}
#endif

#ifdef FRAGMENT_SHADER

in vec2 v_uv;
in flat int v_tex_unit; 
in vec4 v_tint;

layout(location = 0) out vec4 o_col;

#define MAX_TEXTURES 32

uniform sampler2D u_samplers[MAX_TEXTURES];

void main() {
  o_col = v_tint * texture(u_samplers[v_tex_unit], v_uv);
}

#endif