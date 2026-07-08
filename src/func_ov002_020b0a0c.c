/* func_ov002_020b0a0c at 0x020b0a0c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;

extern void LoadLevel(s8 levelID, u8 entranceID, s8 starID, u32 d, s8 e);
extern u8 LEVEL_SPECIFIC_SETTING[];

struct Obj {
    char pad[8];
    u32 f8;
    char pad2[0x90 - 0xc];
    short f90;
};

void func_ov002_020b0a0c(struct Obj *self)
{
    LEVEL_SPECIFIC_SETTING[0] = (u8)self->f90;
    {
        u32 v = self->f8;
        u32 hi = v >> 0x18;
        u32 mid = (v >> 8) & 0xff;
        if (mid == 0xff) mid = -1;
        LoadLevel((s8)hi, (u8)(v >> 0x10), -1, 0, (s8)mid);
    }
}
