#pragma once

// @Pending: (He perdido la paciencia, voy a escribir esto en español).

// Este modelo no es el que realmente querríamos para una app real.

// Tanto la carga de imágenes como de meshes ha de ir a un buffer de
// pixeles como de vertices respectivamente. No esto que tenemos.
// Luego simplemente la info que tienen los propios objetos de 
// IO_Image e IO_Mesh, serían los rangos de ese array.

// A futuro, querríamos hacerlo dinámico y poder descargar imagenes, modelos
// a placer, así que ahí las solución es trabajar por páginas.
// Tener un modelo de almacenamiento con páginas pequeñas, medianas,
// y grandes e ir colocando las imágenes y mallas en cada una. Así al librerar
// libreraríamos por página. 

struct IO_Image {
    u8* data = nullptr;
    s32 width = 0;
    s32 height = 0;
    s32 channels = 0;
    bool is_owner = false;
};

fn io_image_white() -> const IO_Image*;
fn io_image_load(std::string_view filename, IO_Image* image) -> bool;
fn io_image_free(IO_Image* image) -> void;
fn io_image_valid(const IO_Image& image) -> bool;