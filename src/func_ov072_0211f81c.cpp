//cpp
typedef int Fix12;
struct Vector3 { int x, y, z; };
extern "C" void _Z14ApproachLinearRiii(int* p, int a, int b);
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void func_0203568c(int *p, int v);
extern "C" void func_02035684(int *p, int v);
extern "C" void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void* self, Fix12 a, Fix12 b, Fix12 c, Fix12 d);
extern "C" int func_ov072_0211f1dc(char* c);
extern "C" int func_ov072_0211f0a4(char* c);
extern "C" void func_ov072_0211fcb0(char* c, int i);
extern "C" void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(Fix12 a, Fix12 b, Fix12 c);
extern "C" int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned a, unsigned b, unsigned c, const struct Vector3* v, unsigned e);
extern "C" void func_ov072_0211f158(char* c);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cc);
extern "C" void func_ov072_0211f330(char* c, void* mc);
extern "C" void func_ov072_0211f280(char* c);
extern "C" int Vec3_Dist(const void* a, const void* b);
extern const struct Vector3 data_ov072_02122b40;

extern "C" int func_ov072_0211f81c(char* c) {
    int t;
    int v;
    _Z14ApproachLinearRiii((int*)(c+0x98), 0x28000, 0x400);
    if (DecIfAbove0_Short((unsigned short*)(c+0x3a0)) == 0) {
        t = *(int*)(c+0x80);
        _Z14ApproachLinearRiii(&t, 0x1800, 0x11);
        v = t;
        *(int*)(c+0x80) = v;
        *(int*)(c+0x84) = v;
        *(int*)(c+0x88) = v;
        /* reload t from stack for mul (matches ROM: str x3 then ldr for umull) */
        *(int*)(c+0x398) = (int)(((long long)t * 0x82000 + 0x800) >> 12);
        *(int*)(c+0x150) = *(int*)(c+0x398);
        *(int*)(c+0x154) = *(int*)(c+0x398) << 1;
        func_0203568c((int*)(c+0x180), *(int*)(c+0x398));
        func_02035684((int*)(c+0x180), *(int*)(c+0x398));
        _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(c, *(int*)(c+0x398), *(int*)(c+0x398), 0x1000000, 0x1000000);
    }
    if (func_ov072_0211f1dc(c) != 0) {
        if (*(unsigned char*)(c+0x3a3) != 0 && func_ov072_0211f0a4(c) != 0) {
            func_ov072_0211fcb0(c, 3);
        } else {
            func_ov072_0211fcb0(c, 4);
        }
    }
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
    *(int*)(c+0x39c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned*)(c+0x39c), 3, 0x8a, (const struct Vector3*)(c+0x74), 0);
    func_ov072_0211f158(c);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x14c);
    func_ov072_0211f330(c, c+0x180);
    func_ov072_0211f280(c);
    if (*(unsigned char*)(c+0x3a3) == 0) {
        if (Vec3_Dist(&data_ov072_02122b40, *(char**)(c+0x390) + 0x5c) < 0x300000) {
            *(unsigned char*)(c+0x3a3) = 1;
        }
    }
    return 1;
}
