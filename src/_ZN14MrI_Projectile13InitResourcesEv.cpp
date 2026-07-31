//cpp
// @symbol _ZN14MrI_Projectile13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "MrI_Projectile.h"
typedef int Fix12;
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern int _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *v, Fix12 r, int t1, unsigned int u1, unsigned int u2);
extern int _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, Fix12 a, int b, void *v, int c);
extern void func_ov071_02121c6c(char *c);
extern void *data_ov071_021230b8;
struct M48 { int w[12]; };
extern struct M48 data_02082128;

int MrI_Projectile::InitResources()
{
    if (!_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel)) return 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char *)this) + 0xfc, ((char *)this), &data_ov071_021230b8, 0x23000, 0x3c000, 0x200002, 0x8000);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x13c, ((char *)this), 0x19000, 0x19000, 0, 0);
    unk_09c = 0;
    unk_0a0 = 0;
    unk_098 = 0xa000;
    unk_330 = 0x96;
    *(struct M48*)((char *)&unk_2f8) = data_02082128;
    func_ov071_02121c6c(((char *)this));
    return 1;
}
