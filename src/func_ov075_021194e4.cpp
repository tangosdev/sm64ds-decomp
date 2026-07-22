//cpp
typedef int Fix12;

struct OamAttr {
    unsigned short attr0, attr1, attr2, attr3;
};

struct OAM {
    static int Render(bool, OamAttr*, int, int, int, int, Fix12, int);
    static void RenderSub(OamAttr*, int, int);
};

extern "C" int func_0200f0bc(void);
extern OamAttr* data_0208a0f8[];

struct S4 {
    short a, b;
};

extern S4 data_02082214[];
extern int data_0208ee44;

extern "C" void func_ov075_021194e4(char* c)
{
    OamAttr* attr;

    attr = data_0208a0f8[func_0200f0bc()];

    if (*(int*)(c + 0x270) <= 0) {
        int ang;
        int flag;
        int fallback = 0x60;
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

            OAM::Render(one, attr, x, y, minus, minus, fx, zero);

            if (attr->attr3 == 0xffff)
                break;

            attr += 1;
            ang -= 0x2000;
        } while (1);

        *(int*)(((long long)(int)(c + 0x26c)) & 0xFFFFFFFFFFFFFFFFLL) += 0x800;

        if (flag == zero)
            *(int*)(c + 0x270) = 0x78;
    } else {
        OAM::RenderSub(attr, 0x80, 0x60);
        *(int*)(((long long)((int)c + 0x270)) & 0xFFFFFFFFFFFFFFFFLL) -= data_0208ee44;
        *(int*)(c + 0x26c) = 0;
    }
}
