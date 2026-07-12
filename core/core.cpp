#include <lodepng.h>
#include <quant.h>
#include <flashpck.h>
#include <cstdint>

#include "core.h"

#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>

#include <SDL3/SDL.h>

namespace core {

static PckData pck = {0,0,0};


IndexedImage::IndexedImage(RgbaImage rgba, uint32_t color_count) {
    width = rgba.width;
    height = rgba.height;

    indexes.resize(width * height);
    color_table.resize(color_count);

    liq_attr* attr = liq_attr_create();
    liq_set_max_colors(attr, color_count);

    liq_image* image = liq_image_create_rgba(
        attr,
        rgba.colors.data(),
        width,
        height,
        0.0
    );

    liq_result* result;
    if (liq_image_quantize(image, attr, &result) != LIQ_OK) {
        liq_image_destroy(image);
        liq_attr_destroy(attr);
        throw std::runtime_error("Failed to quantize image.");
    }

    if (liq_write_remapped_image(
            result,
            image,
            indexes.data(),
            indexes.size()) != LIQ_OK)
    {
        liq_result_destroy(result);
        liq_image_destroy(image);
        liq_attr_destroy(attr);
        throw std::runtime_error("Failed to remap image.");
    }

    const liq_palette* palette = liq_get_palette(result);

    color_table.resize(palette->count);

    for (unsigned i = 0; i < palette->count; ++i) {
        color_table[i].r = palette->entries[i].r;
        color_table[i].g = palette->entries[i].g;
        color_table[i].b = palette->entries[i].b;
        color_table[i].a = palette->entries[i].a;
    }

    liq_result_destroy(result);
    liq_image_destroy(image);
    liq_attr_destroy(attr);
}

void IndexedImage::resize(uint32_t new_width, uint32_t new_height)
{
    std::vector<uint8_t> new_indexes(new_width * new_height);

    for (uint32_t y = 0; y < new_height; ++y) {
        uint32_t sy = y * height / new_height;

        for (uint32_t x = 0; x < new_width; ++x) {
            uint32_t sx = x * width / new_width;

            new_indexes[y * new_width + x] =
                indexes[sy * width + sx];
        }
    }

    indexes = std::move(new_indexes);
    width = new_width;
    height = new_height;
}

RgbaImage::RgbaImage(std::string filename)
{
    std::vector<unsigned char> image;

    unsigned error = lodepng::decode(
        image,
        width,
        height,
        filename
    );

    if (error)
        throw std::runtime_error(lodepng_error_text(error));

    colors.resize(width * height);

    for (size_t i = 0; i < colors.size(); ++i) {
        colors[i].r = image[i * 4 + 0];
        colors[i].g = image[i * 4 + 1];
        colors[i].b = image[i * 4 + 2];
        colors[i].a = image[i * 4 + 3];
    }
}

void RgbaImage::resize(uint32_t new_width, uint32_t new_height)
{
    SDL_Surface* src = SDL_CreateSurfaceFrom(
        width,
        height,
        SDL_PIXELFORMAT_RGBA32,
        colors.data(),
        width * sizeof(RGBAColor)
    );

    SDL_Surface* dst = SDL_CreateSurface(
        new_width,
        new_height,
        SDL_PIXELFORMAT_RGBA32
    );

    SDL_BlitSurfaceScaled(src, nullptr, dst, nullptr, SDL_SCALEMODE_LINEAR);

    colors.resize(new_width * new_height);
    memcpy(colors.data(), dst->pixels, colors.size() * sizeof(RGBAColor));

    width = new_width;
    height = new_height;

    SDL_DestroySurface(src);
    SDL_DestroySurface(dst);
}

void load_pck(std::string filename) {
    if(pck.og_base_address != 0) {
        pckData_free(&pck);
        pck = {0,0,0};
    }

    if(pckData_init(&pck, (char*)filename.c_str()) != 0)
        throw std::runtime_error("Invalid data " + filename);
}

void unload_pck() {
    pckData_free(&pck);
    pck = {0,0,0};
}

void save_pck(std::string filename) {
    if(pckData_write(&pck, (char*)filename.c_str()))
        throw std::runtime_error("Unable to save the file");
}

std::vector<Ps2Image> get_images() {
    if(pck.og_base_address == 0)
        throw std::runtime_error("Pck not loaded");

    std::vector<Ps2Image> out(0);
    swfFILE* file = (swfFILE*)pck.actual_data;
    for(int o_i = 1; o_i < file->objectCount; o_i++) {
        swfOBJECT_header* header =  pckData_get_obj(&pck, o_i);
        if(header->objectType == SWF_OBJ_BITMAP) {
            swfBITMAP* bmp = (swfBITMAP*)header;
            bmpInfo1* info1 = (bmpInfo1*)pckData_get_ptr_from_og(&pck, bmp->ptr_to_info1);
            out.push_back(Ps2Image(info1));
        }
    }
    return out;
}

RgbaImage::RgbaImage(IndexedImage indexed) {
    this->width = indexed.width;
    this->height = indexed.height;

    this->colors.resize(indexed.width * indexed.height);

    for (size_t i = 0; i < colors.size(); ++i)
        colors[i] = indexed.color_table[indexed.indexes[i]];
}

void RgbaImage::save(const std::string& fname)
{
    if (colors.size() != static_cast<size_t>(width) * height) {
        throw std::runtime_error("Invalid RGBA image");
    }

    unsigned err = lodepng_encode32_file(
        fname.c_str(),
        reinterpret_cast<const unsigned char*>(colors.data()),
        width,
        height);

    if (err != 0) {
        throw std::runtime_error(
            std::string("Failed to save PNG: ") +
            lodepng_error_text(err));
    }
}

void IndexedImage::save(const std::string& fname) const
{
    RgbaImage(*this).save(fname);
}

IndexedImage::IndexedImage(bmpInfo1* info) {
    if(pck.og_base_address == 0)
        throw std::runtime_error("Pck not loaded");
    width = info->width;
    height = info->height;
    ps2Texture* tex = (ps2Texture*)pckData_get_ptr_from_og(&pck, info->ptr_to_tex_data);
    ps2IndexedColors* table = (ps2IndexedColors*)pckData_get_ptr_from_og(&pck, info->indexed_colors_ptr);
    uint32_t num_pixel = width * height;
    color_table = std::vector<RGBAColor>(table->color_count);
    indexes = std::vector<uint8_t>(num_pixel);
    uint8_t* tex_raw = (uint8_t*)pckData_get_ptr_from_og(&pck, tex->ptr_to_texture);

    switch (tex->texture_type) {
        case BPP4_UNSWIZZLED: {
            std::memcpy(color_table.data(),
                        table->indexed_colors,
                        color_table.size() * sizeof(RGBAColor));
            std::vector<uint8_t> unpacked(num_pixel);
            swfBITMAP_4bpp_to_8bpp(tex_raw, unpacked.data(), width, height);
            std::memcpy(indexes.data(), unpacked.data(), indexes.size());
            break;
        }

        case BPP8_INDEX_SWIZZLED_BUT_TEXTURE_NOT: {
            swfBITMAP_unswizzle_palette(
                table->indexed_colors,
                color_table.data(),
                table->color_count);
            
            std::memcpy(indexes.data(), tex_raw, indexes.size());
            break;
        }

        case BPP8_BOTH_SWIZZLED: {
            swfBITMAP_unswizzle_palette(
                table->indexed_colors,
                color_table.data(),
                table->color_count);
            
            swfBITMAP_unswizzle8(
                indexes.data(),
                tex_raw,
                width,
                height);
            
            break;
        }
        default:
            throw std::runtime_error("Unsupported texture type");
    }
    for(auto& color : color_table)
        if(color.a > 0)
            color.a = 255;
}

IndexedImage::IndexedImage(std::string filename, uint32_t color_count) {
    uint8_t* raw_data;
    uint32_t width, height;
    auto err_code = lodepng_decode32_file(&raw_data, &width, &height, filename.c_str());
    if (err_code != 0) {
        throw std::runtime_error("Png image is invalid or does not exist: " + filename);
    }

    auto attr = liq_attr_create();
    liq_set_max_colors(attr, color_count);
    auto png_image = liq_image_create_rgba(attr, raw_data, width, height, 0.0);

    auto result = liq_quantize_image(attr, png_image);
    if(result == NULL) {
        throw std::runtime_error("Quantization failed: " + filename);
    }

    this->indexes =  std::vector<uint8_t>(width * height);

    this->color_table =  std::vector<RGBAColor>(color_count);
    liq_write_remapped_image(result, png_image, &indexes[0], width * height);

    auto palette = liq_get_palette(result);
    for(int i = 0; i < palette->count; ++i) {
        const auto& entry = palette->entries[i];

        color_table[i].r = entry.r;
        color_table[i].g = entry.g;
        color_table[i].b = entry.b;
        color_table[i].a = entry.a;
    }
    this->width = width;
    this->height = height;

    free(raw_data);
    liq_attr_destroy(attr);
    liq_image_destroy(png_image);
    liq_result_destroy(result);
}

Ps2Image::Ps2Image(bmpInfo1* info)
    : address(pckData_get_og_from_ptr(&pck, info)),
      type(((ps2Texture*)pckData_get_ptr_from_og(&pck, info->ptr_to_tex_data))->texture_type),
      image(info)
{
}

void Ps2Image::replace(const std::string& filename) {
    auto new_image = IndexedImage(filename, image.color_table.size());
    if(
        image.width * new_image.width != 
        image.height * new_image.height
    )
        throw std::runtime_error("Aspect ratios doesn't match");

    if (new_image.width != image.width ||
        new_image.height != image.height)
    {
        new_image.resize(image.width, image.height);
    }

    image = std::move(new_image);
    write();
}

void Ps2Image::write() {
    bmpInfo1* info = (bmpInfo1*)pckData_get_ptr_from_og(&pck, address);
    if(info->height != image.height || info->width != image.width) {
        char error_buf[2048 * 2];
        snprintf(error_buf, sizeof(error_buf), "Tried writing image with w: %d, h: %d\nBut actual size is w: %d, h: %d", image.width, image.height, info->width, info->height);
        throw std::runtime_error(error_buf);
    }
    ps2Texture* tex = (ps2Texture*)pckData_get_ptr_from_og(&pck, info->ptr_to_tex_data);
    uint8_t* tex_raw = (uint8_t*)pckData_get_ptr_from_og(&pck, tex->ptr_to_texture);
    ps2IndexedColors* idx = (ps2IndexedColors*)pckData_get_ptr_from_og(&pck, info->indexed_colors_ptr);

    if(tex->texture_type != type) {
        char error_buf[1024];
        snprintf(error_buf, sizeof(error_buf), "Texture type mismatch %d != %d", tex->texture_type, type);
        throw std::runtime_error(error_buf);
    }

    switch(type) {
        case BPP4_UNSWIZZLED: {
            swfBITMAP_8bpp_to_4bpp(image.indexes.data(), tex_raw, image.width, image.height);
            std::memcpy(idx->indexed_colors,
                        image.color_table.data(),
                        image.color_table.size() * sizeof(RGBAColor));
            break;
        }
        case BPP8_INDEX_SWIZZLED_BUT_TEXTURE_NOT: {
            swfBITMAP_swizzle_palette(image.color_table.data(),
                                      idx->indexed_colors,
                                      image.color_table.size());
            std::memcpy(tex_raw, image.indexes.data(), image.width * image.height);
            break;
        }
        case BPP8_BOTH_SWIZZLED: {
            swfBITMAP_swizzle_palette(image.color_table.data(),
                                      idx->indexed_colors,
                                      image.color_table.size());
            swfBITMAP_swizzle8(tex_raw, image.indexes.data(), image.width, image.height);
            break;
        }
        default:
            throw std::runtime_error("Unsupported texture type");
    }
}
}
