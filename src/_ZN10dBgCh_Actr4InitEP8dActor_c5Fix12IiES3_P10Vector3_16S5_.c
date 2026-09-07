// @symbol _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_
/* recovered: named members + shared header */
#include "dBgCh_Actr.h"
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(struct dBgCh_Actr *self, int a1, int a2, int a3, int sp0, int sp1) {
  *(int*)((char*)&self->mActor)=a1;
  *(int*)((char*)&self->mRadius)=a2;
  *(int*)((char*)&self->mHeight)=a3;
  *(int*)((char*)&self->mFlags)=0;
  *(int*)((char*)&self->unk_12c)=sp0;
  *(int*)((char*)&self->unk_130)=sp1;
  *(int*)((char*)&self->mScale)=0x1000;
}
