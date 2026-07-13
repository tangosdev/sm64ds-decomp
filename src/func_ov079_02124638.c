typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
extern s16 SINE_TABLE[];

void func_ov079_02124638(char* obj)
{
    u8 old = *(u8 *)(obj + 0x400);
    u8 *p = (u8 *)(((int)obj + 0x400) & 0xFFFFFFFFFFFFFFFFLL);
    u8 v = *p;
    *p = (u8)(v - 1);
    if (old != 0) {
        u32 t = *(u8 *)(obj + 0x400);
        if (t <= 0x50) {
            *(s16 *)(obj + 0x8c) = 0;
            return;
        }
        {
            s32 amp = (s32)((t - 0x50) << 6);
            u16 ang = (u16)(s16)(t << 13);
            *(s16 *)(obj + 0x8c) = (s16)(((s64)amp * SINE_TABLE[(ang >> 4) * 2] + 0x800) >> 12);
        }
        return;
    }
    *(s32 *)(obj + 0x3b0) = *(s32 *)(obj + 0x3b4);
    if (*(s32 *)(obj + 0x3b0) == 7) {
        *(s32 *)(obj + 0x3b4) = 10;
    }
    *(u8 *)(obj + 0x40b) = 0;
}
