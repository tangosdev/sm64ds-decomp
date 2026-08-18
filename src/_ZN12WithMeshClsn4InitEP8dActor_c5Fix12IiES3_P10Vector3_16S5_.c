// @symbol _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_
/* recovered: named members + shared header */
#include "WithMeshClsn.h"
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(struct WithMeshClsn *self, int a1, int a2, int a3, int sp0, int sp1) {
  *(int*)((char*)&self->mActor)=a1;
  *(int*)((char*)&self->unk_018)=a2;
  *(int*)((char*)&self->unk_01c)=a3;
  *(int*)((char*)&self->mFlags)=0;
  *(int*)((char*)&self->unk_12c)=sp0;
  *(int*)((char*)&self->unk_130)=sp1;
  *(int*)((char*)&self->unk_1b8)=0x1000;
}
