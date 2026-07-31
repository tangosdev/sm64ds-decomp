//cpp
// @symbol _ZN5Unagi13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Unagi.h"
struct V3 { int x, y, z; };
struct PathPtr { char b[8]; };

extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* m, void* f, int a, int b);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN7PathPtrC1Ev(void* p);
extern void _ZN7PathPtr6FromIDEj(void* p, unsigned int id);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, char* actor, struct V3* pos, int r3, int sp0, int sp4, int sp8);
extern int IsStarCollected(int a, int b);
extern void func_ov016_02111bf0(char* c, void* p);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, char* pos, void* d, int sp0, int sp4);
extern void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(char* a, int r1, int r2, int r3, int sp0);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* p, char* out, unsigned int n);
extern short Vec3_HorzAngle(char* a, char* b);

extern struct V3 data_ov016_02114d4c;
extern void* data_ov016_02114dbc;
extern unsigned char data_0209f220;
}

int Unagi::InitResources()
{
    PathPtr path1;
    PathPtr path2;
    PathPtr path3;
    V3 node;
    V3 v1;
    V3 v2;
    int i;
    char* r3;
    void* f;
    char* spawned;

    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_02114d38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x350, f, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d20);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d30);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d28);

    mPathID = mParam & 0xff;
    mVariant = (mParam >> 8) & 0xf;
    unk_414 = (mParam >> 0xc) & 0xf;
    if (mVariant == 0xff)
        mVariant = 0;
    if (mPathID < 0)
        mPathID = 0;

    _ZN7PathPtrC1Ev(&path1);
    _ZN7PathPtr6FromIDEj(&path1, mPathID);
    unk_40c = _ZNK7PathPtr8NumNodesEv(&path1);

    unk_3f0 = mPosX;
    unk_3f4 = mPosY;
    unk_3f8 = mPosZ;
    unk_0a0 = -0x1e000;
    v1 = data_ov016_02114d4c;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        ((char*)this) + 0x110, ((char*)this), &v1, 0x32000, 0x50000, 0x200004, 0);
    v2 = data_ov016_02114d4c;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        ((char*)this) + 0x150, ((char*)this), &v2, 0x32000, 0x50000, 0x200000, 0);

    _ZN7PathPtrC1Ev(&path2);
    _ZN7PathPtr6FromIDEj(&path2, mPathID);
    unk_410 = 1;
    mStarUniqueID = 0;
    unk_3ac = 0x1000;

    if (data_0209f220 == 1)
        goto check_param2;
    if (IsStarCollected(SublevelToLevel(8), 1) != 0)
        goto stage2_path;

check_param2:
    if (mVariant != 2)
        goto ret0_a;
    /* u64-mask forces add r2,r4,#0x3f4 materialization (ROM shape) */
    *(int*)(((int)((char*)this) + 0x3f4)) -= 0x80000;
    unk_410 = 8;
    if (unk_410 >= unk_40c)
        unk_410 = 4;
    mPosY = unk_3f4;
    func_ov016_02111bf0(((char*)this), &data_ov016_02114d8c);
    goto tail;
ret0_a:
    return 0;

stage2_path:
    if (data_0209f220 == 2)
        goto check_param1;
    if (IsStarCollected(SublevelToLevel(8), 2) != 0)
        goto check_param0;

check_param1:
    if (mVariant != 1)
        goto ret0_b;
    spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0xb2, unk_414 | 0x50, ((char*)this) + 0x5c, 0,
        mAreaId, -1);
    if (spawned != 0) {
        mStarUniqueID = *(int*)(spawned + 4);
        *(int*)(spawned + 0xb0) = 0;
        _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0, 0x3e8000, 0x1f40000, 0x1f40000);
    }
    func_ov016_02111bf0(((char*)this), &data_ov016_02114d8c);
    goto tail;
ret0_b:
    return 0;

check_param0:
    if (mVariant != 0)
        goto ret0_c;
    _ZN7PathPtrC1Ev(&path3);
    _ZN7PathPtr6FromIDEj(&path3, mPathID);
    _ZNK7PathPtr7GetNodeER7Vector3j(&path3, ((char*)this) + 0x5c, 0);
    _ZNK7PathPtr7GetNodeER7Vector3j(&path3, ((char*)this) + 0x5c, 1);
    unk_094 = Vec3_HorzAngle(((char*)this) + 0x5c, (char*)&node);
    unk_08e = unk_094;
    func_ov016_02111bf0(((char*)this), &data_ov016_02114dbc);
    goto tail;
ret0_c:
    return 0;

tail:
    r3 = ((char*)this);
    for (i = 0; i < 7; i++) {
        /* array index form -> add r0,r4,ip,lsl#1 (not strength-reduced i*2) */
        ((short*)((char*)&unk_418))[i] = 0;
        ((short*)((char*)&unk_426))[0] = 0;
        *(int*)(r3 + 0x448) = mPosX;
        *(int*)(r3 + 0x44c) = mPosY;
        *(int*)(r3 + 0x450) = mPosZ;
        r3 += 0xc;
    }
    unk_428 = unk_08c;
    unk_42a = unk_08e;
    unk_42c = unk_090;
    return 1;
}
