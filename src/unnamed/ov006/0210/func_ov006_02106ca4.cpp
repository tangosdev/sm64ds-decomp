//cpp
#include "types.h"

#pragma opt_propagation off

struct C;
typedef void (C::*PMF)(int);

extern PMF data_ov006_02142840[];
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern char *data_ov004_020beb68;

extern "C" {
void func_ov006_021050bc(void *c);
void func_ov006_02104580(void *c);
void func_ov006_02104870(void *c);
void func_ov006_02104ea8(void *c);
void func_ov006_021068d8(void *c);
void func_ov004_020b0a54(int a);
void func_ov004_020adb1c(int a);
}

extern "C" void func_ov006_02106ca4(char *c)
{
    int found;
    int i;
    int n;

    func_ov006_021050bc(c);
    found = 0;
    i = found;
    n = *(int *)(c + 0x4000 + 0xcb8);
    if (n > 0) {
        do {
            u8 *p = (u8 *)(c + i + 0x4efa);
            (((C *)c)->*data_ov006_02142840[*p])(i);
            i++;
            if (*p != 0)
                found++;
            n = *(int *)(c + 0x4000 + 0xcb8);
        } while (i < n);
    }
    if (found != 0)
        return;

    if (*(u16 *)(c + 0x4e00 + 0xc0) != 0) {
        int off;
        u16 *tp;
        u8 *hi2;
        off = 0x4ec0;
        tp = (u16 *)(int)((long long)(int)(c + off) & 0xFFFFFFFFFFFFFFFFLL);
        hi2 = (u8 *)(c + 0x4000);
        *tp = *tp - 1;
        if (hi2[0xfe6] == 0) {
            int hit;
            u8 idx;
            hit = 0;
            idx = data_020a0e40;
            if (data_020a0de8[idx * 4] != 0) {
                if (data_020a0de9[idx * 4] != 0)
                    hit = 1;
            }
            if (hit)
                *(u16 *)(c + 0x4e00 + 0xc0) = 0;
        }
        if (*(s16 *)(c + 0x4e00 + 0xc0) > 0)
            return;
        *(u16 *)(c + 0x4e00 + 0xc0) = 0;
        if (*(u8 *)(c + 0x4000 + 0xfe6) != 0) {
            char *g;
            func_ov006_02104870(c);
            func_ov004_020b0a54(0);
            func_ov006_02104ea8(c);
            *(u8 *)(c + 0x4000 + 0xfe3) = 0;
            *(u8 *)(c + 0xc3) = 0;
            g = data_ov004_020beb68;
            if (g != 0) {
                if (*(int *)(g + 0xb4) < 9999)
                    (*(int *)(int)(g + 0xb4))++;
                if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                    *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
            }
            func_ov004_020adb1c(data_ov004_020beb68 != 0 ? *(int *)(data_ov004_020beb68 + 0xb4) : 0);
            return;
        }
        *(u16 *)(c + 0x4e00 + 0xc0) = 0x10;
        *(int *)(c + 0x4000 + 0xca8) = 7;
        func_ov006_021068d8(c);
        return;
    }
    func_ov006_02104580(c);
    {
        int inc;
        u16 *ip;
        inc = 0x4ec6;
        ip = (u16 *)(int)((long long)(unsigned)(c + inc) & 0xFFFFFFFFFFFFFFFFLL);
        *ip = *ip + 1;
    }
}
