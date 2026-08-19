// @symbol _ZN13RaycastGroundC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "RaycastGround.h"
extern void func_02035514(void* c);
extern int* func_0203819c(void* t);

void* _ZN13RaycastGroundC1Ev(struct RaycastGround *self) {
    func_02035514(((char*)self));
    func_0203819c((char*)&self->unk_010);
    *(unsigned int**)((char*)self) = data_02099264;
    *(unsigned int**)((char*)&self->unk_010) = data_02099274;
    self->unk_04c = 0x1f4000;
    return ((char*)self);
}
