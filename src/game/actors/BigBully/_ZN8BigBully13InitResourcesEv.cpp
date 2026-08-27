//cpp
#include "types.h"
// @symbol _ZN8BigBully13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BigBully.h"
extern "C" {
typedef struct dActor_c dActor_c;
struct dBgCh_Gnd { char buf[0x50]; };

extern int func_ov064_02116ec0(void* obj);
extern int _ZN8dActor_c9TrackStarEjj(dActor_c* self, unsigned int a, unsigned int b);
extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* self, const Vector3* p, dActor_c* a);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* self);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* self);
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
        dBgCh_Gnd rg;
        Vector3 pos;
        Vector3 v;
        int i;
        int ang;

        mNumBulliesKilled = 0;
        _ZN9dBgCh_GndC1Ev(&rg);

        {
            int tz = mPosZ;
            int ty = mPosY + 0x32000;
            int tx = mPosX;
            v.x = tx;
            v.y = ty;
            v.z = tz;
        }
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, (dActor_c*)((char*)this));

        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) {
            pos.y = *(int*)(rg.buf + 0x44);
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
                _ZN9dBgCh_GndD1Ev(&rg);
                return 0;
            }

            i++;
            ang += 0x5555;
        } while (i < 3);

        _ZN9dBgCh_GndD1Ev(&rg);
        goto done;
    }

    mNumBulliesKilled = 0xff;
done:
    return saved;
}
