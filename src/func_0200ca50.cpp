//cpp
typedef short s16;
typedef unsigned char u8;

class C;
typedef int (C::*PMF)();

extern "C" void _Z15ApproachLinear2Rsss(s16 &dst, s16 a, s16 b);
extern "C" int func_0200cbe0(C *c);

class C { public: int dummy; };

extern "C" int func_0200ca50(C *self)
{
    u8 *f = (u8 *)self;
    int flags;
    int r5;
    {
        void *p = *(void **)(f + 0x13c);
        _Z15ApproachLinear2Rsss(*(s16 *)(f + 0x17a), *(s16 *)((u8 *)p + 0x24), 0x80);
    }
    r5 = 1;
    flags = *(int *)(f + 0x154);
    if (flags & 8) {
        ;
    } else if (flags & 0x8000) {
        func_0200cbe0(self);
    } else {
        u8 *obj = *(u8 **)(f + 0x138);
        if (*(int *)(obj + 8) != 0) {
            PMF *pp = (PMF *)(obj + 8);
            r5 = (self->**pp)();
        }
    }
    return r5;
}
