//cpp
// @symbol func_ov102_0214b248
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
#include "common.h"
extern "C" {
    extern void func_ov102_0214ae1c(void*);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void func_0203568c(int *p, int v);
extern void func_02035684(int *p, int v);
extern void func_ov002_020ef228(void *c, int arg);
extern int func_ov102_0214b128(void *c);
}

namespace Sound { unsigned int PlayLong(unsigned int, unsigned int, unsigned int, Vector3 const &, short); }

extern "C" int func_ov102_0214b248(char *c)
{
    if (DecIfAbove0_Short((unsigned short *)(c + 0x3ea))) {
        *(unsigned int *)(c + 0x3e4) = Sound::PlayLong(
            *(unsigned int *)(c + 0x3e4), 3, 0x188, *(Vector3 *)(c + 0x74), 0);
    }

    {
        unsigned short st = *(unsigned short *)(c + 0x3ea);
        if (st != 0 && st <= 4) {
            *(int *)(((int)c + 0x128)) &= ~0x8000;
            if (*(unsigned short *)(c + 0x3ea) == 1) {
                func_ov102_0214ae1c(c);
                return 0;
            }
            {
                int v = ((5 - *(unsigned short *)(c + 0x3ea)) << 12) / 4 + 0x1000;
                *(int *)(c + 0x88) = v;
                *(int *)(c + 0x84) = *(int *)(c + 0x88);
                *(int *)(c + 0x80) = *(int *)(c + 0x84);
                *(int *)(c + 0x114) = v * 0x3c;
                *(int *)(c + 0x118) = v * 0x50;
                func_0203568c((int *)(c + 0x144), v * 0x3c);
                func_02035684((int *)(c + 0x144), v * 0x3c);
                func_ov002_020ef228(c + 0x144, (int)c);
            }
            {
                int *f = (int *)(((long long)(int)(c + 0x128)));
                *f &= ~4;
                if (*(unsigned short *)(c + 0x3ea) == 2) {
                    *f |= 0x4000;
                }
            }
        } else {
            func_ov102_0214b128(c);
        }
    }
    return 1;
}
