#include "core.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "lrenderer.h"
#include <cstring>
#include <cmath>

namespace lrenderer {


    static void round_bottom_left(SDL_Surface* surf, int radius)
    {
        auto* pixels = static_cast<RGBAColor*>(surf->pixels);

        const int w = surf->w;
        const int h = surf->h;

        constexpr int border = 2;
        constexpr int pad_left = 4;
        constexpr int pad_bottom = 4;
        constexpr uint8_t br = 100;

        const int cx = radius + pad_left;
        const int cy = h - radius - pad_bottom;

        const int outer2 = radius * radius;
        const int inner = radius - border;
        const int inner2 = inner * inner;

        auto px = [&](int x, int y) -> RGBAColor& {
            return pixels[y * w + x];
        };

        // Clip outside the rounded corner.
        for (int y = cy; y < h; ++y) {
            for (int x = 0; x < cx; ++x) {
                int dx = x - cx;
                int dy = y - cy;

                if (dx * dx + dy * dy > outer2)
                    px(x, y).a = 0;
            }
        }

        for (int x = 0; x < pad_left; ++x)
            for (int y = 0; y < cy; ++y)
                px(x, y).a = 0;

        for (int y = h - pad_bottom; y < h; ++y)
            for (int x = cx; x < w; ++x)
                px(x, y).a = 0;

        // Left border.
        for (int x = pad_left; x < pad_left + border; ++x)
            for (int y = 0; y <= cy; ++y)
                px(x, y) = {br, br, br, 255};

        // Bottom border.
        for (int y = h - pad_bottom - border; y < h - pad_bottom; ++y)
            for (int x = cx - border; x < w; ++x)
                px(x, y) = {br, br, br, 255};

        // Rounded border.
        for (int y = cy; y < h; ++y) {
            for (int x = 0; x < cx; ++x) {
                int dx = x - cx;
                int dy = y - cy;
                int d2 = dx * dx + dy * dy;

                if (d2 <= outer2 && d2 >= inner2)
                    px(x, y) = {br, br, br, 255};
            }
        }
    }

    std::tuple<core::RgbaImage, core::RgbaImage>
    render(core::RgbaImage og, int sx, int sy, int ex, int ey)
    {
        if (sx < 0 || sy < 0 ||
            ex > (int)og.width ||
            ey > (int)og.height ||
            sx >= ex || sy >= ey)
        {
            throw std::runtime_error("Invalid crop rectangle");
        }
    
        SDL_Surface* og_s = SDL_CreateSurface(
            og.width,
            og.height,
            SDL_PIXELFORMAT_RGBA32
        );
    
        std::memcpy(
            og_s->pixels,
            og.colors.data(),
            og.colors.size() * sizeof(RGBAColor)
        );
    
    
        // This is the actual PS2 stored texture size
        SDL_Surface* tgt_s = SDL_CreateSurface(
            LSCR_W,
            LSCR_H,
            SDL_PIXELFORMAT_RGBA32
        );
    
    
        SDL_Rect src = {
            sx,
            sy,
            ex - sx,
            ey - sy
        };
    
        SDL_Rect dst = {
            0,
            0,
            LSCR_W,
            LSCR_H
        };
    
    
        SDL_BlitSurfaceScaled(
            og_s,
            &src,
            tgt_s,
            &dst,
            SDL_SCALEMODE_LINEAR
        );
    
        SDL_DestroySurface(og_s);
    
    
        round_bottom_left(tgt_s, 60);
    
    
        // Split 512x256 into two 256x256 textures
        SDL_Surface* l_s = SDL_CreateSurface(
            LSCR_H,
            LSCR_H,
            SDL_PIXELFORMAT_RGBA32
        );
    
        SDL_Surface* r_s = SDL_CreateSurface(
            LSCR_H,
            LSCR_H,
            SDL_PIXELFORMAT_RGBA32
        );
    
    
        SDL_Rect left = {
            0,
            0,
            LSCR_H,
            LSCR_H
        };
    
        SDL_Rect right = {
            LSCR_H,
            0,
            LSCR_H,
            LSCR_H
        };
    
    
        SDL_BlitSurface(tgt_s, &left, l_s, nullptr);
        SDL_BlitSurface(tgt_s, &right, r_s, nullptr);
    
    
        auto ret = std::make_tuple(
            core::RgbaImage(
                LSCR_H,
                LSCR_H,
                (RGBAColor*)l_s->pixels
            ),
            core::RgbaImage(
                LSCR_H,
                LSCR_H,
                (RGBAColor*)r_s->pixels
            )
        );
    
    
        SDL_DestroySurface(tgt_s);
        SDL_DestroySurface(l_s);
        SDL_DestroySurface(r_s);
    
        return ret;
    }
}