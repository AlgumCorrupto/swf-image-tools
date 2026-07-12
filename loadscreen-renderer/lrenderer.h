#include "core.h"

namespace lrenderer {
    void init();
    void deinit();
    std::tuple<
        core::RgbaImage, core::RgbaImage
    > 
    render(core::RgbaImage og, int sx, int sy, int ex, int ey);
}