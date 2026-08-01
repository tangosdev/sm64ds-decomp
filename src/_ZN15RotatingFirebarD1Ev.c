// @symbol _ZN15RotatingFirebarD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "RotatingFirebar.h"
extern void __destroy_arr(void *arr, int a, int b, void *fn);
extern void _ZN19CylinderClsnWithPosD1Ev(void *c);
extern int _ZTV17ExclamationSwitch[];
int _ZN15RotatingFirebarD1Ev(struct RotatingFirebar *self) {
    *(int**)((char *)self) = (int*)_ZTV15RotatingFirebar;
    __destroy_arr(((char *)self) + 0x360, 8, 0x3c, (void*)_ZN19CylinderClsnWithPosD1Ev);
    *(int**)((char *)self) = (int*)_ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMeshCollider);
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN5ActorD2Ev(((char *)self));
    return (int)((char *)self);
}
