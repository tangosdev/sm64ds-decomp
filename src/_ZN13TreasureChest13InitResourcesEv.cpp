//cpp
// @symbol _ZN13TreasureChest13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "TreasureChest.h"
struct SharedFilePtr { int x; };
struct BMD_File;
struct BCA_File;
struct dActor_c;
extern "C" {
BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, BMD_File*, int, int);
BCA_File* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, BCA_File*, int, int fix, unsigned int);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, dActor_c*, int fix, int t, unsigned int, unsigned int);
void func_ov064_0211a284(char* t);
unsigned char _ZN8dActor_c9TrackStarEjj(void* thiz, unsigned int, unsigned int);
}
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov064_0211c96c;
extern SharedFilePtr data_ov064_0211c964;

int TreasureChest::InitResources()
{
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a8);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov064_0211c96c), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0xd4,
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov064_0211c964), 0x40000000, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x138, (dActor_c*)((char*)this), 0x96000, 0x96000, 0x200004, 0);
    func_ov064_0211a284(((char*)this));
    mOrder = param1;
    mStarID = (unsigned int)param1 >> 8;
    if (mStarID != 0xff) {
        mTrackedStarID = _ZN8dActor_c9TrackStarEjj(((char*)this), mStarID, 2);
    }
    return 1;
}
