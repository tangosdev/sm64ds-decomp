#include "types.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_trig.h"

/* Places the eye on the orbit around the target: distance away, at the
 * camera's elevation and azimuth. out/base stay pointers into eye and
 * target: spelled cam->eye.x .. cam->target.z directly, the function
 * compiles 16 bytes short. The table reads stay hand-indexed: any one of
 * them through FX_SinIdx/FX_CosIdx (nitro/fx/fx_trig.h) moves the table
 * base, the cosine and indexB to other registers (13 to 26 words differ,
 * whatever the angle locals' declaration order, types or shift placement). */
void func_ov007_020bffb8(Ov007Camera *cam) {
    s32 *out = &cam->eye.x;
    s32 *base = &cam->target.x;
    s32 angleA = cam->elevation;
    s32 angleB = cam->azimuth;
    s32 indexA;
    s32 indexB;
    s32 trigAPlus;

    angleA >>= 4;
    angleB >>= 4;
    indexA = angleA << 1;
    indexB = angleB << 1;
    trigAPlus = FX_SinCosTable_[indexA + 1];

    {
        s32 initialX = (trigAPlus * cam->distance) >> 12;
        s32 trigB = FX_SinCosTable_[indexB];
        s32 storedX;
        s32 product;

        out[0] = initialX;
        storedX = out[0];
        product = storedX * trigB;
        out[0] = base[0] + (product >> 12);
    }
    {
        s32 trigA = FX_SinCosTable_[indexA];
        s32 product = trigA * cam->distance;
        out[1] = base[1] + (product >> 12);
    }
    {
        s32 initialZ = (trigAPlus * cam->distance) >> 12;
        s32 trigB = FX_SinCosTable_[indexB + 1];
        s32 storedZ;
        s32 product;

        out[2] = initialZ;
        storedZ = out[2];
        product = storedZ * trigB;
        out[2] = base[2] + (product >> 12);
    }
}
