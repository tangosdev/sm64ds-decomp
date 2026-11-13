/* NitroSDK fx_atan.c (pret pokediamond arm9/lib/NitroSDK/src/FX_atan.c):
 * the angle of (x, y) from the +y axis toward +x, as a 16-bit game angle
 * (0x10000 per turn). The quadrant and octant pick a bias and a sign, and
 * the octant's ratio indexes FX_AtanTable_ (FX_AtanTable_.c). Later SDKs return fx16 radians instead.
 * FX_Atan, defined above FX_Atan2 in the SDK file, is dead-stripped here. */

#include "types.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"

u16 FX_Atan2(fx32 x, fx32 y) {
    u32 positive, bias, denominator, numerator;
    if (x > 0) {
        if (y > 0) {
            if (y > x) {
                numerator = (u32)x;
                denominator = (u32)y;
                bias = 0;
                positive = 1;
            } else if (y < x) {
                numerator = (u32)y;
                denominator = (u32)x;
                bias = 0x4000;
                positive = 0;
            } else {
                return 0x2000;
            }
        } else if (y < 0) {
            y = -y;
            if (y < x) {
                numerator = (u32)y;
                denominator = (u32)x;
                bias = 0x4000;
                positive = 1;
            } else if (y > x) {
                numerator = (u32)x;
                denominator = (u32)y;
                bias = 0x8000;
                positive = 0;
            } else {
                return 0x6000;
            }
        } else {
            return 0x4000;
        }
    } else if (x < 0) {
        x = -x;
        if (y < 0) {
            y = -y;
            if (y > x) {
                numerator = (u32)x;
                denominator = (u32)y;
                bias = (u32)(-0x8000);
                positive = 1;
            } else if (y < x) {
                numerator = (u32)y;
                denominator = (u32)x;
                bias = (u32)(-0x4000);
                positive = 0;
            } else {
                return 0xA000;
            }
        } else if (y > 0) {
            if (y < x) {
                numerator = (u32)y;
                denominator = (u32)x;
                bias = (u32)(-0x4000);
                positive = 1;
            } else if (y > x) {
                numerator = (u32)x;
                denominator = (u32)y;
                bias = 0x0;
                positive = 0;
            } else {
                return 0xE000;
            }
        } else {
            return 0xC000;
        }
    } else {
        if (y >= 0)
            return 0x0;
        else
            return 0x8000;
    }
    if (denominator == 0x0)
        return 0x0;
    if (positive)
        return (u16)(bias + FX_AtanTable_[_Z6FX_Divii((fx32)numerator, (fx32)denominator) >> 5]);
    else
        return (u16)(bias - FX_AtanTable_[_Z6FX_Divii((fx32)numerator, (fx32)denominator) >> 5]);
}
