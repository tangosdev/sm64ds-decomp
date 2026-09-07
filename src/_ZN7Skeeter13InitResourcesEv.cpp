//cpp
// @symbol _ZN7Skeeter13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Loads the model and four animations, builds both collision volumes, and then
 * FINDS THE WATER LINE -- which is the part that matters.
 *
 * It raycasts straight down from 0x32000 above the spawn point using a local
 * dBgCh_Gnd, and what it does with the hit depends on a surface flag: a
 * 0x20 surface sets unk_39c and only records the height, while any other
 * surface records it as both unk_3a8 and unk_3ac. mPosY is then snapped to
 * unk_3ac, so the skeeter starts exactly on the surface it found.
 *
 * Two early exits skip the raycast entirely: one level/mode combination
 * (0x12 in mode 2) and the flagged-surface case both install a different
 * starting state and return.
 *
 * Otherwise the heading is randomised -- four bits of RandomIntInternal shifted
 * into the top of a s16 -- and written through mModelAnim's own angle slot
 * before being published to mPrevAngleY and mAngleY.
 *
 * The local typedefs are renamed Loc* rather than deleted: the shared header
 * supplies the real Fix12/Vector3/SharedFilePtr, but these calls take the ROM's
 * by-value Fix12<int> signatures, which mwccarm passes differently -- the 6az
 * wall, notes/mwccarm-codegen.md.
 */
#include "Skeeter.h"
#include "dBgCh_Gnd.h"
typedef int LocFix12;
typedef struct { int w[2]; } LocSharedFilePtr;
typedef struct { short x,y,z; } LocVector3_16;
typedef struct { int x,y,z; } LocVec3;
typedef struct BMD_File BMD_File;
typedef struct dActor_c dActor_c;
struct C; typedef int (C::*PMF)();

extern "C" {
BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(LocSharedFilePtr* f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
void _ZN9Animation8LoadFileER13SharedFilePtr(LocSharedFilePtr* f);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, dActor_c* a, LocVec3* v, LocFix12 r, LocFix12 h, unsigned int e, unsigned int g);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, LocFix12 r, LocFix12 h, LocVector3_16* p, LocVector3_16* q);
void func_0203558c(void* self);
int func_ov090_02131e00(void* c, PMF* p);
int SurfaceInfo_TestFlag0x20(const SurfaceInfo* p);
int RandomIntInternal(int* seed);

extern LocSharedFilePtr data_ov090_021344a0;
extern LocSharedFilePtr data_ov090_02134488;
extern LocSharedFilePtr data_ov090_02134480;
extern LocSharedFilePtr data_ov090_02134490;
extern LocSharedFilePtr data_ov090_02134498;
extern LocVec3 data_ov090_0213412c;
extern unsigned char data_0209f2d8;
extern signed char data_0209f2f8;
extern PMF data_ov090_021344f4;
extern int data_02092138;
extern int data_0209e650;
extern PMF data_ov090_021344e4;
}

int Skeeter::InitResources()
{
    char* c = (char*)this;
    BMD_File* f;
    int r;
    Vector3 pos;
    LocVec3 v;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov090_021344a0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, f, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov090_02134488);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov090_02134480);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov090_02134490);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov090_02134498);

    mTerminalVelocity = -0x3c000;

    v.x = data_ov090_0213412c.x;
    v.y = data_ov090_0213412c.y;
    v.z = data_ov090_0213412c.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, (dActor_c*)c, &v, 0x5a000, 0x5a000, 0x200000, 0x7eff0);

    mAngleY = mPrevAngleY;
    unk_3a4 = 0x1000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (dActor_c*)c, 0xc8000, 0, (LocVector3_16*)0, (LocVector3_16*)0);
    func_0203558c(&mWithMeshClsn);

    unk_108 = 1;
    unk_10a = 2;
    mVertAccel = -0x3000;

    {
        int b = 1;
        if (data_0209f2d8 != 2) b = 0;
        if (b != 0 && data_0209f2f8 == 0x12) {
            unk_3ac = mPosY;
            unk_374 = mPosX;
            unk_378 = mPosY;
            unk_37c = mPosZ;
            func_ov090_02131e00(c, &data_ov090_021344f4);
            return 1;
        }
    }

    {
        dBgCh_Gnd ground;
        ground.mProbeHeight = 0xbb8000;
        ground.StartDetectingWater();
        {
            int py = mPosY;
            int pz = mPosZ;
            int px = mPosX;
            int ip = py + 0x32000;
            pos.x = px;
            pos.y = ip;
            pos.z = pz;
        }
        ground.SetObjAndPos(pos, this);
        unk_3a8 = data_02092138;
        if (ground.DetectClsn() != 0) {
            if (SurfaceInfo_TestFlag0x20(&ground.surface) != 0) {
                unk_39c = 1;
                unk_3ac = ground.clsnY;
            } else {
                unk_3a8 = ground.clsnY;
                unk_3ac = ground.clsnY;
            }
        }

        mPosY = unk_3ac;
        unk_374 = mPosX;
        unk_378 = mPosY;
        unk_37c = mPosZ;

        if (unk_39c != 0) {
            func_ov090_02131e00(c, &data_ov090_021344f4);
            return 1;
        }

        {
            r = RandomIntInternal(&data_0209e650);
            short ang = (short)((((unsigned int)r >> 8) & 0xf) << 12);
            *(short*)&unk_39a = ang;
            mPrevAngleY = *(short*)&unk_39a;
            mAngleY = mPrevAngleY;
        }
        func_ov090_02131e00(c, &data_ov090_021344e4);
    }

    return 1;
}
