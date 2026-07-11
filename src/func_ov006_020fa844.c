typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef int s32;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

extern s32 data_ov006_0212eb44[];

void func_ov006_020fa844(char *self)
{
    int i;
    char *w;

    w = self;
    for (i = 0; i < 3; i++) {
        *(s32 *)(w + 0x4e58) = (data_ov006_0212eb44[i] << 12) - *(s32 *)(self + 0x5bfc);
        *(s32 *)(w + 0x4e5c) = -*(s32 *)(self + 0x5c00);
        if (*(u8 *)(w + 0x4e6e) != 0) {
            *(u16 *)AT(w, 0x4e68) -= 0x40;
            if (*(s16 *)(w + 0x4e68) <= -0x1000) {
                *(u16 *)(w + 0x4e68) = 0xf000;
                *(u8 *)(w + 0x4e6e) = 0;
            }
        } else {
            *(u16 *)AT(w, 0x4e68) += 0x40;
            if (*(s16 *)(w + 0x4e68) >= 0x1000) {
                *(s16 *)(w + 0x4e68) = 0x1000;
                *(u8 *)AT(w, 0x4e6e) += 1;
            }
        }
        w += 0x18;
    }
}
