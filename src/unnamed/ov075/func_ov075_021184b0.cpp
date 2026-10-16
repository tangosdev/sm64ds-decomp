//cpp
/* Leftover: the ten OamAttr lists the tables below index
 * (data_ov075_0211cc64 .. data_ov075_0211d2ac) stay the ROM copy. A TU's
 * .data emits as one contribution and the delinks entry claims one
 * contiguous range per section, so this file's .data slot is the two
 * tables at 0x0211c9e0; the lists sit scattered across a shared .data
 * region pooled with other screens' lists and cannot join it. The tail
 * of data_ov075_0211d2ac is _ZTV10dScEntry_c's header, and _ZTI/_ZTS
 * 10dScEntry_c are emitted by the TU that owns the vtable, which this
 * file is not. */
#include "types.h"
#include "OamAttr.h"
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, OamAttr*, int, int, int, int, int, int, int, int);
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int, OamAttr*, int, int, int, int, int, int);
extern "C" int func_0203da9c(void);
extern "C" void func_ov075_0211621c(char *sl);
extern "C" int func_0200f0bc(void);
extern "C" void func_0202048c(void *dst, int a, int b);
extern u8 data_0209b2f0[];
extern OamAttr data_ov075_0211cb84;
extern OamAttr data_ov075_0211cc8c[];
extern OamAttr data_ov075_0211d04c[];
extern OamAttr data_ov075_0211d104[];
extern OamAttr data_ov075_0211ccb4[];
extern OamAttr data_ov075_0211d264[];
extern OamAttr data_ov075_0211cc64[];
extern OamAttr data_ov075_0211d014[];
extern OamAttr data_ov075_0211d0c4[];
extern OamAttr data_ov075_0211cda4[];
extern OamAttr data_ov075_0211d2ac[];

/* Indexed by func_0200f0bc. Each entry is one OAM list. */
OamAttr *data_ov075_0211c9e0[5] = {
    data_ov075_0211cc8c,
    data_ov075_0211d04c,
    data_ov075_0211d104,
    data_ov075_0211ccb4,
    data_ov075_0211d264,
};

OamAttr *data_ov075_0211c9f4[5] = {
    data_ov075_0211cc64,
    data_ov075_0211d014,
    data_ov075_0211d0c4,
    data_ov075_0211cda4,
    data_ov075_0211d2ac,
};

#define LADR(p) ((int *)(unsigned int)(p))

extern "C" void func_ov075_021184b0(char *sl)
{
    int id = func_0203da9c();
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(false, &data_ov075_0211cb84, 0x80, 0x20, -1, -1, 0x1000, 0x1000, 0, -1);
    func_ov075_0211621c(sl);

    u8 *p = data_0209b2f0 + id;
    OamAttr *sb;
    if (*p != 0)
        sb = data_ov075_0211c9f4[func_0200f0bc()];
    else
        sb = data_ov075_0211c9e0[func_0200f0bc()];

    {
        int i = 0;
        int x = 0x80;
        int y = 0x60;
        do {
            int fix = (i == *(int *)(sl + 0x26c)) ? 0xe00 : 0x1000;
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(true, sb, x, y, -1, -1, fix, 0);
            if (sb->attr3 == 0xffff)
                break;
            sb++;
            i++;
        } while (1);

        {
            int *q = LADR(sl + 0x268);
            *q = *q + 1;
            if (*(int *)(sl + 0x268) >= 3) {
                *(int *)(sl + 0x268) = 0;
                {
                    int *r = LADR(sl + 0x26c);
                    *r = *r + 1;
                    if (*(int *)(sl + 0x26c) > i)
                        *(int *)(sl + 0x26c) = 0;
                }
            }
        }
    }

    if (*p != 0) {
        func_0202048c(sl + 0x20c, 0x80, 0x88);
        func_0202048c(sl + 0x1b4, 0x80, 0x88);
    } else {
        func_0202048c(sl + 0x1b4, 0x80, 0x88);
        func_0202048c(sl + 0x238, 0x6c, 0x6e);
        func_0202048c(sl + 0x20c, 0xa6, 0x6e);
    }
    func_0202048c(sl + 0x1e0, 0x80, 0x88);
}
