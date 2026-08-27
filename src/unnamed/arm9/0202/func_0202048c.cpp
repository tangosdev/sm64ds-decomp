//cpp
extern "C" {
struct Matrix2x2;
}
typedef int Fix12i;

extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int show, void* attr, int a, int b, int c, int d, Fix12i e, Fix12i f, int g, int h);
extern "C" int _ZN3OAM9RenderSubEP7OamAttriiii(void* attr, int a, int b, int c, int d);
extern "C" int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int show, void* attr, int a, int b, int c, int d, Matrix2x2* m);

extern short data_02082214[];

extern "C" void func_0202048c(char* a0, int a1, int a2)
{
    if (*(unsigned char*)(a0 + 0x26) == 0) return;
    if (*(void**)(a0 + 4) == 0) return;

    int f08 = *(int*)(a0 + 8);
    if (f08 == 0x1000 && *(int*)(a0 + 0xc) == 0x1000 && *(unsigned short*)(a0 + 0x20) == 0) {
        int mode = *(int*)(a0 + 0x28);
        if (mode == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, *(void**)(a0 + 4), a1, a2 + 0xc0,
                *(signed char*)(a0 + 0x22), *(signed char*)(a0 + 0x23), 0x1000, 0x1000, 0, -1);
            _ZN3OAM9RenderSubEP7OamAttriiii(*(void**)(a0 + 4), a1, a2,
                *(signed char*)(a0 + 0x22), *(signed char*)(a0 + 0x23));
            return;
        }
        if (mode == 1) {
            _ZN3OAM9RenderSubEP7OamAttriiii(*(void**)(a0 + 4), a1, a2,
                *(signed char*)(a0 + 0x22), *(signed char*)(a0 + 0x23));
            return;
        }
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, *(void**)(a0 + 4), a1, a2,
            *(signed char*)(a0 + 0x22), *(signed char*)(a0 + 0x23), 0x1000, 0x1000, 0, -1);
        return;
    }

    int idx = (*(unsigned short*)(a0 + 0x20)) >> 4;
    int f0c = *(int*)(a0 + 0xc);
    int i0 = idx * 2;
    short tb = data_02082214[i0 + 1];
    short ta = data_02082214[i0];

    int m[4];
    m[0] = (tb * f08 + 0x800) >> 12;
    m[1] = (ta * f08 + 0x800) >> 12;
    m[2] = -((ta * f0c + 0x800) >> 12);
    m[3] = (tb * f0c + 0x800) >> 12;

    if (*(int*)*(void**)(a0 + 4) & 0x10000000) {
        m[0] = -m[0];
        m[1] = -m[1];
    }
    if (*(int*)*(void**)(a0 + 4) & 0x20000000) {
        m[2] = -m[2];
        m[3] = -m[3];
    }

    int mode = *(int*)(a0 + 0x28);
    if (mode == 2) {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, *(void**)(a0 + 4), a1, a2 + 0xc0,
            *(signed char*)(a0 + 0x22), *(signed char*)(a0 + 0x23), (Matrix2x2*)m);
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, *(void**)(a0 + 4), a1, a2,
            *(signed char*)(a0 + 0x22), *(signed char*)(a0 + 0x23), (Matrix2x2*)m);
        return;
    }
    if (mode == 1) {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, *(void**)(a0 + 4), a1, a2,
            *(signed char*)(a0 + 0x22), *(signed char*)(a0 + 0x23), (Matrix2x2*)m);
        return;
    }
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, *(void**)(a0 + 4), a1, a2,
        *(signed char*)(a0 + 0x22), *(signed char*)(a0 + 0x23), (Matrix2x2*)m);
}
