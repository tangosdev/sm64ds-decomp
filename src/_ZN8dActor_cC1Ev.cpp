//cpp
// @symbol _ZN8dActor_cC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
void _ZN7fBase_cC2Ev(void* self);
void func_0203b244(void* l, void* n);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void* self, int a, int b, int c, int d);
}
extern void* data_0208e4b8;
extern void* data_0208e3a4;
extern void* data_0209b468;
extern void* data_0209b460;
extern void* data_0209b45c;
extern short data_0208e378;
extern void* data_020a4bb8;
extern unsigned char data_0209f2d8;

extern "C" void* _ZN8dActor_cC1Ev(struct dActor_c *self) {
  _ZN7fBase_cC2Ev(((char*)self));
  *(void**)((char*)self) = &data_0208e4b8;
  *(void**)((char*)self) = &data_0208e3a4;
  self->unk_050 = 0;
  self->unk_054 = 0;
  *(void**)((char*)&self->unk_058) = ((char*)self);
  func_0203b244(&data_0209b468, ((char*)self)+0x50);
  {
    int* p = (int*)data_0209b460;
    if (p) {
      self->mPosX = p[0];
      self->mPosY = p[1];
      self->mPosZ = p[2];
    }
  }
  {
    short* q = (short*)data_0209b45c;
    if (q) {
      self->mAngleX = q[0];
      self->mAngleY = q[1];
      self->mAngleZ = q[2];
      q = (short*)data_0209b45c;
      self->mPrevAngleX = q[0];
      self->mPrevAngleY = q[1];
      self->mPrevAngleZ = q[2];
    }
  }
  self->mAreaId = data_0209b44c;
  self->unk_0ce = data_0208e378;
  {
    void** base = *(void***)&data_020a4bb8;
    int idx = self->actorID;
    char* s = (char*)base[idx];
    self->mFlags = *(int*)(s+8);
    {
      int b = (data_0209f2d8 == 2);
      int r3;
      int d = *(int*)(s+0x18);
      if (b) r3 = *(int*)(s+0x14) + 0x7d0000;
      else r3 = *(int*)(s+0x14);
      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(((char*)self), *(int*)(s+0xc), *(int*)(s+0x10), r3, d);
    }
  }
  return ((char*)self);
}
