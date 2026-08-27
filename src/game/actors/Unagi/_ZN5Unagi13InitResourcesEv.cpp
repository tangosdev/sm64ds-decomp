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
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(char* self, char* actor, struct V3* pos, int r3, int sp0, int sp4, int sp8);
extern int IsStarCollected(int a, int b);
extern void func_ov016_02111bf0(char* c, void* p);
extern char* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, char* pos, void* d, int sp0, int sp4);
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(char* a, int r1, int r2, int r3, int sp0);
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
    void* f;
    char* spawned;

    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_02114d38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x350, f, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d20);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d30);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d28);

    mPathID = param1 & 0xff;
    mVariant = (param1 >> 8) & 0xf;
    mStarParam = (param1 >> 0xc) & 0xf;
    if (mVariant == 0xff)
        mVariant = 0;
    if (mPathID < 0)
        mPathID = 0;

    _ZN7PathPtrC1Ev(&path1);
    _ZN7PathPtr6FromIDEj(&path1, mPathID);
    mPathNodeCount = _ZNK7PathPtr8NumNodesEv(&path1);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mTerminalVelocity = -0x1e000;
    v1 = data_ov016_02114d4c;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        ((char*)this) + 0x110, ((char*)this), &v1, 0x32000, 0x50000, 0x200004, 0);
    v2 = data_ov016_02114d4c;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        ((char*)this) + 0x150, ((char*)this), &v2, 0x32000, 0x50000, 0x200000, 0);

    _ZN7PathPtrC1Ev(&path2);
    _ZN7PathPtr6FromIDEj(&path2, mPathID);
    mPathNodeIndex = 1;
    mStarUniqueID = 0;
    mBlendModelAnim.speed = 0x1000;

    if (data_0209f220 == 1)
        goto check_param2;
    if (IsStarCollected(SublevelToLevel(8), 1) != 0)
        goto stage2_path;

check_param2:
    if (mVariant != 2)
        goto ret0_a;
    /* u64-mask forces add r2,r4,#0x3f4 materialization (ROM shape) */
    *(int*)(((int)((char*)this) + 0x3f4)) -= 0x80000;
    mPathNodeIndex = 8;
    if (mPathNodeIndex >= mPathNodeCount)
        mPathNodeIndex = 4;
    mPosY = mHomePosY;
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
    spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0xb2, mStarParam | 0x50, ((char*)this) + 0x5c, 0,
        mAreaId, -1);
    if (spawned != 0) {
        mStarUniqueID = *(int*)(spawned + 4);
        *(int*)(spawned + 0xb0) = 0;
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0, 0x3e8000, 0x1f40000, 0x1f40000);
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
    mPrevAngleY = Vec3_HorzAngle(((char*)this) + 0x5c, (char*)&node);
    mAngleY = mPrevAngleY;
    func_ov016_02111bf0(((char*)this), &data_ov016_02114dbc);
    goto tail;
ret0_c:
    return 0;

tail:
    for (i = 0; i < 7; i++) {
        /* array index form -> add r0,r4,ip,lsl#1 (not strength-reduced i*2) */
        mSegmentAngle[i] = 0;
        mSegmentAngle[7] = 0;
        mSegmentPos[i].x = mPosX;
        mSegmentPos[i].y = mPosY;
        mSegmentPos[i].z = mPosZ;
    }
    mInitAngleX = mAngleX;
    mInitAngleY = mAngleY;
    mInitAngleZ = mAngleZ;
    return 1;
}
