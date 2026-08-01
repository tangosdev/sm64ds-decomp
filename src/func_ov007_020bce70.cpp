//cpp
#include "types.h"
struct Sub {
    char pad2[2];
    short field2;
    int field4;
    char pad8[4];
    int fieldc;
};
struct S {
    char pad[0x40];
    Sub* f40;
    char* f44;
    char* f48;
    void** f4c;
};
extern S* data_ov007_0210342c;
extern char data_ov007_02102f28[];

extern "C" void func_ov007_020c92d0(Sub* s);
extern void func_ov007_020c940c(void* s);
namespace cstd { int div(int, int); }
extern void func_ov007_020c93c4(void* a, char* b, char* c, short* d);

extern "C" void func_ov007_020bce70(short n, int m)
{
    int t;

    data_ov007_0210342c->f40->field2 = n;
    func_ov007_020c92d0(data_ov007_0210342c->f40);

    t = data_ov007_0210342c->f40->fieldc;
    if (t == 0) {
        *data_ov007_0210342c->f4c = data_ov007_02102f28 + (n << 6);
        func_ov007_020c940c(data_ov007_0210342c->f4c);
        if (m == 0) {
            t = m + 1;
            func_ov007_020c940c(data_ov007_0210342c->f44);
        } else {
            func_ov007_020c940c(data_ov007_0210342c->f48);
        }
    }

    if (t >= 0 && t <= m) {
        int val;
        if (t <= 0)
            val = 0;
        else if (t >= m)
            val = 0x1000;
        else
            val = cstd::div(t << 12, m);
        func_ov007_020c93c4((void*)val, data_ov007_0210342c->f48,
                            (char*)data_ov007_0210342c->f4c, (short*)data_ov007_0210342c->f44);
        data_ov007_0210342c->f40->field4 = 1;
    } else {
        if (data_ov007_0210342c->f40->field4 != 0)
            data_ov007_0210342c->f40->field4 = 0;
    }
}
