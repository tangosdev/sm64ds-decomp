//cpp
// @symbol _ZN16daObjCtMecha05_c13InitResourcesEv
// recovered name: daObjCtMecha05_c_InitResources
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
#include "daObjCtMecha05_c.h"

/* daObjCtMecha05_c::InitResources - recovered from vtable slot identity.
 *
 * Every ROM symbol below is spelled by its already-mangled name -- an
 * ordinary (non-"C") declaration would mangle it a SECOND time and
 * eligible.py's reference check would flag the doubly-mangled name as
 * unresolvable. void* everywhere, mirroring the un-migrated
 * func_ov065_0211b1d4 this file replaces and daObjCtMecha03_c's own
 * InitResources, which took the same route rather than calling through the
 * real Model/ShadowModel/dBgW_KcMbg members now reachable via
 * daObjCtMecha05_c.h -- a class-typed by-value Fix12<int> argument is not
 * safe to assume matches the ROM through a real method call (see the
 * warning in include/dBgActor_c.h and include/dActor_c.h about CW homing
 * class-typed by-value parameters to the stack).
 *
 * ModelLoadFile, MeshColliderLoadFile and UpdatePosWithTransformSym WERE
 * phantom references -- names no module defines, which the byte gate cannot
 * see because match.py compares relocated words as wildcards. This branch read
 * the cartridge and established the real ROM symbols behind them, so the calls
 * below name those directly -- the same idiom daObjCtMecha03_c::InitResources
 * (its sibling in this class's own migration) and SignPost::InitResources
 * (#1554) already use. decl_common.h no longer declares the phantoms.
 *
 * The CLPS_Block symbol at 0x02112258 is the same kind of shared,
 * locally-unowned reference the daObjCtMecha03_c pass found at 0x02112198:
 * an "ambiguous" data symbol declared by twelve OTHER overlays' own
 * symbols.txt but not ov065's, and adding an ov065 entry for it would drag
 * this overlay's own computed base address, and every other ov065 symbol's
 * offset, out of range (tools/modules.py derives an overlay's base from its
 * lowest symbol).
 *
 * SETTLED to ov035 by tools/overlay_residency.py, the same as
 * daObjCtMecha03_c::InitResources's own site: of the twelve candidate
 * overlays (ov009/010/012/013/024/033/035/043/047/052/053/056), residency's
 * loader/level constraints leave exactly one able to coexist with ov065.
 * Spelled as ov035's own symbol name, data_ov035_02112258, so the
 * `complete` marker below is now warranted.
 */
extern "C" {
extern void *_ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisp, void *bmd, int a, int b);
extern void _ZN11ShadowModel10InitCuboidEv(void *thisp);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *thisp);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *thisp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thisp, void *kcl, void *mtx, int fix, short sh, void *clps);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
extern void func_020393d4(void *p, void *v);
extern void func_020393a4(void *p, int v);
extern void func_02039394(void *p, int v);
}

struct BMD_File; struct KCL_File; struct dActor_c; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
extern "C" unsigned char data_0209f2c0;
extern "C" int data_ov035_02112258;
struct dBgCh_Gnd {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[3];      // pad to 0x54 total
    dBgCh_Gnd();
    void SetObjAndPos(const Vector3 &v, dActor_c *a);
    int DetectClsn();
    ~dBgCh_Gnd();
};

struct V3 { int x, y, z; };

int daObjCtMecha05_c::InitResources()
{
    char *self = (char *)this;

    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov065_0211d904);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, mf, 1, -1);
    _ZN11ShadowModel10InitCuboidEv(self + 0x33c);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);
    void *kf = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov065_0211d90c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, kf, self + 0x2ec, 0x199, *(short*)(self + 0x8e), (void*)&data_ov035_02112258);
    func_020393d4(self + 0x124,
        (void*)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393a4(self + 0x124, 0x1c0000);
    func_02039394(self + 0x124, 0x1000);
    *(short*)(self + 0x94) = *(short*)(self + 0x8e);
    *(int*)(self + 0x320) = *(int*)(self + 0x5c);
    *(int*)(self + 0x324) = *(int*)(self + 0x60);
    *(int*)(self + 0x328) = *(int*)(self + 0x64);
    *(short*)(self + 0x334) = (&data_ov065_0211c0c8)[data_0209f2c0];

    V3 v;
    v.x = *(int*)(self + 0x5c);
    v.y = *(int*)(self + 0x60);
    v.z = *(int*)(self + 0x64);
    v.y = v.y - 0xa000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(Vector3*)&v, (dActor_c*)0);
    *(int*)(self + 0x338) = v.y;
    if (rg.DetectClsn() != 0)
        *(int*)(self + 0x338) = rg.result;
    return 1;
}
