#include "types.h"
typedef struct Foo {
    int w0;
    int w4;
    int w8;
    int wc;
} Foo;

typedef struct Ctx {
    char pad[0x44];
    Foo* foo;
    char pad2[0x40];
} Ctx;

extern int* func_ov007_020c0aa8(int a, int b, int c, u16 d, u16 e);
extern void func_ov007_020c9a2c(Ctx* ctx, void* a);

int* func_ov007_020c95fc(void* a, int b, int c)
{
    Foo f;
    Ctx ctx;
    Foo* p;

    if (a == 0) {
        return func_ov007_020c0aa8(0, b, c, 0x20, 0x18);
    }
    func_ov007_020c9a2c(&ctx, a);
    p = ctx.foo;
    f = *p;
    return func_ov007_020c0aa8((int)((char*)p + 0x10), b, c, (u16)f.w0, (u16)f.w4);
}
