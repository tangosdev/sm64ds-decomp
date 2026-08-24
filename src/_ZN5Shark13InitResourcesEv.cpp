//cpp
// @symbol _ZN5Shark13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Shark.h"
#include "PathPtr.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* file, int a, int b);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* f);
extern void _ZN7PathPtrC1Ev(void* thiz);
extern void _ZN7PathPtr6FromIDEj(void* thiz, unsigned int id);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void* thiz, void* actor, void* pos, int f, int g, unsigned int h, unsigned int i);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* thiz, void* out, unsigned int j);

extern char data_ov090_021345ac[];

}

int Shark::InitResources()
{
    /* RAW STORAGE, not `PathPtr p1;`: PathPtr now has a DECLARED constructor
       (_ZN7PathPtrC1Ev.cpp, see notes/ctor-migration.md), so typed locals
       would each grow an implicit construction bl at their declaration point
       and then be constructed AGAIN by the hand calls below. The ROM
       interleaves these constructions with the surrounding work (p1 before
       the collision init, p2 after it), which typed locals cannot express --
       so the storage stays dumb and the calls stay explicit. */
    u32 p1_storage[sizeof(PathPtr) / sizeof(u32)];
    u32 p2_storage[sizeof(PathPtr) / sizeof(u32)];
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x30c,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov090_021345a4), 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_021345ac);
    mPathID = (*(s32 *)&param1) & 0xff;
    if (mPathID < 0) mPathID = 0;
    _ZN7PathPtrC1Ev(p1_storage);
    _ZN7PathPtr6FromIDEj(p1_storage, mPathID);
    unk_38c = _ZNK7PathPtr8NumNodesEv(p1_storage);
    mTerminalVelocity = -0x3c000;
    unk_374 = 0;
    unk_378 = 0;
    unk_37c = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        ((char*)this) + 0x110, ((char*)this), ((char*)this) + 0x374, 0x42000, 0x6e000, 0x200004, 0);
    _ZN7PathPtrC1Ev(p2_storage);
    _ZN7PathPtr6FromIDEj(p2_storage, mPathID);
    mPathNodeIdx = 1;
    _ZNK7PathPtr7GetNodeER7Vector3j(p2_storage, ((char*)this) + 0x5c, mPathNodeIdx);
    func_ov090_021338b4(((char*)this), data_ov090_021345cc);
    return 1;
}
