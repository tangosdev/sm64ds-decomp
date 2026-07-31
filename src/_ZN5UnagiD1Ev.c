// @symbol _ZN5UnagiD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_BlendModelAnim.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Unagi.h"
extern int func_0207328c(void*, int, int, void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int func_ov002_020aed18(int*);
extern void func_020072c0(void);

int _ZN5UnagiD1Ev(struct Unagi *self) {
    *(void**)((void*)self) = _ZTV5Unagi;
    func_0207328c((char*)((void*)self) + 0x448, 7, 0xc, (void*)func_020072c0);
    _ZN14BlendModelAnimD1Ev((char*)&self->mBlendModelAnim);
    _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos2);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos1);
    func_ov002_020aed18((int*)((void*)self));
    return (int)((void*)self);
}
