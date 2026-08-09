//cpp
#include "types.h"
// @symbol _ZN8BigBully13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BigBully.h"
extern "C" {
typedef struct Actor Actor;
struct RaycastGround { char buf[0x50]; };

extern int func_ov064_02116ec0(void* obj);
extern int _ZN5Actor9TrackStarEjj(Actor* self, unsigned int a, unsigned int b);
extern void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3* p, Actor* a);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern void _ZN13RaycastGroundD1Ev(RaycastGround* self);
extern Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s8 areaID, s16 deathTableID);
extern s16 data_02082214[];
}

int BigBully::InitResources()
{
    int saved;

    *(void**)((char*)&unk_330) = &data_ov064_0211b93c;
    saved = func_ov064_02116ec0(((char*)this));
    unk_3fc = mParam & 0xf;
    unk_3fd = (u8)_ZN5Actor9TrackStarEjj((Actor*)((char*)this), unk_3fc, 2);
    mSecretSoundCounter = 0;

    if ((mParam & 0xff00) == 0x100) {
        RaycastGround rg;
        Vector3 pos;
        Vector3 v;
        int i;
        int ang;

        unk_3fe = 0;
        _ZN13RaycastGroundC1Ev(&rg);

        {
            int tz = mPosZ;
            int ty = mPosY + 0x32000;
            int tx = mPosX;
            v.x = tx;
            v.y = ty;
            v.z = tz;
        }
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, (Actor*)((char*)this));

        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
            pos.y = *(int*)(rg.buf + 0x44);
        }

        i = 0;
        ang = 0;
        do {
            int idx = (u16)(s16)ang >> 4;
            Actor* spawned;

            pos.x = mPosX + data_02082214[idx * 2] * 500 - 0x64000;
            pos.z = mPosZ - data_02082214[idx * 2 + 1] * 500;

            spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xd7, -1, &pos, (Vector3_16*)0, mAreaId, -1);
            if (spawned != 0) {
                *(int*)((char*)spawned + 0x3fc) = unk_004;
            } else {
                _ZN13RaycastGroundD1Ev(&rg);
                return 0;
            }

            i++;
            ang += 0x5555;
        } while (i < 3);

        _ZN13RaycastGroundD1Ev(&rg);
        goto done;
    }

    unk_3fe = 0xff;
done:
    return saved;
}
