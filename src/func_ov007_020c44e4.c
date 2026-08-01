#include "types.h"
// @symbol func_ov007_020c44e4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))))



int func_ov007_020c44e4(int count)
{
    char* p = (char*)func_ov007_020c3df4(0, 0x88);
    int i;
    u16 zs[3];

    *(s16*)(p + 0x5a) = 0;
    *(s16*)(p + 0x58) = (s16)count;
    *(int**)(p + 0x5c) = (int*)func_ov007_020c3df4(0, count << 2);
    for (i = 0; i < count; i++) {
        (*(int**)(p + 0x5c))[i] = 0;
    }

    *(int*)(p) = 0;
    *(u16*)(p + 0x62) = 1;
    *(u16*)(p + 0x60) = *(u16*)(p + 0x62);
    *(int*)(p + 8) = 0;
    *(int*)(p + 4) = *(int*)(p + 8);
    *(u16*)(p + 0xe) = 0;
    *(u16*)(p + 0xc) = *(u16*)(p + 0xe);

    {
        struct Vector3 z3 = {0, 0, 0};
        *(struct Vector3*)(p + 0x14) = z3;

    *(int*)(p + 0x24) = 0;
    *(int*)(p + 0x20) = *(int*)(p + 0x24);
    *(int*)(p + 0x40) = 0x1000;
    *(int*)(p + 0x3c) = *(int*)(p + 0x40);
    *(u16*)(p + 0x38) = 0;
    *(u16*)(p + 0x36) = *(u16*)(p + 0x38);
    *(u16*)(p + 0x34) = *(u16*)(p + 0x36);

        {
            struct Vector3 z3b = {0, 0, 0};
            *(struct Vector3*)(p + 0x28) = z3b;
        }
    }

    *(u16*)(p + 0x4c) = 0x7fff;
    *(u16*)(p + 0x4a) = *(u16*)(p + 0x4c);
    *(u16*)(p + 0x48) = *(u16*)(p + 0x4a);
    *(u16*)(p + 0x46) = *(u16*)(p + 0x48);
    *(u16*)(p + 0x44) = 0x7fff;
    *(u8*)(p + 0x4e) = 0x23;
    *(int*)(p + 0x50) = 0x1f000;
    *(int*)(p + 0x64) = 3;
    *(int*)(p + 0x68) = 0;
    *(int*)(p + 0x6c) = 0;
    *(s16*)(p + 0x72) = 0;
    *(s16*)(p + 0x70) = *(s16*)(p + 0x72);
    *(int*)(p + 0x54) = 0;
    *(int*)(p + 0x78) = 0x1000;
    *(int*)(p + 0x74) = *(int*)(p + 0x78);
    *(u8*)(p + 0x7c) = 0x19;
    *(u8*)(p + 0x7d) = 0x24;

    {
        u16 *zw = (u16 *)AT(zs, 0);
        zw[0] = 0;
        zw[1] = 0;
        zw[2] = 0;
    }
    {
        u16 a = ((volatile u16 *)zs)[0];
        u16 b = ((volatile u16 *)zs)[1];
        *(u16*)(p + 0x7e) = a;
        *(u16*)(p + 0x80) = b;
        *(u16*)(p + 0x82) = zs[2];
    }
    *(int*)(p + 0x84) = 0;
    return (int)p;
}
