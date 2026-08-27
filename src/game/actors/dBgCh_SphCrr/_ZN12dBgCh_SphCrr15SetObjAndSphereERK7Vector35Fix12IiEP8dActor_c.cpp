//cpp
// @symbol _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c
/* recovered: named members + shared header */
#include "decl_common.h"
#include "dBgCh_SphCrr.h"

extern "C" void func_020353b0(void *c, void *p);

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az homes class-typed by-value parameters. The declaration in
   dBgCh_SphCrr.h is the real one and callers may use it. */
extern "C" void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(dBgCh_SphCrr *self, const Vector3 *pos, int radius, dActor_c *actor)
{
    /* through the REFERENCE: a pointer-level upcast makes mwcc emit the
       null-checked MI adjustment (movs/addne), the ROM's is unconditional */
    func_0203abd4((int *)&(dM3dGSph &)*self, (int *)pos, radius);
    func_020353b0((char *)self, actor);
    func_02037b5c((char *)self);
    self->mScale = 0x1000;
}
