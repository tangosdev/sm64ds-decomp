//cpp
// @symbol func_ov006_02115248
/* func_ov006_02115248 -- dScMgSmartball_c, ov006 0x02115248, 0x238 bytes.
 *
 * Spawns up to four balls: for each of the four passes it takes the first free
 * slot of sixteen, draws two angles from the shared RNG, and fills that slot's
 * position (0x4854 + j*8) and velocity (0x48d4 + j*8) from the sine table,
 * biased by the caller's origin, then marks the slot taken.
 *
 * TWO THINGS ARE LOAD BEARING AND BOTH WERE MEASURED.
 *
 * 1. TVec2 HAS AN EMPTY DESTRUCTOR.  `v` is never address-taken, so a plain POD
 *    pair is scalarised into registers and the frame comes out sub sp,#0x1c; the
 *    non-POD copy forces a stack home and the cartridge's sub sp,#0x24.  That is
 *    the only difference between an otherwise byte-identical body and this one.
 *
 * 2. `sa` IS A SEPARATE s64 STATEMENT, NOT AN INITIALISER.  sinA feeds two
 *    64-bit products, times 0x8000 here and times 0x4000 twelve lines down.
 *    Widening it once into its own local, ASSIGNED as a statement ahead of the
 *    first product, makes 2004/b56 compute the sign word once into a short-lived
 *    scratch and derive both high halves from it back to back (asr ...,#0x1f then
 *    lsl #0xf and lsl #0xe adjacent, the scratch dying immediately into the
 *    #0x800 rounding constant).  Every other spelling defers the second shift
 *    about twenty words into the y block and misses the whole window:
 *    `s64 sa = sinA;` at the declaration, using sa for the 0x8000 product as
 *    well, hoisting either product into a named int, reordering the stores, the
 *    six FX operand and helper forms, and five optimisation pragmas were all
 *    measured and all leave the same 27-word window.
 *
 * The two arrays are reached by raw offset, not through the header's mArray2 /
 * mArray3: a struct-array view of either one changes the frame and the loop's
 * addressing and does not reproduce.
 */
#include "dScMgSmartball_c.h"

extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern s16 data_02082214[];
}

struct TVec2 { int x; int y; ~TVec2() {} };

#define FX(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

extern "C" void func_ov006_02115248(dScMgSmartball_c *self, int *origin)
{
    char *c = (char *)self;
    int i;
    int j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 0x10; j++) {
            u8 *flag = (u8 *)(c + j + 0x4804);
            if (*flag == 1) {
                continue;
            }
            {
                int rnd1 = RandomIntInternal(&data_0209d4b8);
                int rnd2 = RandomIntInternal(&data_0209d4b8);
                int angB = ((((unsigned int)rnd2 >> 16) & 0x7fff) * 0x8000) >> 16;
                int angA = ((((unsigned int)rnd1 >> 16) & 0x7fff) * 0x20000) >> 16;
                s16 sinB = data_02082214[(angB >> 4) * 2 + 1];
                s16 sinA = data_02082214[(angA >> 4) * 2 + 1];
                s16 cosA;
                s64 sa;
                TVec2 v;
                sa = (s64)sinA;
                v.x = FX(sinB, FX(sinA, 0x8000));
                *(int *)(c + j * 8 + 0x4854) = v.x;
                cosA = data_02082214[(angA >> 4) * 2];
                v.y = FX(sinB, FX(cosA, 0x8000));
                *(int *)(c + j * 8 + 0x4858) = v.y;
                *(int *)(c + j * 8 + 0x4854) += origin[0];
                *(int *)(c + j * 8 + 0x4858) += origin[1];
                *(int *)(c + j * 8 + 0x48d4) = FX(sinB, FX(sa, 0x4000));
                *(int *)(c + j * 8 + 0x48d8) = FX(sinB, FX(cosA, 0x4000));
                *(int *)(c + j * 4 + 0x4814) = 0;
                *flag = 1;
            }
            break;
        }
    }
}
