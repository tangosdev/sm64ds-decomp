//cpp
// @symbol _ZN22RotatingUpDownPlatform8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatform.h"
extern "C" void _Z14ApproachLinearRiii(int*, int, int);
extern "C" void _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
typedef void (*PMFholder);
struct Platform {
    void UpdateModelPosAndRotY();
    int IsClsnInRange(int a, int b);
    void UpdateClsnPosAndRot();
};
struct PmfEntry;
extern "C" void func_020393d4(void *p, void *v);

typedef void (Platform::*PMF)();
struct PmfRow { PMF pmf; };
extern "C" PmfRow data_ov091_021354e0[];

int RotatingUpDownPlatform::Behavior()
{
    char *s = (char*)((Platform *)this);
    int old = *(int*)(s + 0x320);
    (((Platform *)this)->*data_ov091_021354e0[old].pmf)();
    *(unsigned short*)(((int)s + 0x354)) += 1;
    if (old != *(int*)(s + 0x320)) {
        *(short*)(s + 0x354) = 0;
        func_020393d4(s + 0x124, 0);
    } else {
        func_020393d4(s + 0x124, (void*)&_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    }
    if (*(unsigned char*)(s + 0x352) == 0) {
        int rate = 0x5000;
        int saved = *(int*)(s + 0x60);
        _Z14ApproachLinearRiii((int*)(s + 0x34c), (*(unsigned char*)(s + 0x356) != 0) ? 0x1e000 : 0, rate);
        *(int*)(((int)s + 0x60)) -= *(int*)(s + 0x34c);
        *(int*)(s + 0x60) = saved;
    }
    ((Platform *)this)->UpdateModelPosAndRotY();
    if (((Platform *)this)->IsClsnInRange(0, 0) != 0)
        ((Platform *)this)->UpdateClsnPosAndRot();
    *(unsigned char*)(s + 0x356) = 0;
    return 1;
}
