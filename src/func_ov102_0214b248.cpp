//cpp
// @symbol func_ov102_0214b248
#include "decl_common.h"
#include "common.h"

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void func_0203568c(int *p, int v);
extern void func_02035684(int *p, int v);
extern void func_ov002_020ef228(void *c, int arg);
extern int func_ov102_0214b128(void *c);
}

namespace Sound { unsigned int PlayLong(unsigned int, unsigned int, unsigned int, Vector3 const &, short); }

extern "C" int func_ov102_0214b248(void *c)
{
    if (DecIfAbove0_Short((unsigned short *)((char *)c + 0x3ea))) {
        *(unsigned int *)((char *)c + 0x3e4) = Sound::PlayLong(
            *(unsigned int *)((char *)c + 0x3e4), 3, 0x188, *(Vector3 *)((char *)c + 0x74), 0);
    }

    {
        unsigned short st = *(unsigned short *)((char *)c + 0x3ea);
        if (st != 0 && st <= 4) {
            *(int *)(((int)c + 0x128)) &= ~0x8000;
            if (*(unsigned short *)((char *)c + 0x3ea) == 1) {
                func_ov102_0214ae1c(c);
                return 0;
            }
            {
                int v = ((5 - *(unsigned short *)((char *)c + 0x3ea)) << 12) / 4 + 0x1000;
                *(int *)((char *)c + 0x88) = v;
                *(int *)((char *)c + 0x84) = *(int *)((char *)c + 0x88);
                *(int *)((char *)c + 0x80) = *(int *)((char *)c + 0x84);
                *(int *)((char *)c + 0x114) = v * 0x3c;
                *(int *)((char *)c + 0x118) = v * 0x50;
                func_0203568c((int *)((char *)c + 0x144), v * 0x3c);
                func_02035684((int *)((char *)c + 0x144), v * 0x3c);
                func_ov002_020ef228((char *)c + 0x144, (int)c);
            }
            {
                int *f = (int *)(((long long)(int)((char *)c + 0x128)));
                *f &= ~4;
                if (*(unsigned short *)((char *)c + 0x3ea) == 2) {
                    *f |= 0x4000;
                }
            }
        } else {
            func_ov102_0214b128(c);
        }
    }
    return 1;
}
