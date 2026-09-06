/* func_ov006_020dbe9c -- dScMgCoin_c, ov006 0x020dbe9c, 0xe0 bytes.
 *
 * Draws the hand cursor while the stylus is down: the sine table entry for the
 * scene's current angle becomes a 2x2 rotation matrix [cos, sin; -sin, cos] at
 * unit scale, and that plus the touch position goes to the shared sprite call.
 *
 * THE FOUR MATRIX WORDS ARE WRITTEN AS ANONYMOUS EXPRESSIONS, IN SLOT ORDER,
 * AFTER THE ICON INDEX.  That is the whole match and it took the run's widest
 * sweep to find.  Two rigid regimes exist here: with `#pragma opt_propagation
 * off` the registers are the cartridge's but the two 64-bit values complete in
 * the wrong order, and without it they complete in the right order but the sine
 * table's base colours to ip where the cartridge uses r4.  Neither regime moved
 * under 28 statement orders, 6 multiply spellings, 12 table-pointer forms or 39
 * pragma combinations, because every one of those kept a named `rx` / `ry` pair.
 * Deleting the pair -- storing straight into vec[] -- collapses both regimes
 * onto the cartridge's schedule, and then the icon index has to be read FIRST:
 * moving that one line below any of the four stores costs fifteen words.
 *
 * The unit scale is written as a multiply, not a shift, because that is what
 * FX_MUL is; 2004/b56 strength-reduces it to the 64-bit shift either way and the
 * bytes are identical for `* 0x1000` and `<< 12`.
 */
#pragma opt_propagation off

typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef long long s64;

extern s16 data_02082214[];
extern void *data_ov006_02134b4c[];
extern void func_ov004_020b023c(void *obj, int x, int y, int w, int *vec);

#define FX_MUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

void func_ov006_020dbe9c(char *c)
{
    char *s = c + 0x5000;

    if (*(u8 *)(s + 0x1bd) == 0)
        return;
    {
        u16 idx_h = *(u16 *)(c + 0x5100 + 0xb8);
        s32 xr = *(s32 *)(s + 0x1a8);
        s32 yr = *(s32 *)(s + 0x1ac);
        int i = (idx_h >> 4) * 2;
        int vec[4];
        u8 idx_l = *(u8 *)(s + 0x1be);

        vec[0] = FX_MUL(data_02082214[i + 1], 0x1000);
        vec[1] = FX_MUL(data_02082214[i], 0x1000);
        vec[2] = -FX_MUL(data_02082214[i], 0x1000);
        vec[3] = FX_MUL(data_02082214[i + 1], 0x1000);
        func_ov004_020b023c(data_ov006_02134b4c[idx_l], xr >> 12, yr >> 12, -1, vec);
    }
}
