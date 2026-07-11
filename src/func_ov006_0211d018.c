typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned char u8;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov006_0211d018(char *base, int idx)
{
    int off = idx * 0x1c;
    char *bo = base + off;
    if (*(u8 *)(bo + 0x4be5) != 0) {
        *(u8 *)(bo + 0x4be1) = 6;
        return;
    }
    {
        u16 v = *(u16 *)((char *)AT(base, 0x4bde) + off);
        u16 *p = (u16 *)AT((char *)AT(base, 0x4bde), off);
        if (v != 0) {
            *p = v - 1;
            return;
        }
    }
    *(u8 *)(bo + 0x4be2) = 1;
    if ((*(s32 *)(base + 0x4bcc + off) >> 0xc) < 0) {
        *(u8 *)(bo + 0x4be4) = 1;
        *(s32 *)(base + 0x4bcc + off) = -0x80000;
        *(s32 *)(bo + 0x4bd4) = 0x4800;
        *(u8 *)(bo + 0x4be1) = 3;
        return;
    }
    *(u8 *)(bo + 0x4be4) = 0;
    *(s32 *)(base + 0x4bcc + off) = 0x180000;
    *(s32 *)(bo + 0x4bd4) = -0x4800;
    *(u8 *)(bo + 0x4be1) = 2;
}
