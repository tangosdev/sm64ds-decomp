//cpp
// @symbol _ZN8MantaRay13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Loads the model and animation, unpacks the spawn word, caches the path, and
 * places the ray on it.
 *
 * mParam is TWO fields in one word: the low byte is the path ID and bits 12-15
 * are unk_388. The path ID is clamped at zero afterwards -- a signed read of a
 * masked byte cannot go negative, so that test is dead as written and kept as
 * the ROM has it.
 *
 * unk_380 caches the node count once here; Behavior wraps mPathNode against it
 * every frame rather than re-asking the path. The ray is then STARTED AT NODE
 * 1, not 0, and its position comes from the path rather than the spawn point.
 *
 * Two of the four files it claims live in ov002, not this overlay -- shared
 * assets it still has to release itself (see CleanupResources).
 *
 * One level (data_0209f2d8 == 2) overrides all of it: node 3, a fixed heading,
 * and a hardcoded position. That is a scripted placement, not a spawn.
 */
#include "MantaRay.h"
struct SharedFilePtr;
struct BMD_File;
struct Actor;

struct PathPtr {
    char pad[8];
    PathPtr();
    void FromID(unsigned int id);
    int NumNodes() const;
    void GetNode(Vector3& out, unsigned int idx) const;
};

extern "C" {
BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& p);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
void _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr& p);
void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* self, Actor* a, const Vector3& v, int b, int c, unsigned int d, unsigned int e);
void func_ov090_02132ac4(unsigned char* c, void* p);

extern SharedFilePtr data_ov090_02134524;
extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov090_0213452c;
extern Vector3 data_ov090_02134200;
extern int data_ov090_0213454c;
extern unsigned char data_0209f2d8;
}

int MantaRay::InitResources()
{
    unsigned char* thiz = (unsigned char*)this;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, _ZN5Model8LoadFileER13SharedFilePtr(data_ov090_02134524), 1, -1);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da10);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a8);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_0213452c);

    unk_37c = mParam & 0xff;
    unk_388 = (*(unsigned int*)&mParam >> 0xc) & 0xf;
    if (unk_37c < 0) unk_37c = 0;

    {
        PathPtr pp;
        pp.FromID(*(unsigned int*)&unk_37c);
        unk_380 = pp.NumNodes();
    }

    unk_0a0 = -0x3c000;

    {
        Vector3 v;
        v.x = data_ov090_02134200.x;
        v.y = data_ov090_02134200.y;
        v.z = data_ov090_02134200.z;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(&mMovingCylinderClsnWithPos, (Actor*)thiz, v,
            0x150000, 0xc8000, 0x200004, 0);
    }

    {
        PathPtr pp;
        pp.FromID(*(unsigned int*)&unk_37c);
        mPathNode = 1;
        pp.GetNode(*(Vector3*)&mPosX, *(unsigned int*)&mPathNode);
    }

    {
        int b = (int)(data_0209f2d8 == 2);
        if (b != 0) {
            mPathNode = 3;
            mPrevAngleX = (short)0xf303;
            mPrevAngleY = 0xb50;
            mPrevAngleZ = 0;
            mPosX = (int)0xfdfb8000;
            mPosY = (int)0xff8f8000;
            mPosZ = 0x29a000;
            unk_0b0 = 0;
        }
    }

    func_ov090_02132ac4(thiz, &data_ov090_0213454c);
    return 1;
}
