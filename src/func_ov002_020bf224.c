/* func_ov002_020bf224 — scale b by an s16 factor from a 0x18-stride table
 * indexed by byte data_020a0e40 (fx12 rounded multiply), clamped below by c.
 * No callees; data: data_020a0e40 (u8 index), data_0209f4a0 (s16 table).
 */

typedef unsigned char u8;
typedef unsigned int u32;
typedef signed short s16;
typedef long long s64;

struct Entry {
    s16 val;             /* 0x0 */
    char _pad[0x16];
};

extern u8 data_020a0e40;
extern struct Entry data_0209f4a0[];

int func_ov002_020bf224(int a, int b, int c)
{
    s64 p = (s64)b * data_0209f4a0[data_020a0e40].val;
    b = (int)((p + 0x800) >> 12);
    if (b < c)
        b = c;
    return b;
}
