/* func_ov007_020c0308 — fill b*32 BG map entries at dst with sequential tile
 * numbers OR'd with palette (c & 0xf) << 12. No callees.
 */

typedef unsigned int u32;
typedef unsigned short u16;

#pragma opt_strength_reduction off
void func_ov007_020c0308(u16* dst, int b, int c)
{
    u32 i = 0;
    u32 hi;
    u32 n;
    if ((u32)(b << 5) == 0)
        return;
    hi = ((u32)(c << 28)) >> 16;
    n = b << 5;
    do {
        u16 lo = i;
        dst[i] = hi | lo;
        i++;
    } while (i < n);
}
