//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Ov007Sprite.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_cp.h"

/* Sizes the sprite in world space for the camera: its pixel width and
 * height times |depth| * sin(fovy) / (cos(fovy) * 0x60), 0x60 being half the
 * screen's 192 lines. Each axis has its own block: with the height half
 * reusing the width half's locals, it schedules the height load early and
 * 25 words differ. */
extern "C" void func_ov007_020c43bc(Ov007Sprite *sprite, Ov007Camera *camera) {
    {
        int x = sprite->depth;
        int idx = camera->fovy >> 4;
        s16 cosFov = FX_CosIdx(idx);
        if (x < 0)
            x = -x;
        s16 sinFov = FX_SinIdx(idx);
        sprite->worldWidth =
            FX_Div(FX_MUL((sprite->width << 12), FX_MUL(sinFov, x)), cosFov * 0x60);
    }
    {
        int x = sprite->depth;
        int idx = camera->fovy >> 4;
        s16 cosFov = FX_CosIdx(idx);
        if (x < 0)
            x = -x;
        s16 sinFov = FX_SinIdx(idx);
        sprite->worldHeight =
            FX_Div(FX_MUL((sprite->height << 12), FX_MUL(sinFov, x)), cosFov * 0x60);
    }
}
