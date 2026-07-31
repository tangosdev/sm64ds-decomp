//cpp
// @symbol _ZN3Amp8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Amp.h"
extern "C" {
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, void *v);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void func_ov070_02120724(void *c);
extern int data_ov070_0212365c[];
}

int Amp::Behavior()
{
    int *p;
    func_ov070_02120d34(((char *)this));
    p = (int *)(((int)((char *)this) + 0x414) & 0xFFFFFFFFFFFFFFFF);
    *p += data_ov070_0212365c[1];
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char *)this) + 0x1d8, ((char *)this) + 0x410);
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsnWithPos);
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsnWithPos);
    func_ov070_02120724(((char *)this));
    return 1;
}
