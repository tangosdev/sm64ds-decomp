// @symbol _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "SphereClsn.h"
extern void func_020353b0(void* c, void* p);

void _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(struct SphereClsn *self, int* vec, int fix, void* actor) {
    func_0203abd4((int*)((char*)&self->unk_038), vec, fix);
    func_020353b0(((char*)self), actor);
    func_02037b5c(((char*)self));
    self->unk_108 = 0x1000;
}
