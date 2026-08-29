//cpp
// @symbol _ZN8dActor_cC2Ev
#include "decl_common.h"
#include "dActor_c.h"
extern "C" {
extern int data_0209b468;           /* the global actor list */
extern s16* data_0209b460;          /* spawn position, or null to leave it at 0 */
extern s16* data_0209b45c;          /* spawn rotation, or null */
extern s16 data_0208e378;           /* spawn death-table id */
extern int* data_020a4bb8;          /* actor info table, indexed by actorID */
extern unsigned char data_0209f2d8; /* game mode; 2 pushes the clip distance out */
int func_0203b244(void* l, void* n);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void* self, int a, int b, int c, int d);
}

/* Base-object constructor. The declared inheritance chain generates fBase_c's
   base step and the dBase_c/dActor_c vptr stores before the spawn-seeding body. */
dActor_c::dActor_c() : mActorListNode(this) {
    int* info;
    int isMode2;
    int clipDistance;

    func_0203b244((void*)&data_0209b468, &mActorListNode);

    {
        int* spawnPos = (int*)data_0209b460;
        if (spawnPos != 0) {
            mPosX = spawnPos[0];
            mPosY = spawnPos[1];
            mPosZ = spawnPos[2];
        }
    }
    {
        s16* spawnRot = data_0209b45c;
        if (spawnRot != 0) {
            mAngleX = spawnRot[0];
            mAngleY = spawnRot[1];
            mAngleZ = spawnRot[2];
            /* Re-read of the same global, and the ROM does re-issue the load --
               fold it into one pointer and the second group loses an ldr. */
            spawnRot = data_0209b45c;
            mPrevAngleX = spawnRot[0];
            mPrevAngleY = spawnRot[1];
            mPrevAngleZ = spawnRot[2];
        }
    }

    mAreaId = data_0209b44c;
    mDeathTableID = data_0208e378;

    info = (int*)(((int**)data_020a4bb8)[actorID]);
    mFlags = info[2];
    isMode2 = (data_0209f2d8 == 2);
    if (isMode2 != 0)
        clipDistance = info[5] + 0x7d0000;
    else
        clipDistance = info[5];
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, info[3], info[4], clipDistance, info[6]);
}
