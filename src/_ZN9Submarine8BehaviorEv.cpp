//cpp
// @symbol _ZN9Submarine8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Submarine.h"
typedef int (*dummy)();
class C;
typedef int (C::*PMF)();
class C { public: int d; };

typedef struct { int x, y, z; } Vec3;

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* cc);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int fx, int t1, int t2, int s4, int s5);
extern void _ZN9Animation7AdvanceEv(void* thiz);
}

int Submarine::Behavior()
{
    volatile int v[3];
    int x, y, z;

    DecIfAbove0_Short((unsigned short*)((char*)&unk_100));
    {
        char* obj = *(char**)((char*)&unk_110);
        if (*(int*)(obj + 8) != 0) {
            PMF* pp = (PMF*)(obj + 8);
            ((C*)((char*)this)->**pp)();
        }
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), 0);

    x = mPosX;
    v[0] = x;
    y = mPosY;
    v[1] = y;
    z = mPosZ;
    v[2] = z;
    y += 0x384000;
    v[1] = y;

    *(void**)((char*)&unk_1b8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(volatile unsigned int*)((char*)&unk_1b8), 0x139, v[0], v[1], z, 0, 0);

    unk_08c = unk_092;
    unk_08e = unk_094;
    unk_090 = unk_096;

    func_ov026_02111f30(((char*)this));
    _ZN9Animation7AdvanceEv((char*)&mTextureTransformer);
    _ZN9Animation7AdvanceEv((char*)&mAnimation);
    return 1;
}
