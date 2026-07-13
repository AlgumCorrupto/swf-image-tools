#pragma once

#include "core.h"

namespace lrenderer {

    #define LSCR_OFFS 6.28318f

    // Texture storage size
    constexpr int LSCR_PART_SIZE = 256;

    constexpr int LSCR_W = LSCR_PART_SIZE * 2;
    constexpr int LSCR_H = LSCR_PART_SIZE;

    // Stored texture aspect ratio
    constexpr int LSCR_STORAGE_ASPECT_W = 2;
    constexpr int LSCR_STORAGE_ASPECT_H = 1;

    // Possible final display aspect ratios
    constexpr int SCREEN_16_9_W = 16;
    constexpr int SCREEN_16_9_H = 9;

    constexpr int SCREEN_4_3_W = 4;
    constexpr int SCREEN_4_3_H = 3;

    std::tuple<
        core::RgbaImage, core::RgbaImage
    > render(core::RgbaImage og, int sx, int sy, int ex, int ey);
}