//cpp
#include "types.h"
// @symbol _ZN8BigBully13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BigBully.h"
#include "dBgCh_Gnd.h"
extern "C" {
extern int func_ov064_02116ec0(void* obj);
extern int _ZN8dActor_c9TrackStarEjj(dActor_c* self, unsigned int a, unsigned int b);
extern dActor_c* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s8 areaID, s16 deathTableID);
extern s16 data_02082214[];
}

int BigBully::InitResources()
{
    int saved;

    *(void**)((char*)&mFileTable) = &data_ov064_0211b93c;
    saved = func_ov064_02116ec0(((char*)this));
    mStarID = param1 & 0xf;
    unk_3fd = (u8)_ZN8dActor_c9TrackStarEjj((dActor_c*)((char*)this), mStarID, 2);
    mSecretSoundCounter = 0;

    if ((param1 & 0xff00) == 0x100) {
        mNumBulliesKilled = 0;

        dBgCh_Gnd ground;
        Vector3 pos;
        Vector3 v;
        int i;
        int ang;

        {
            int tz = mPosZ;
            int ty = mPosY + 0x32000;
            int tx = mPosX;
            v.x = tx;
            v.y = ty;
            v.z = tz;
        }
        ground.SetObjAndPos(v, this);

        if (ground.DetectClsn() != 0) {
            pos.y = ground.clsnY;
        }

        i = 0;
        ang = 0;
        do {
            int idx = (u16)(s16)ang >> 4;
            dActor_c* spawned;

            pos.x = mPosX + data_02082214[idx * 2] * 500 - 0x64000;
            pos.z = mPosZ - data_02082214[idx * 2 + 1] * 500;

            spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xd7, -1, &pos, (Vector3_16*)0, mAreaId, -1);
            if (spawned != 0) {
                *(int*)((char*)spawned + 0x3fc) = uniqueID;
            } else {
                return 0;
            }

            i++;
            ang += 0x5555;
        } while (i < 3);

        goto done;
    }

    mNumBulliesKilled = 0xff;
done:
    return saved;
}
