//cpp
typedef int (*dummy)();
class C;
typedef int (C::*PMF)();
class C { public: int d; };

typedef struct { int x, y, z; } Vec3;

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* cc);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int fx, int t1, int t2, int s4, int s5);
extern void func_ov026_02111f30(char* c);
extern void _ZN9Animation7AdvanceEv(void* thiz);
}

extern "C" int _ZN9Submarine8BehaviorEv(char* c)
{
    volatile int v[3];
    int x, y, z;

    DecIfAbove0_Short((unsigned short*)(c + 0x100));
    {
        char* obj = *(char**)(c + 0x110);
        if (*(int*)(obj + 8) != 0) {
            PMF* pp = (PMF*)(obj + 8);
            ((C*)c->**pp)();
        }
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

    x = *(int*)(c + 0x5c);
    v[0] = x;
    y = *(int*)(c + 0x60);
    v[1] = y;
    z = *(int*)(c + 0x64);
    v[2] = z;
    y += 0x384000;
    v[1] = y;

    *(void**)(c + 0x1b8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(volatile unsigned int*)(c + 0x1b8), 0x139, v[0], v[1], z, 0, 0);

    *(short*)(c + 0x8c) = *(short*)(c + 0x92);
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    *(short*)(c + 0x90) = *(short*)(c + 0x96);

    func_ov026_02111f30(c);
    _ZN9Animation7AdvanceEv(c + 0x178);
    _ZN9Animation7AdvanceEv(c + 0x164);
    return 1;
}
