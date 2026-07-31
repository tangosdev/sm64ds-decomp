// @symbol _ZN8PathLiftD2Ev
/* recovered: named members + shared header */
#include "PathLift.h"
extern int func_0207328c(void *p, int a, int b, void *f);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ActorD2Ev(void *p);
extern int data_ov002_0210af70[];
extern int _ZTV17ExclamationSwitch[];
int _ZN8PathLiftD2Ev(struct PathLift *self) {
    *(int**)(((char *)self)) = data_ov002_0210af70;
    func_0207328c(((char *)self)+0x320, 3, 0x50, _ZN5ModelD1Ev);
    *(int**)(((char *)self)) = _ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char *)&self->mMovingMeshCollider);
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN5ActorD2Ev(((char *)self));
    return (int)((char *)self);
}
