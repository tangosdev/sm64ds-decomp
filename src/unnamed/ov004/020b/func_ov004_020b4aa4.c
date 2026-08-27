struct Pair { int a, b; };
extern int _Z15ApproachLinear2Rsss(short* p, short a, short b);
extern int _Z14ApproachLinearRiii(int* p, int a, int b);
extern void Vec2_Sub(struct Pair* o, int* a, int* b);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n);
extern void func_ov004_020b1b40(void* c);
extern void func_ov004_020b5334(char* c);
extern int data_ov004_020bf9f8[];
void func_ov004_020b4aa4(char* c) {
    struct Pair t;
    if (_Z15ApproachLinear2Rsss((short*)(c + 0x20), 0, 1) == 0) return;
    _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
    _Z14ApproachLinearRiii((int*)(c + 0xc), *(int*)(c + 0x14), *(int*)(c + 0x1c));
    Vec2_Sub(&t, (int*)(c + 0x10), (int*)(c + 8));
    if (t.a != 0) return;
    if (t.b != 0) return;
    if (_Z15ApproachLinear2Rsss((short*)(c + 0x20), 0, 1) == 0) return;
    _ZN5Sound12PlayBank2_2DEj(0x149);
    func_ov004_020b1b40((void*)1);
    _Z14ApproachLinearRiii(data_ov004_020bf9f8, 0, 1);
    func_ov004_020b5334(c);
}
