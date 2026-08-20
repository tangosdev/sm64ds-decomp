// @symbol _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgCh_SphCrr.h"
extern void func_020353b0(void* c, void* p);

void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(struct dBgCh_SphCrr *self, int* vec, int fix, void* actor) {
    func_0203abd4((int*)((char*)&self->unk_038), vec, fix);
    func_020353b0(((char*)self), actor);
    func_02037b5c(((char*)self));
    self->unk_108 = 0x1000;
}
