//cpp
#include "types.h"
struct C;
typedef void (C::*PMF)(int);

extern PMF data_ov006_02141840[];
extern void* func_020beb68;

extern "C" void func_ov006_020dde28(char* c, int i);
extern "C" void func_ov006_020dd2cc(char* c);
extern "C" void func_ov006_020dca04(char* c);

struct C {};

extern "C" void func_ov006_020de440(char* c) {
    int i;
    char* p;
    for (i = 0, p = c; i < 0x28; i++, p += 0x1c) {
        if (*(u8*)(p + 0x4677) != 0) {
            (((C*)c)->*data_ov006_02141840[*(u8*)(p + 0x4675)])(i);
            func_ov006_020dde28(c, i);
        }
    }
    func_ov006_020dd2cc(c);
    func_ov006_020dca04(c);
    if (*(s32*)(c + 0x51c8) == 3) {
        return;
    }
    {
        int found = 0;
        int j;
        char* q;
        for (j = 0, q = c; j < 0x18; j++, q += 0x18) {
            if (*(u8*)(q + 0x4ad3) != 0) {
                if (*(u8*)(q + 0x4ad5) == 0) {
                    found++;
                    break;
                }
            }
        }
        if (found != 0) {
            return;
        }
    }
    *(s32*)(c + 0x51c8) = 3;
    *(s32*)(c + 0x51cc) = 0x40;
    *(u8*)(c + 0x51df) = 1;
    {
        int t;
        void* g = func_020beb68;
        if (g != 0) {
            t = *(s32*)((char*)g + 0xa8);
        } else {
            t = 0;
        }
        if (t > *(s32*)(c + 0x51d4)) {
            *(u8*)(c + 0x51db) = 1;
        } else {
            *(u8*)(c + 0x51db) = 0;
        }
    }
}
