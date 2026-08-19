//cpp
// @symbol _ZN6DorrieD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Dorrie.h"
extern "C" {
extern void _ZN10dCcAcPos_cD1Ev(void *);
extern int __destroy_arr(void *, int, int, void *);
extern void func_ov065_021180b8();
extern void *_ZTV6Dorrie[];
extern void *data_020a0eac;
void *_ZN6DorrieD0Ev(struct Dorrie *self) {
    *(void ***)((char *)self) = _ZTV6Dorrie;
    _ZN10dCcAcPos_cD1Ev((char *)&self->unk_1140);
    _ZN7dCcAc_cD1Ev((char *)&self->unk_110c);
    _ZN10dBgCh_ActrD1Ev((char *)&self->mWithMeshClsn);
    __destroy_arr(((char *)self) + 0x150, 7, 0x200, (void*)&func_ov065_021180b8);
    _ZN9ModelAnimD1Ev((char *)&self->mModelAnim);
    _ZN8dActor_cD2Ev(((char *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((char *)self), *(void **)&data_020a0eac);
    return ((char *)self);
}
}
