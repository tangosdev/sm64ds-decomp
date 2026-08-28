//cpp
// @symbol _ZN8dActor_cC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
void func_0203b244(void* l, void* n);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void* self, int a, int b, int c, int d);
}
extern void* data_0209b468;         /* the global actor list */
extern void* data_0209b460;         /* spawn position, or null to leave it at 0 */
extern void* data_0209b45c;         /* spawn rotation, or null */
extern short data_0208e378;         /* spawn death-table id */
extern void* data_020a4bb8;         /* actor info table, indexed by actorID */
extern unsigned char data_0209f2d8; /* game mode; 2 pushes the clip distance out */

/* Complete-object constructor. The ROM carries C1 and C2 as two separate copies
   of the same body, so this duplicates src/_ZN8dActor_cC2Ev.cpp by design; keep
   the two in step. The declared inheritance chain generates the fBase_c base
   call and the dBase_c/dActor_c vptr stores before this body.

   The block nesting at the bottom is load-bearing: it is what puts the info-table
   temporaries in the frame slots the ROM uses. Flattening it costs bytes. */
dActor_c::dActor_c() {
  mListPrev = 0;
  mListNext = 0;
  mListOwner = this;
  func_0203b244(&data_0209b468, &mListPrev);
  {
    int* spawnPos = (int*)data_0209b460;
    if (spawnPos) {
      mPosX = spawnPos[0];
      mPosY = spawnPos[1];
      mPosZ = spawnPos[2];
    }
  }
  {
    short* spawnRot = (short*)data_0209b45c;
    if (spawnRot) {
      mAngleX = spawnRot[0];
      mAngleY = spawnRot[1];
      mAngleZ = spawnRot[2];
      /* Re-read of the same global, and the ROM does re-issue the load --
         reuse the pointer already loaded and the second group loses an ldr. */
      spawnRot = (short*)data_0209b45c;
      mPrevAngleX = spawnRot[0];
      mPrevAngleY = spawnRot[1];
      mPrevAngleZ = spawnRot[2];
    }
  }
  mAreaId = data_0209b44c;
  mDeathTableID = data_0208e378;
  {
    void** table = *(void***)&data_020a4bb8;
    int actorID = this->actorID;
    char* info = (char*)table[actorID];
    mFlags = *(int*)(info+8);
    {
      int isMode2 = (data_0209f2d8 == 2);
      int clipDistance;
      int farDistance = *(int*)(info+0x18);
      if (isMode2) clipDistance = *(int*)(info+0x14) + 0x7d0000;
      else clipDistance = *(int*)(info+0x14);
      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, *(int*)(info+0xc), *(int*)(info+0x10), clipDistance, farDistance);
    }
  }
}
