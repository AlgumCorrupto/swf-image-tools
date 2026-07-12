#pragma once

#include <flashpck.h>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace core {

struct IndexedImage;
struct RgbaImage;
struct Ps2Image;

// internal representatnion
struct IndexedImage {
    std::vector<uint8_t> indexes;
    std::vector<RGBAColor> color_table;
    uint32_t width, height;

    // constructor from png image
    IndexedImage(std::string filename, uint32_t color_count);
    IndexedImage(RgbaImage rgba, uint32_t color_count); // make this
    void save(const std::string &fname) const;
    void resize(uint32_t new_width, uint32_t new_height); // make this

    // constructor from bmpInfo1
    IndexedImage(bmpInfo1* info);
    IndexedImage() = default;
};

// for display in the GUI
struct RgbaImage {
    uint32_t width, height;
    std::vector<RGBAColor> colors;

    // constructor from indexed image
    RgbaImage(IndexedImage indexed);
    RgbaImage(std::string filename); // make this
    void resize(uint32_t new_width, uint32_t new_height); // make this
    void save(const std::string &fname);
};

// the representation of the image 
// in the pck file
struct Ps2Image {
    IndexedImage image;
    uint32_t address; // of swfInfo1
    uint8_t type;

    Ps2Image(bmpInfo1* info);
    Ps2Image() = default;

    bool operator==(Ps2Image rhs) const {
        return (address == rhs.address) 
        && (type == rhs.type)
        && (image.height == rhs.image.height)
        && (image.width == rhs.image.width); 
    }

    void write();
    void replace(const std::string& filename);
};

void load_pck(std::string filename);
void unload_pck();
void save_pck(std::string filename);
std::vector<Ps2Image> get_images();
}
