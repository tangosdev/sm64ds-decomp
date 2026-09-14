//cpp
// @symbol _ZN3MrI13InitResourcesEv
// MATCHED 0/166 at size 0x298 (2004/b56).
//
// This sat at 3/166 across two campaigns, both declaring it floored. The whole residue
// was a three-word rotation of the ModelBase::SetFile argument setup at +0x20 -- the same
// four instructions in the same registers, with the independent constant in a different
// slot:
//   ROM:  mov r2,#1 / mov r1,r0 / add r0,r4,#0xd4 / mov r3,r2
//   was:  mov r1,r0 / add r0,r4,#0xd4 / mov r2,#1 / mov r3,r2
//
// None of that was about scheduling. The entire difference is the declaration of
// LoadBlueCoinModel, which this file used to declare as taking no arguments. The ROM
// passes the actor to it: all ten call sites in the cartridge set r0 immediately before
// the bl (nine as `mov r0, r4`, and this function's own site at +0x0c leaves the incoming
// this-pointer in r0 untouched). Declaring the parameter and passing `c` keeps the
// incoming r0 live across that first call, mwccarm coalesces the two, and the entry
// block's argument setup comes out in the cartridge's order.
//
// The trap that hid it for two campaigns: LoadBlueCoinModel IGNORES its argument -- its
// first real instruction is `ldr r0,[pc,#0x14]`, overwriting r0 before any read. So the
// usual screen ("does the callee read r0 before writing it?") answers NO and discards the
// fix. The effect that pays is entirely caller-side register liveness, and the only
// reliable evidence for the parameter is the CALL SITES, not the callee's body.
// src/LoadBlueCoinModel.c is matched and byte-verified and still declares it `(void)`;
// the callee's bytes are identical either way, so no byte gate, linkcheck or fdiff run
// can see the difference. Eight other sources in the tree already declare it by pointer.
//
// Measured inert while this was believed floored, all 3/166 at the exact size: 136
// pragma on/off cells over 68 accepted pragma names; every loop spelling (for/while/
// do-while/for(;;)/goto-label/nested scope/inline helper); nested and member-call forms
// of SetFile; named/const/register/bool/byte/short spellings of the two 1 constants at
// four declaration positions; void* vs BMD_File* parameter spellings; and Model::LoadFile
// declared by pointer vs reference.
#include "MrI.h"

struct BMD_File;
struct BTP_File;
struct BCA_File;
struct SharedFilePtr { void *hdr; void *ptr; };

extern "C" void LoadBlueCoinModel(void *self);
extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(BMD_File &a, BTP_File &b);
extern "C" void *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *a, const Vector3 *b, int c, int d, unsigned int e, unsigned int f);
extern "C" unsigned char _ZN8dActor_c9TrackStarEjj(void *self, unsigned int a, unsigned int b);
extern "C" void func_ov071_02121634(void *self, int a);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int a, int b, unsigned int c);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, BTP_File &f, int a, int b, unsigned int c);
extern "C" void _ZN9dBgCh_GndC1Ev(void *self);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, Vector3 *pos, void *act);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern "C" void func_ov071_02120c90(void *c);
extern "C" void _ZN9dBgCh_GndD1Ev(void *self);

extern "C" SharedFilePtr data_ov002_0210da38;
extern "C" SharedFilePtr data_ov071_02123050;
extern "C" SharedFilePtr *data_ov071_021226a4[2];
extern "C" SharedFilePtr data_ov071_021226a0;
extern "C" SharedFilePtr data_ov071_02123048;
extern "C" SharedFilePtr data_ov071_02123038;
extern "C" Matrix4x3 IDENTITY_MATRIX4X3;

struct M48 { int w[12]; };

s32 MrI::InitResources()
{
    char *c = (char *)this;
    BMD_File *bmd;
    LoadBlueCoinModel(c);

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
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, c, &v, 0x55000, 0x96000, 0x200004, 0x42000);
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
            _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, c, &v, 0xaa000, 0x12c000, 0x200004, 0);
            mScaleX = 0x2000;
            mScaleY = 0x2000;
            mScaleZ = 0x2000;
            mShadowRadiusScale = 0x2000;
            {
                unsigned char mask = (unsigned char)(param1 & 0xf);
                unsigned char star = _ZN8dActor_c9TrackStarEjj(c, mask, 2);
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

    *(M48 *)&mShadowMat = *(M48 *)&IDENTITY_MATRIX4X3;

    char rg[0x50];
    _ZN9dBgCh_GndC1Ev(rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, (Vector3 *)&mPosX, c);
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
