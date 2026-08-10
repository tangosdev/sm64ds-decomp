//cpp
// @symbol _ZN6Player13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
// Match: Player::InitResources (ov002:0x020e558c).
// Heap zero of operator_new(0x14) at +0x588 uses mwccarm CLEAR shape
// (4x strb post-inc). Pure C never emits that for a heap pointer under
// -O4,p (only stack u8[N]={0} does); small asm block reproduces it.
// Also: pin data_0209f2d8 in local `d` so ==2 keeps r1 bool (preserves r0);
// decl order td,tx,tz + load order for RaycastGround pos regs.
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

#include "Player.h"

struct CAA0 { char pad[8]; int unk8; };

extern "C" {
    int SublevelToLevel(int s);
    void func_ov002_020e6330(void* p);
    void func_ov002_020e6350(void* p);
    void func_ov002_020e5948(void* p);
    void func_ov002_020beabc(void* p);
    void* func_02073470(int a, int b, int c, void* d, void* e);
    void* _ZN6Memory13operator_new2Ej(unsigned int sz);
    void func_ov002_020e63a4(void* p);
    void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int a, int b, void* v1, void* v2);
    void func_02035644(void* p, int a);
    void func_ov002_020d6368(void* p);
    void LoadSilverStarAndNumber(void);
    int _ZN8SaveData16HasPlayerLostCapEv(void);
    void func_02013a00(void);
    void func_ov002_020c7dd0(void* p, int a);
    void _ZN13RaycastGroundC1Ev(void* p);
    void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* p, void* v, void* a);
    void _ZN4BgCh19StartDetectingWaterEv(void* p);
    int StartWithFarCamera(void);
    void _ZN13RaycastGroundD1Ev(void* p);
    void func_020072c0(void);
    void func_0203d384(void);

    extern u8 data_0209f2d8;
    extern s8 data_0209f2f8;
    extern u8 data_0209f254;
    extern s8 data_02092114;
    extern u8 data_02092128[];
    extern CAA0 data_0209caa0;
    extern int data_0209fc48;
    extern u8 data_0209f250;
    extern u8 data_0209f2fc;
    extern int data_0209212c;
    extern u8 data_0209211c;
    extern u8 data_0209f200;
}

struct V3 { int x, y, z; };

s32 Player::InitResources()
{
    /* The helpers below take void or char pointers, and this file's match
       depends on declaration and load order documented in the header comment
       above. One alias keeps every one of those decisions untouched. */
    void* self = (void*)this;
    char* c = (char*)self;
    V3 pos;
    char rc[0x50];
    unsigned int a, b;
    u8 n8;
    u8 sub;
    int t;
    int changed;
    int i; unsigned int u;
    u8* q;
    u8 f254;
    u8 cur;
    int td, tx, tz;

    a = *(unsigned int*)(c + 8);
    mPlayerNo = (a >> 6) & 3;
    n8 = a >> 8;
    b = *(unsigned int*)(c + 8);
    sub = (b >> 3) & 7;
    mCharacter = b & 7;
    *(u8*)(c + 0x6da) = sub;
    changed = 0;
    t = (unsigned int)(data_0209f2d8 == 1);
    if (t == 0) {
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            data_0209f254 = 0;
            cur = mCharacter;
            if (sub != cur) {
                data_02092114 = -1;
                sub = cur;
                data_02092128[mPlayerNo] = *(volatile u8*)(&mCharacter);
                changed = 1;
            }
        }
        if (data_02092114 >= 0) sub = mCharacter;
    }
    *(int*)(c + 8) = sub;
    {
      unsigned int d = data_0209f2d8;
      t = (unsigned int)(d == 2);
      if (t != 0) goto Ld0;
      t = (unsigned int)(d == 1);
    }
    if (t != 0) goto Lac;
    if ((data_0209caa0.unk8 & 0x80) != 0) goto Lac;
    t = (unsigned int)(data_0209fc48 != 0);
    if (t == 0) goto Ld0;
Lac:
    if (mPlayerNo == data_0209f250) {
        func_ov002_020e6330(c);
        func_ov002_020e6350(c);
    }
Ld0:
    func_ov002_020e5948(c);
    if (changed != 0) func_ov002_020beabc(c);
    *(void**)(&unk_578) = func_02073470(0x32, 0xc, 8, (void*)func_0203d384, (void*)func_020072c0);
    *(void**)(&unk_57c) = _ZN6Memory13operator_new2Ej(0x32);
    *(void**)(&mHeldObjQueue) = _ZN6Memory13operator_new2Ej(0x14);
    q = *(u8**)(&mHeldObjQueue);
    asm {
      mov r1, #5
      mov r0, #0
    Lzero:
      strb r0, [q], #1
      strb r0, [q], #1
      strb r0, [q], #1
      strb r0, [q], #1
      subs r1, r1, #1
      bne Lzero
    }
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov002_020e63a4(c);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x380, c, 0x32000, 0x32000, c + 0x92, c + 0x8c);
    func_02035644(c + 0x380, 0x28000);
    mTerminalVelocity = -0x4b000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mSpawnAngleY = mAngleY;
    mGroundY = 0x80000000;
    unk_64c = 0x80000000;
    unk_650 = 0x80000000;
    mOpacity = 0x1f;
    mClpsWord1Lo = 0xff;
    *(s16*)&unk_6a2 = mAreaId;
    f254 = data_0209f254;
    if (f254 != 0) {
        if (*(int*)(c + 8) == 3) {
            mEggParams = f254;
            func_ov002_020d6368(c);
            if ((f254 & 0x80) != 0) {
                if ((mLoadedResourceFlags & 1) == 0) {
                    LoadSilverStarAndNumber();
                    *(u8*)((int)(&mLoadedResourceFlags)) |= 1;
                }
            }
        }
    }
    data_0209f254 = 0;
    if (data_0209f2fc == 1) {
        if (_ZN8SaveData16HasPlayerLostCapEv() != 0) func_02013a00();
    }
    func_ov002_020c7dd0(c, n8);
    _ZN13RaycastGroundC1Ev(rc);
    tz = mPosZ;
    tx = mPosX;
    td = data_0209212c;
    pos.x = tx;
    pos.y = td;
    pos.z = tz;
    *(int*)(rc + 0x4c) = td * 2;
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rc, &pos, c);
    _ZN4BgCh19StartDetectingWaterEv(rc);
    if (data_0209f2f8 == 0x1d) {
        data_0209211c = 2;
        data_0209f200 = 0xe;
    }
    if (StartWithFarCamera() != 0) unk_715 = 1;
    mPeakY = mPosY;
    _ZN13RaycastGroundD1Ev(rc);
    return 1;
}
