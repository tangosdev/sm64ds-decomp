#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))&0xFFFFFFFFFFFFFFFFLL))

void func_ov006_0211d924(char* p, int i)
{
    u8 flag;

    flag = *(u8*)(p + 0x4000 + (i << 5) + 0xbbe);

    if (flag == 0) {
        *(u16*)(p + 0x4bb4 + (i << 5)) += 1;
        *(int*)(p + 0x4ba4 + (i << 5)) += *(int*)(p + 0x4000 + (i << 5) + 0xbac);
        *(int*)AT(p, 0x4bc4) += *(int*)(p + 0x4000 + (i << 5) + 0xbac);
        if (*(int*)(p + 0x4000 + (i << 5) + 0xbac) >= 0x800)
            *(int*)(p + 0x4bac + (i << 5)) -= 0x20;
        if (*(u16*)(p + 0x4b00 + (i << 5) + 0xb4) >= 0x10) {
            *(u16*)(p + 0x4b00 + (i << 5) + 0xb4) = 0;
            *(u8*)(p + 0x4bbe + (i << 5)) += 1;
            *(int*)(p + 0x4000 + (i << 5) + 0xbac) = 0;
        }
    } else if (flag == 1) {
        *(u16*)(p + 0x4bb4 + (i << 5)) += 1;
        if (*(u16*)(p + 0x4b00 + (i << 5) + 0xb4) == 0x10)
            *(int*)(p + 0x4000 + (i << 5) + 0xbac) = -0x1000;
        *(int*)(p + 0x4ba4 + (i << 5)) += *(int*)(p + 0x4000 + (i << 5) + 0xbac);
        *(int*)AT(p, 0x4bc4) += *(int*)(p + 0x4000 + (i << 5) + 0xbac);
        if (*(int*)(p + 0x4000 + (i << 5) + 0xbac) <= -0x800)
            *(int*)(p + 0x4bac + (i << 5)) += 0x20;
        if (*(u16*)(p + 0x4b00 + (i << 5) + 0xb4) >= 0x20) {
            *(u16*)(p + 0x4b00 + (i << 5) + 0xb4) = 0x20;
            *(u8*)(p + 0x4000 + (i << 5) + 0xbbe) = 0;
            *(u8*)(p + 0x4000 + (i << 5) + 0xbba) = 4;
        }
    }
}
