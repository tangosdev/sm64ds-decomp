//cpp
extern "C" {
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* thiz, void* bca, int a, int b, int f, unsigned short t);
extern void func_02012790(int);
extern int data_ov075_0211b534[];
extern void* data_ov075_0211d41c[];
extern short data_02082214[];
}
static inline int* L(void* p) {
    return (int*)(int)(p);
}
extern "C" void func_ov075_02114560(char* c)
{
    char* mid;
    int* px;
    int* py;
    int* pz;
    int s, v, sc;
    unsigned short ang;
    int* p13c;
    int* p144;

    if (*(int*)(c + 0x120) < data_ov075_0211b534[*(unsigned char*)(c + 0x152)] &&
        *(unsigned char*)(c + 0x153) == 0) {
        *(int*)(c + 0x144) = 0x2a000;
        *(unsigned char*)(c + 0x153) = 1;
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c, data_ov075_0211d41c[1], 4, 0x40000000, 0x1000, 0);
        *(int*)(c + 0x114) = 0;
    }

    mid = (char*)L(c + 0x100);
    ang = *(unsigned short*)(mid + 0x50);
    v = *(int*)(c + 0x13c);
    s = data_02082214[(ang >> 4) * 2];
    px = L(c + 0x118);
    sc = (int)(((long long)v * s + 0x800) >> 12);
    *px = *px + sc;

    py = L(c + 0x11c);
    pz = L(c + 0x120);
    *py = *py + *(int*)(c + 0x144);

    ang = *(unsigned short*)(mid + 0x50);
    v = *(int*)(c + 0x13c);
    s = data_02082214[(ang >> 4) * 2 + 1];
    sc = (int)(((long long)v * s + 0x800) >> 12);
    *pz = *pz + sc;

    if (*(int*)(c + 0x11c) < 0) {
        *(int*)(c + 0x11c) = 0;
        *(int*)(c + 0x144) = 0;
        *(unsigned char*)(c + 0x153) = 0;
    }

    if (*(unsigned char*)(c + 0x153) == 0) {
        p13c = L(c + 0x13c);
        *p13c = *p13c + 0x2000;
        if (*(int*)(c + 0x13c) > *(int*)(c + 0x140))
            *(int*)(c + 0x13c) = *(int*)(c + 0x140);
    }

    p144 = L(c + 0x144);
    *p144 = *p144 - 0x4000;
    if (*(int*)(c + 0x144) < -0x1e000)
        *(int*)(c + 0x144) = -0x1e000;

    if (*(int*)(c + 0x120) < -0x30c000) {
        if (*(unsigned char*)(c + 0x154) == 0) {
            *(unsigned char*)(c + 0x154) = 1;
            *(unsigned char*)(c + 0x155) = 1;
            func_02012790(0x121);
        }
    }

    if (*(int*)(c + 0x120) < -0x3e8000) {
        *(int*)(c + 0x110) = 6;
        *(int*)(c + 0x114) = 0;
    }
}
