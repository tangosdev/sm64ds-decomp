//cpp
typedef int Fix12;

#include "OamAttr.h"

struct OAM {
    static int Render(bool, OamAttr*, int, int, int, int, Fix12, int);
    static void RenderSub(OamAttr*, int, int);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(bool, OamAttr*, int, int, int, int, Fix12, int);


extern "C" int func_0200f0bc(void);
extern OamAttr* data_ov075_0211c968[];

struct S4 {
    short a, b;
};

extern S4 data_02082214[];
extern int data_0208ee44;

extern "C" void func_ov075_021199d8(char* c)
{
    OamAttr* attr;

    attr = data_ov075_0211c968[func_0200f0bc()];

    if (*(int*)(c + 0x270) <= 0) {
        int ang;
        int flag;
        int fallback = 0x1c;
        int zero = 0;
        int one = 1;
        int minus = -1;
        int fx = 0x1000;
        int x = 0x80;

        ang = *(int*)(c + 0x26c);
        flag = 0;

        do {
            int y;

            if (ang >= 0 && ang < 0x10000) {
                flag = one;
                y = (data_02082214[(unsigned short)(short)(unsigned short)ang >> 4].a >> 0xa) + fallback;
            } else {
                y = fallback;
            }

            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(one, attr, x, y, minus, minus, fx, zero);

            if (attr->attr3 == 0xffff)
                break;

            attr += 1;
            ang -= 0x2000;
        } while (1);

        *(int*)(c + 0x26c) += 0x800;

        if (flag == zero)
            *(int*)(c + 0x270) = 0x78;
    } else {
        OAM::RenderSub(attr, 0x80, 0x1c);
        *(int*)(c + 0x270) -=
            data_0208ee44;
        *(int*)(c + 0x26c) = 0;
    }
}
