#pragma once

// @Pending:

// Este modelo no es el que realmente querríamos para una app real.

// (para desarrollo mirar io_image.h).

struct IO_Model_VTX {
    Vec3 pos;   
    Vec2 uv;
    Vec3 normal;
    Vec4 color;
};

struct IO_Model_Shape {
    struct {
        std::string ambient;
        std::string diffuse;
    } textures;

    std::string name;
    u32 index_offset = 0;
    u32 index_count = 0;
    u32 material_index = 0;
};

struct IO_Model {
    bool normals_as_colors = false;
    std::string name;
    std::string dirpath;
    Array<IO_Model_VTX> vertices;
    Array<u32> elems;
    Array<IO_Model_Shape> shapes;
};

fn io_model_load(std::string_view filename, IO_Model* model) -> bool;
fn io_model_done(IO_Model* model) -> void;