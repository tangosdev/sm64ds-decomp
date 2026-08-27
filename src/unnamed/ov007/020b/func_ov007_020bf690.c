extern void func_ov007_020bf790(void);
extern void func_ov007_020c8498(void *o);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, unsigned short a, short b);

struct Obj6 { char _0[8]; int f8; char _c[8]; int f14; char _18[4]; int f1c; };
struct Sub { char _0[0x40]; int f40; char _44[4]; int f48; char _4c[0x48]; int f94; };
struct G { struct Obj6 *obj6; struct Sub *sub; int flags; char _c[4]; int f10; };
extern struct G *data_ov007_02104bd8;

void func_ov007_020bf690(void)
{
    struct G *g = data_ov007_02104bd8;
    struct Obj6 *obj6 = g->obj6;
    int flags = g->flags;
    int did = 0;
    if (g->f10 != 0) {
        func_ov007_020bf790();
        data_ov007_02104bd8->f10 = did;
        did = 1;
    }
    func_ov007_020c8498(obj6);
    if ((flags & 1) == 0) return;
    int v = obj6->f14;
    if (did == 0 && (flags & 2) != 0) {
        _ZN3G2x18SetBlendBrightnessEPVtts((void*)0x4000050, 0x3f, 0);
    }
    if (obj6->f1c != 0) v = 0x1000 - v;
    data_ov007_02104bd8->sub->f48 = v;
    data_ov007_02104bd8->sub->f40 = data_ov007_02104bd8->sub->f48;
    if (obj6->f8 != 3) return;
    data_ov007_02104bd8->sub->f94 = 0;
    data_ov007_02104bd8->flags &= ~1;
    if (obj6->f1c == 0) return;
    _ZN3G2x18SetBlendBrightnessEPVtts((void*)0x4000050, 0x3f, -0x10);
}
