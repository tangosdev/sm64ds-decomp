//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "cstd.h"

extern "C" {

extern void func_ov007_020b2614(int a, int b, int c);

struct Sub { char pad2[2]; s16 field2; int field4; char pad8[4]; int fieldc; };
struct S {
    char pad[8];
    struct Sub* f8;
    struct Sub* fc;
    char pad10[0xdc];
    int fec;
    char padf0[8];
    int ff8;
};
extern struct S* data_ov007_0210342c;

void func_ov007_020b1b74(void)
{
    int r4 = data_ov007_0210342c->f8->fieldc;
    if (r4 == 0)
        func_ov007_020b2614(0, 0, -1);

    if (r4 == 0xa)
        data_ov007_0210342c->fc->field2 = 3;

    if (r4 >= 0 && r4 <= 0xa) {
        int val;
        int sq;
        if (r4 <= 0)
            val = 0;
        else if (r4 >= 0xa)
            val = 0x1000;
        else
            val = cstd::div(r4 << 12, 0xa);
        sq = (int)(((long long)val * val) >> 12);
        data_ov007_0210342c->ff8 = FX_MUL(sq, 0x17d0);
    } else if (r4 <= 0x1e) {
        int val;
        int t;
        int sq;
        if (r4 <= 0xa)
            val = 0;
        else if (r4 >= 0x1e)
            val = 0x1000;
        else
            val = cstd::div((r4 - 0xa) << 12, 0x14);
        t = 0x1000 - val;
        sq = (int)(((long long)t * t) >> 12);
        data_ov007_0210342c->ff8 = FX_MUL(sq, 0x7d0) + 0x1000;
    } else {
        data_ov007_0210342c->f8->field2 = 0;
    }

    if (data_ov007_0210342c->fec == 1)
        data_ov007_0210342c->ff8 = 0x1000;
}
}
