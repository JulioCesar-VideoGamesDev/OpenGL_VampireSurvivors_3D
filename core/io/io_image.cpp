#include "io_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

fn io_image_white() -> const IO_Image* {
    static u8 g_white_pixels[] = { 255, 255, 255, 255 };
    static IO_Image image = { g_white_pixels, 1, 1, 4 };
    return &image;
}

fn io_image_load(std::string_view filename, IO_Image* image) -> bool {
    if (filename.empty() || !image)
    return false;

    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = stbi_load(filename.data(), &image->width, &image->height, &image->channels, 0);
    checkf(data, "IO_Image load failed!\n");
    image->data = data; 
    image->is_owner = true;
    return true;
}

fn io_image_free(IO_Image* image) -> void {

    stbi_image_free(image->data);
    image->data = nullptr;
    image->is_owner = false;
}

fn io_image_valid(const IO_Image& image) -> bool {
    bool is_valid = image.data != nullptr &&
        image.width    > 0 &&
        image.height   > 0 &&
        (image.channels == 1 || image.channels == 3 || image.channels == 4);
    return is_valid;
}