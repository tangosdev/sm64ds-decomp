#ifndef NITRO_FX_FX_TRIG_H
#define NITRO_FX_FX_TRIG_H

/*
 * NitroSDK's table sine and cosine (nitro/fx/fx_trig.h): FX_SinCosTable_ holds
 * a {sin, cos} pair of fx16 for each of the 4096 steps of a full turn, so a
 * 16-bit game angle indexes it as angle >> 4.
 *
 * Most sources declare the table locally and index it by hand. Through these
 * inlines the index and the load become their own expression, which changes
 * register allocation, so switching a caller over is matching work, not a
 * respelling.
 */

#include "nitro/fx/fx.h"

extern const fx16 FX_SinCosTable_[];

#ifdef __cplusplus
extern "C" {
#endif

/* fx_atan.c: FX_Atan2(x, y) is the angle of (x, y) from +y toward +x, as a
 * u16 with 0x10000 per turn. The parameters are unnamed on purpose: each
 * named parameter in a declaration costs every C++ file that includes this
 * header a compiler @NNN number. */
extern const fx16 FX_AtanTable_[128];
unsigned short FX_Atan2(fx32, fx32);

#ifdef __cplusplus
}
#endif

static inline fx16 FX_SinIdx(int idx) {
    return FX_SinCosTable_[idx * 2];
}

static inline fx16 FX_CosIdx(int idx) {
    return FX_SinCosTable_[idx * 2 + 1];
}

#endif /* NITRO_FX_FX_TRIG_H */
