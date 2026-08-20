//cpp
// @symbol _ZN9PushBlock13InitResourcesEv
/* recovered: vtable identified, shared common types, declarations from a shared header
 *
 * PushBlock::InitResources -- vtable slot 0, ov002 0x020b8fe0. Attributed by
 * the vtable: _ZTV9PushBlock (ov002 0x021096b0) carries this address at slot
 * 0, where _ZTV10dBgActor_c carries fBase_c's generic 0x02043c80.
 *
 * Kept as an extern "C" free function under the literal mangled name rather
 * than a real `PushBlock::InitResources()` method -- the same shape
 * include/dActor_c.h documents for fBase_c::InitResources and
 * src/_ZN13BigBrickBlock13InitResourcesEv.c use: `self` arrives as a raw
 * pointer and the body still reads/writes by hand offset, unconverted. The
 * mangled name alone is enough to fill the vtable slot correctly; nothing
 * here requires the class's true member syntax. */
#include "decl_common.h"
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern "C" KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *k, void *m, int fix, short s, void *clps);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *a, int b, int c, void *d, int e);

struct V3 { int x, y, z; };
struct dBgCh_Gnd { char buf[0x44]; int f44; char rest[8]; };
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *self, V3 *v, void *a);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);

extern SharedFilePtr data_ov002_0210df9c;
extern SharedFilePtr data_ov002_0210df94;

extern "C" int _ZN9PushBlock13InitResourcesEv(char *self)
{
    dBgCh_Gnd rg;
    V3 v;
    BMD_File *bmd;
    KCL_File *kcl;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210df9c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);
    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov002_0210df94);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, kcl, self + 0x2ec, 0x1000, *(short *)(self + 0x8e), &data_ov002_0210d7b4);

    *(int *)(self + 0x9c) = -0x2000;
    *(int *)(self + 0xa0) = -0x3c000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(self + 0x320, self, 0x96000, 0x96000, 0, 0);

    v.x = *(int *)(self + 0x5c);
    v.y = *(int *)(self + 0x60);
    v.z = *(int *)(self + 0x64);
    v.y = v.y + 0x14000;
    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, 0);
    *(int *)(self + 0x4e8) = v.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg))
        *(int *)(self + 0x4e8) = rg.f44;
    *(int *)(self + 0x4dc) = *(int *)(self + 0x5c);
    *(int *)(self + 0x4e0) = *(int *)(self + 0x60);
    *(int *)(self + 0x4e4) = *(int *)(self + 0x64);
    _ZN9dBgCh_GndD1Ev(&rg);
    return 1;
}
