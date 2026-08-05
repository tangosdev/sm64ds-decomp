//cpp
namespace cstd { int div(int, int); }

extern "C" {
extern int func_ov007_020c5014(void *, int);
extern void func_ov007_020bdbcc(int, int);
}

struct BA0 { int f0; char pad4[0x18 - 4]; int f18; char pad1c[0x34 - 0x18 - 4]; int f34; int f38; };
struct B9C_inner { char pad[0x50]; int f50; };
struct B9C { char pad4[4]; B9C_inner *f4; };

extern BA0 *data_ov007_02104ba0;
extern B9C *data_ov007_02104b9c;

extern "C" int func_ov007_020bbd24(int param)
{
    int r4;
    int r5;

    if (param == 0)
        r4 = data_ov007_02104ba0->f38;
    else
        r4 = 0x168;

    r4 = cstd::div(data_ov007_02104ba0->f18 << 0xc, r4);
    if (r4 < 0x1000)
        r4 = 0x1000;

    r5 = func_ov007_020c5014((void *)data_ov007_02104ba0->f0, data_ov007_02104ba0->f34 >> 0xc);
    if (r5 == 1) {
        data_ov007_02104ba0->f34 = 0;
    } else if (r5 == 0) {
        int *fp = (int *)(((int)data_ov007_02104ba0 + 0x34));
        *fp += r4;
    }

    func_ov007_020bdbcc(4, 4);

    switch (param) {
    case 0: {
        int *q = (int *)(((int)&data_ov007_02104b9c->f4->f50));
        int v = *q;
        if (v <= 0x5000) {
            *q = v + 0x7d0;
            break;
        }
        if (v <= 0x14000) {
            *q = v + 0x190;
            break;
        }
        if (v <= 0x1f000) {
            *q = v + 0xbb8;
            break;
        }
        *q = 0x1f000;
        return r5 == 2;
    }
    case 1:
        return r5 == 2;
    }
    return 0;
}
