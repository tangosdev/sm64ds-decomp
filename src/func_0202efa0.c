/* func_0202efa0 at 0x0202efa0 (arm9 main)
 *
 * Builds a 192-entry (one per scanline) 4-byte-per-line blend/fade gradient
 * table into one of two 0x300-byte double buffers at data_0209f648, selected
 * by the flip flag data_0209f608 (writes the new index to data_0209f60c).
 * The table is first filled with the neutral pattern 0x80807F7F, then four
 * successive ramp segments (slopes +0x53 / -0x160 / +0x53 / +0x53&+0x160)
 * write clamped 0..0x7F coefficients and their 0x100-complements per line,
 * positioned by a 32.32 fixed-point cursor stepped by 2^32/duration
 * (duration = *(s32*)(self+0x1c)). Finally the whole 0x600 region is cache
 * flushed. Looks like a per-scanline capture/blend wipe effect table.
 */
typedef unsigned char u8;
typedef unsigned int u32;
typedef long long s64;

extern void MultiStore_Int(int val, int *dst, int len);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);

extern volatile u32 data_0209f608;
extern volatile u32 data_0209f60c;
extern u8 data_0209f648[];

#pragma opt_strength_reduction off
void func_0202efa0(char *self)
{
    volatile int fill;
    int idx;
    int i;
    int pos;
    int step;
    u8 *buf;
    int dur;
    int v, w;
    int a, b;
    u8 *p;
    s64 d64;

    idx = (data_0209f608 == 0) ? 1 : 0;
    buf = data_0209f648 + idx * 0x300;
    fill = 0x80807F7F;
    i = 0;
    MultiStore_Int(fill, (int *)buf, 0x300);

    dur = *(int *)(self + 0x1c);
    if (dur > 0) {
        pos = (int)(-0x6000000000LL / dur) + 0x100000;
        d64 = dur;
        step = (int)(0x100000000LL / d64);
        while (pos < 0) {
            if (i >= 0xC0) break;
            pos += step;
            i++;
        }

        v = (int)(d64 * ((s64)pos * 0x532DLL) / 0x10000000000LL);
        while (pos < 0xB0E00) {
            if (i >= 0xC0) break;
            v += 0x53;
            a = (0x8000 - v) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            p = buf + i * 4;
            p[3] = a;
            buf[i * 4] = 0x100 - p[3];
            pos += step;
            i++;
        }

        v = (int)((s64)(*(int *)(self + 0x1c)) * 0x0F37292CLL / 0x100000000LL);
        while (pos < 0x11E200) {
            if (i >= 0xC0) break;
            v -= 0x160;
            a = (0x8000 - v) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            p = buf + i * 4;
            p[3] = a;
            buf[i * 4] = 0x100 - p[3];
            pos += step;
            i++;
        }

        while (pos < 0x161D00) {
            if (i >= 0xC0) break;
            v += 0x53;
            a = (0x8000 - v) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            p = buf + i * 4;
            p[3] = a;
            buf[i * 4] = 0x100 - p[3];
            pos += step;
            i++;
        }

        w = 0;
        while (pos < 0x1CF200) {
            if (i >= 0xC0) break;
            v += 0x53;
            w += 0x160;
            a = (0x8000 - v) / 256;
            b = (0x8000 - w) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            if (b < a) b = a;
            if (b > 0x7F) b = 0x7F;
            p = buf + i * 4;
            p[3] = a;
            p[2] = b;
            p[1] = 0x100 - p[2];
            buf[i * 4] = 0x100 - p[3];
            pos += step;
            i++;
        }
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209f648, 0x600);
    data_0209f60c = idx;
}
