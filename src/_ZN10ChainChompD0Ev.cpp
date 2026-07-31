//cpp
// @symbol _ZN10ChainChompD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "ChainChomp.h"
extern "C" {
extern int func_0207328c(void *p, int a, int b, void *fn);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *p);
extern int func_ov002_020aed18(int *x);
extern int data_ov034_021147ec[];
extern int func_020072c0(void);
extern int data_020a0eac;

void *_ZN10ChainChompD0Ev(struct ChainChomp *self) {
    *(int**)((char *)self) = data_ov034_021147ec;
    func_0207328c(((char *)self) + 0x578, 7, 0xc, (void*)func_020072c0);
    func_0207328c(((char *)self) + 0x524, 7, 0xc, (void*)func_020072c0);
    func_0207328c(((char *)self) + 0x40c, 7, 0x28, (void*)_ZN11ShadowModelD1Ev);
    func_0207328c(((char *)self) + 0x1dc, 7, 0x50, (void*)_ZN5ModelD1Ev);
    _ZN11ShadowModelD1Ev((char *)&self->mShadowModel);
    _ZN9ModelAnimD1Ev((char *)&self->mModelAnim);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)&self->mMovingCylinderClsnWithPos);
    func_ov002_020aed18((int*)((char *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char *)self), *(void**)&data_020a0eac);
    return ((char *)self);
}
}
