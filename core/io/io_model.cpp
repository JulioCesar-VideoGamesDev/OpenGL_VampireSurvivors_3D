#include "io_model.h"
#include "os_core.h"

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

fn io_model_load(std::string_view filename, IO_Model* model) -> bool {
    if (filename.empty() || !model) {
        return false;
    }
    
    // Get path info
    Path_Info info = get_path_info(filename);
    model->dirpath = info.dirpath;
    model->name = info.name;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ok = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        filename.data(),
        model->dirpath.data()
    );

    if (!ok)
    {
        checkf(false, "Mesh could not be loaded!");
        return false;
    }

    reserve(&model->vertices, (u32) attrib.vertices.size() / 3);
    reserve(&model->elems, 1024);

    u64 it_index = 0; for (const auto& shape: shapes)
    {
        u32 index_offset = count(model->elems);
        u32 index_count = 0;
        
        for (const auto& idx : shape.mesh.indices)
        {
            IO_Model_VTX& v = append(&model->vertices);

            v.pos.x = attrib.vertices[3 * idx.vertex_index + 0];
            v.pos.y = attrib.vertices[3 * idx.vertex_index + 1];
            v.pos.z = attrib.vertices[3 * idx.vertex_index + 2];

            if (idx.texcoord_index >= 0)
            {
                v.uv.x = attrib.texcoords[2 * idx.texcoord_index + 0];
                v.uv.y = attrib.texcoords[2 * idx.texcoord_index + 1];
            }
            
            v.normal.x = attrib.normals[3 * idx.normal_index + 0];
            v.normal.y = attrib.normals[3 * idx.normal_index + 1];
            v.normal.z = attrib.normals[3 * idx.normal_index + 2];

            if (model->normals_as_colors)
            {
                v.color.x = v.normal.x;
                v.color.y = v.normal.y;
                v.color.z = v.normal.z;
                v.color.w = 1.f;
            }
            else
            {
               v.color = Color.White;
            }
            
            append(&model->elems, count(model->elems));
            ++index_count;
        }

        IO_Model_Shape& s = append(&model->shapes);
        s.index_offset = index_offset;
        s.index_count = index_count;
        s.material_index = shape.mesh.material_ids.empty() ? 0u : static_cast<u32>(shape.mesh.material_ids[0]);
        
        if (it_index < materials.size())
        {
            s.name = materials[it_index].name;
            s.textures.ambient = materials[it_index].ambient_texname;
            s.textures.diffuse = materials[it_index].diffuse_texname;
        }

        ++it_index;
    }
    
    return true;
}

fn io_model_done(IO_Model* model) -> void {
    reset(&model->vertices);
    reset(&model->elems);
    reset(&model->shapes);
    *model = {};
}