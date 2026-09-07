//cpp
// @symbol _ZN3MrI13InitResourcesEv
// NONMATCHING: 3/166 at exact size 0x298. The draft that stood here DID NOT COMPILE at
// all -- `mwccarm.exe: undefined identifier 'Matrix4x3'` at its line 111 -- so it was
// scoring nothing. This is a real C++ method on include/MrI.h with named members
// (mModelAnim, mShadowModel, mdCcAcPos_c, mShadowRadiusScale, mShadowHeight, mTimer,
// unk_1ec, unk_217) and no shadow structs.
//
// The whole residue is a three-word rotation of the ModelBase::SetFile argument setup at
// +0x20: the ROM emits mov r2,#1 / mov r1,r0 / add r0,r4,#0xd4 / mov r3,r2, every build
// here emits mov r1,r0 / add r0,r4,#0xd4 / mov r2,#1 / mov r3,r2. Same instruction
// multiset, same registers -- pure emission order.
//
// PROBED, and this is the useful part for anyone who returns to it: mwccarm 2004/b56 DOES
// emit the ROM's constant-first order, but only when the basic block holding the call also
// contains a condition-code comparison. `if (!SetFile(...)) return;` and even a comparison
// on an unrelated global (`g = (h == 3);`) both flip it; a plain call, an if with an empty
// body, a switch, a goto label, a stored result, a trailing loop whose compare sits in the
// loop latch, and a following call all leave it alone. This function has no comparison in
// that block, so the order is out of reach from the source. Also inert: 14 conditional-
// context spellings of the call, named/const/register/bool/byte/short constants, nested
// and member-call forms of SetFile, 4 inline-helper wrappers, 7 structural variants of the
// surrounding loop and declarations, and all 24 pragma cells.
// @symbol _ZN3MrI13InitResourcesEv
#include "MrI.h"

struct BMD_File;
struct BTP_File;
struct BCA_File;
struct SharedFilePtr { void *hdr; void *ptr; };

extern "C" void LoadBlueCoinModel(void);
extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(BMD_File &a, BTP_File &b);
extern "C" void *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN10dCcAcPos_c4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *a, Vector3 const &b, int c, int d, unsigned int e, unsigned int f);
extern "C" unsigned char _ZN5Actor9TrackStarEjj(void *self, unsigned int a, unsigned int b);
extern "C" void func_ov071_02121634(void *self, int a);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int a, int b, unsigned int c);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, BTP_File &f, int a, int b, unsigned int c);
extern "C" void _ZN9dBgCh_GndC1Ev(void *self);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P5Actor(void *self, Vector3 const &pos, void *act);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern "C" void func_ov071_02120c90(void *c);
extern "C" void _ZN9dBgCh_GndD1Ev(void *self);

extern "C" SharedFilePtr data_ov002_0210da38;
extern "C" SharedFilePtr data_ov071_02123050;
extern "C" SharedFilePtr *data_ov071_021226a4[2];
extern "C" SharedFilePtr data_ov071_021226a0;
extern "C" SharedFilePtr data_ov071_02123048;
extern "C" SharedFilePtr data_ov071_02123038;
extern "C" char data_02082128;

struct M48 { int w[12]; };

s32 MrI::InitResources()
{
    char *c = (char *)this;
    BMD_File *bmd;
    LoadBlueCoinModel();

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da38);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov071_02123050);
    ((ModelBase *)&mModelAnim)->SetFile(bmd, 1, 1);

    int i;
    for (i = 0; i < 2; i++) {
        SharedFilePtr *seq = data_ov071_021226a4[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(*seq);
        BMD_File *bmd2 = *(BMD_File **)((char *)&data_ov071_02123050 + 4);
        BTP_File *btp = *(BTP_File **)((char *)seq + 4);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(*bmd2, *btp);
    }

    _ZN9Animation8LoadFileER13SharedFilePtr(**(SharedFilePtr **)&data_ov071_021226a0);

    if (!mShadowModel.InitCylinder())
        return 0;

    unsigned short kind = actorID;
    int isKind1 = (kind == 0x106);
    if (isKind1) {
        Vector3 v;
        v.x = 0;
        v.y = -0x4b000;
        v.z = 0;
        _ZN10dCcAcPos_c4InitEP5ActorRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, c, v, 0x55000, 0x96000, 0x200004, 0x42000);
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        mShadowRadiusScale = 0x1000;
    } else {
        int isKind2 = (kind == 0x107);
        if (isKind2) {
            Vector3 v;
            v.x = 0;
            v.y = -0x96000;
            v.z = 0;
            _ZN10dCcAcPos_c4InitEP5ActorRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, c, v, 0xaa000, 0x12c000, 0x200004, 0);
            mScaleX = 0x2000;
            mScaleY = 0x2000;
            mScaleZ = 0x2000;
            mShadowRadiusScale = 0x2000;
            {
                unsigned char mask = (unsigned char)(param1 & 0xf);
                unsigned char star = _ZN5Actor9TrackStarEjj(c, mask, 2);
                unk_217 = star;
            }
        }
    }

    mVertAccel = 0;
    mTerminalVelocity = 0;
    func_ov071_02121634(c, 0);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File *)data_ov071_02123048.ptr, 0, 0x1000, 0);

    *(int *)(c + 0x130) = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, *(BTP_File *)data_ov071_02123038.ptr, 0, 0x1000, 0);

    *(int *)(c + 0x144) = 0x1000;
    unk_1ec = 0;
    mTimer = 0x2e;

    *(M48 *)&mShadowMat = *(M48 *)&data_02082128;

    char rg[0x50];
    _ZN9dBgCh_GndC1Ev(rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P5Actor(rg, *(Vector3 *)&mPosX, c);
    int y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(rg)) {
        y = (mPosY - *(int *)(rg + 0x44)) + 0x1e000;
    } else {
        y = 0x12c000;
    }
    mShadowHeight = y;
    func_ov071_02120c90(c);
    _ZN9dBgCh_GndD1Ev(rg);

    return 1;
}
