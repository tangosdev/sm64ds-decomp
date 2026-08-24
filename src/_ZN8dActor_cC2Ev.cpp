//cpp
#include "types.h"
// @symbol _ZN8dActor_cC2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
extern int data_0208e4b8;           /* dBase_c vtable  */
extern int data_0208e3a4;           /* dActor_c vtable */
extern int data_0209b468;           /* the global actor list */
extern s16* data_0209b460;          /* spawn position, or null to leave it at 0 */
extern s16* data_0209b45c;          /* spawn rotation, or null */
extern s16 data_0208e378;           /* spawn death-table id */
extern int* data_020a4bb8;          /* actor info table, indexed by actorID */
extern unsigned char data_0209f2d8; /* game mode; 2 pushes the clip distance out */
void _ZN7fBase_cC2Ev(void* self);
int func_0203b244(void* l, void* n);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void* self, int a, int b, int c, int d);
}

/* Base-object constructor. Chains fBase_c, links the actor into the global list,
   and seeds position/rotation/area/death-table id from the globals the spawner
   parked them in. Not a real dActor_c::dActor_c(): defining it as a method would
   make some TU the key function and emit a second copy of the class vtable. */
extern "C" void* _ZN8dActor_cC2Ev(struct dActor_c *self) {
    int* info;
    int isMode2;
    int clipDistance;

    _ZN7fBase_cC2Ev(self);
    /* dBase_c's constructor is inlined here, which is why its vptr is stored and
       then immediately overwritten. Both stores are in the ROM; keep them. */
    *(void**)self = &data_0208e4b8;
    *(void**)self = &data_0208e3a4;

    self->mListPrev = 0;
    self->mListNext = 0;
    self->mListOwner = self;
    func_0203b244((void*)&data_0209b468, &self->mListPrev);

    {
        int* spawnPos = (int*)data_0209b460;
        if (spawnPos != 0) {
            self->mPosX = spawnPos[0];
            self->mPosY = spawnPos[1];
            self->mPosZ = spawnPos[2];
        }
    }
    {
        s16* spawnRot = data_0209b45c;
        if (spawnRot != 0) {
            self->mAngleX = spawnRot[0];
            self->mAngleY = spawnRot[1];
            self->mAngleZ = spawnRot[2];
            /* Re-read of the same global, and the ROM does re-issue the load --
               fold it into one pointer and the second group loses an ldr. */
            spawnRot = data_0209b45c;
            self->mPrevAngleX = spawnRot[0];
            self->mPrevAngleY = spawnRot[1];
            self->mPrevAngleZ = spawnRot[2];
        }
    }

    self->mAreaId = data_0209b44c;
    self->mDeathTableID = data_0208e378;

    info = (int*)(((int**)data_020a4bb8)[self->actorID]);
    self->mFlags = info[2];
    isMode2 = (data_0209f2d8 == 2);
    if (isMode2 != 0)
        clipDistance = info[5] + 0x7d0000;
    else
        clipDistance = info[5];
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(self, info[3], info[4], clipDistance, info[6]);
    return self;
}
