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
out vec3 v_frag_pos;
out flat int v_tex_unit; 

void main() {
    gl_Position = u_projection * u_transform * vec4(a_pos.xyz, 1.0);
    v_frag_pos = vec3(u_transform * vec4(a_pos.xyz, 1.0));
    v_tex_unit = u_tex_unit;
    v_uv = a_uv;
    v_normal = normalize(mat3(transpose(inverse(u_transform))) * a_normal);
    v_color = a_color;
}
#endif

#ifdef FRAGMENT_SHADER

#define MAX_LIGHTS 32

struct Light_Data {
  vec4 color;
  vec4 pos;
  vec4 view_pos;
};

layout(std140, binding = 1) uniform LightBuffer {
    Light_Data lights[MAX_LIGHTS];
    vec4 light_count;
};

int count = int(light_count.x);

in vec2 v_uv;
in vec3 v_normal;
in vec4 v_color;
in vec3 v_frag_pos;
in flat int v_tex_unit; 

layout(location = 0) out vec4 o_col;

#define MAX_TEXTURES 32
uniform sampler2D u_samplers[MAX_TEXTURES];

void main() { // Avoid hardcoding, add ambient, specular_color and defusse when creating the material
  float ambient_strenght = 0.5;
  float specular_strenght = 0.5;

  vec3 finalLighting = vec3(0.0);
  
  for(int i = 0; i < count; i++) {

    vec3 lightPos = lights[i].pos.xyz;
    vec3 lightColor = lights[i].color.rgb;

    vec3 L = normalize(lightPos - v_frag_pos);

    float diffuse = max(dot(v_normal, L), 0.0);

    finalLighting += lightColor * diffuse;
  }

  vec4 tex = texture(u_samplers[v_tex_unit], v_uv);

  o_col = vec4(finalLighting, 1.0) * tex;
}

#endif