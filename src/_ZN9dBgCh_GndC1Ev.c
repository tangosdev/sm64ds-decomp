// @symbol _ZN9dBgCh_GndC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgCh_Gnd.h"
extern void _ZN5dBgChC2Ev(void* c);
extern int* _ZN5dBgPiC2Ev(void* t);

void* _ZN9dBgCh_GndC1Ev(struct dBgCh_Gnd *self) {
    _ZN5dBgChC2Ev(((char*)self));
    _ZN5dBgPiC2Ev((char*)&self->unk_010);
    *(unsigned int**)((char*)self) = data_02099264;
    *(unsigned int**)((char*)&self->unk_010) = data_02099274;
    self->unk_04c = 0x1f4000;
    return ((char*)self);
}
