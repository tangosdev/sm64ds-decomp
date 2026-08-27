//cpp
// @symbol _ZN10BulletBill13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Loads both models, builds the shadow and both collision volumes, and starts
 * in state 0.
 *
 * It loads each file and then passes the pointer from the SharedFilePtr's own
 * +4 rather than the value it just got back -- so the load is really a claim,
 * and the file pointer is read out of the shared record afterwards.
 *
 * Any of the three setup steps failing returns 0 and abandons the spawn.
 *
 * The cylinder is anchored 0x50000 below the actor, the same offset Behavior
 * re-applies every frame.
 *
 * The sub-object arguments are cast to char*: the extern declarations here take
 * char*, and passing the typed member address without the cast does not compile
 * -- worth noting because it looks like a byte cost in a greedy sweep and is
 * not one. With the cast, all four are free.
 */
#include "BulletBill.h"
struct dActor_c; struct Vector3_16; struct BMD_File;
extern "C" struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(char* self);
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(char* self, struct dActor_c* a, struct Vector3* pos, int r, int h, u32 f1, u32 f2);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(char* self, struct dActor_c* a, int r, int h, struct Vector3_16* rot, int f);
extern "C" char data_ov079_02128300[];
extern "C" char data_ov079_021282f0[];

int BulletBill::InitResources()
{
    char* c = (char*)this;
    struct BMD_File* f1 = _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_02128300);
    struct BMD_File* f2 = _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_021282f0);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii((char*)&mModel1, *(struct BMD_File**)(data_ov079_02128300+4), 1, -1) == 0)
        return 0;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii((char*)&mModel2, *(struct BMD_File**)(data_ov079_021282f0+4), 1, -1) == 0)
        return 0;
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    {
        struct Vector3 v;
        v.x = 0; v.y = -0x50000; v.z = 0;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj((char*)&mdCcAcPos_c, (struct dActor_c*)c, &v, 0x78000, 0xa0000, 0x200005, 0x3c0);
    }
    mState = 0;
    mStateTimer = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_((char*)&mWithMeshClsn, (struct dActor_c*)c, 0x32000, 0, 0, 0);
    unk_3d8 = 0;
    return 1;
}
