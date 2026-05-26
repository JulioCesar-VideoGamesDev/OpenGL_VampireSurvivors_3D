#pragma once
#include "graphics.h"
#include "asset.h"

fn draw_init() -> void;
fn draw_sprite(const Texture* tex, s32 sprite, Vec4 tint, const Mat4& transform) -> void;
fn draw_sprite(Vec4 tint, const Mat4& transform) -> void;
fn draw_done() -> void;
fn draw_update(f32 dt) -> void;

struct Material {
    Asset_Handle shader;
    Asset_Handle texture_diffuse; 
};

struct Submesh {
    Asset_Handle material;
    u32 elem_offset = 0u;
    u32 elem_count = 0u;
};

struct Mesh {
    Vertex_Buffer vbo;
    Array<Submesh> submeshes;
};

fn mesh_init(Mesh* mesh, std::string_view filename, Asset_Handle shader, bool normals_as_colors = false) -> void;
fn mesh_done(Mesh* mesh) -> void;
fn draw_mesh(const Mesh* mesh, const Mat4& transform) -> void;