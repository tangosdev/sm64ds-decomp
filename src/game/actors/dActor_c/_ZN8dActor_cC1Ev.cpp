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
extern void* data_0208e4b8;         /* dBase_c vtable  */
extern void* data_0208e3a4;         /* dActor_c vtable */
extern void* data_0209b468;         /* the global actor list */
extern void* data_0209b460;         /* spawn position, or null to leave it at 0 */
extern void* data_0209b45c;         /* spawn rotation, or null */
extern short data_0208e378;         /* spawn death-table id */
extern void* data_020a4bb8;         /* actor info table, indexed by actorID */
extern unsigned char data_0209f2d8; /* game mode; 2 pushes the clip distance out */

/* Complete-object constructor. The ROM carries C1 and C2 as two separate copies
   of the same body, so this duplicates src/_ZN8dActor_cC2Ev.cpp by design; keep
   the two in step. Not a real dActor_c::dActor_c() -- defining it as a method
   would make some TU the key function and emit a second copy of the class vtable.

   The block nesting at the bottom is load-bearing: it is what puts the info-table
   temporaries in the frame slots the ROM uses. Flattening it costs bytes. */
extern "C" void* _ZN8dActor_cC1Ev(struct dActor_c *self) {
  _ZN7fBase_cC2Ev(self);
  /* dBase_c's constructor is inlined here, which is why its vptr is stored and
     then immediately overwritten. Both stores are in the ROM; keep them. */
  *(void**)self = &data_0208e4b8;
  *(void**)self = &data_0208e3a4;
  self->mListPrev = 0;
  self->mListNext = 0;
  self->mListOwner = self;
  func_0203b244(&data_0209b468, &self->mListPrev);
  {
    int* spawnPos = (int*)data_0209b460;
    if (spawnPos) {
      self->mPosX = spawnPos[0];
      self->mPosY = spawnPos[1];
      self->mPosZ = spawnPos[2];
    }
  }
  {
    short* spawnRot = (short*)data_0209b45c;
    if (spawnRot) {
      self->mAngleX = spawnRot[0];
      self->mAngleY = spawnRot[1];
      self->mAngleZ = spawnRot[2];
      /* Re-read of the same global, and the ROM does re-issue the load --
         reuse the pointer already loaded and the second group loses an ldr. */
      spawnRot = (short*)data_0209b45c;
      self->mPrevAngleX = spawnRot[0];
      self->mPrevAngleY = spawnRot[1];
      self->mPrevAngleZ = spawnRot[2];
    }
  }
  self->mAreaId = data_0209b44c;
  self->mDeathTableID = data_0208e378;
  {
    void** table = *(void***)&data_020a4bb8;
    int actorID = self->actorID;
    char* info = (char*)table[actorID];
    self->mFlags = *(int*)(info+8);
    {
      int isMode2 = (data_0209f2d8 == 2);
      int clipDistance;
      int farDistance = *(int*)(info+0x18);
      if (isMode2) clipDistance = *(int*)(info+0x14) + 0x7d0000;
      else clipDistance = *(int*)(info+0x14);
      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(self, *(int*)(info+0xc), *(int*)(info+0x10), clipDistance, farDistance);
    }
  }
  return self;
}
