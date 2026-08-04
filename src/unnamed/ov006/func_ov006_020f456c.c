#include "types.h"
typedef struct Car {
    u8 pad00[0x13];
    u8 b13;
    u8 b14;
    u8 pad15[3];
} Car;

typedef struct Ctx {
    u8 pad0000[0xa8];
    s32 fa8;
    u8 pad00ac[0x51a8 - 0xac];
    Car cars[12];
    u8 pad52c8[0x5314 - 0x52c8];
    s32 f5314;
    u8 pad5318[0x5322 - 0x5318];
    u16 h5322;
    u8 pad5324[0x533c - 0x5324];
    u8 b533c;
    u8 b533d;
} Ctx;

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];

extern void func_ov006_020f3f10(Ctx *c);
extern void func_ov006_020f3964(Ctx *c);
extern unsigned int func_02012790(unsigned int a);
extern void func_ov006_020f39fc(Ctx *c);

void func_ov006_020f456c(Ctx *c)
{
    s32 i;
    s32 j;
    s32 k;
    s32 j2;
    s32 k2;
    s32 ok;
    u16 *p;

    func_ov006_020f3f10(c);
    func_ov006_020f3964(c);

    if (c->h5322 != 0)
    {
        p = (u16 *)(int)(((long long)(int)((char *)c + 0x5322)));
        *p = (u16)(*p - 1);
        return;
    }

    i = data_020a0e40;
    ok = 0;
    if (data_020a0de8[i * 4] != 0)
    {
        if (data_020a0de9[i * 4] != 0)
        {
            ok = 1;
        }
    }
    if (ok == 0)
    {
        return;
    }

    func_02012790(0x62);

    if (c->fa8 == 0)
    {
        if (c->b533c == 1)
        {
            for (j = 0; j < 2; j++)
            {
                for (k = 0; k < 5; k++)
                {
                    if (c->cars[j * 5 + k + 2].b13 != 0)
                    {
                        c->cars[j * 5 + k + 2].b14 = 6;
                        break;
                    }
                }
            }
        }
        else
        {
            for (j2 = 0; j2 < 3; j2++)
            {
                for (k2 = 0; k2 < 4; k2++)
                {
                    if (c->cars[j2 * 4 + k2].b13 != 0)
                    {
                        c->cars[j2 * 4 + k2].b14 = 6;
                        break;
                    }
                }
            }
        }
        c->b533d = 0;
    }

    func_ov006_020f39fc(c);
    c->f5314 = 4;
}
