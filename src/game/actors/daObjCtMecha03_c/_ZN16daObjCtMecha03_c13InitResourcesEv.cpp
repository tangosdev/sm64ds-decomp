//cpp
// @symbol _ZN16daObjCtMecha03_c13InitResourcesEv
// recovered name: daObjCtMecha03_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjCtMecha03_c::InitResources - recovered from vtable slot identity */
#include "daObjCtMecha03_c.h"
#include "dBgW.h"

/* Every ROM symbol below is spelled by its already-mangled name: an ordinary
   (non-"C") declaration would mangle it a SECOND time (_Z35_ZN5Model... instead
   of _ZN5Model...) and eligible.py's reference check flags the doubly-mangled
   name as unresolvable -- it is not in any symbols.txt, because nothing ever
   defines it. void* everywhere, mirroring PyramidTop::InitResources and the
   other InitResources siblings that call these same functions. */
extern "C" {
extern int func_ov065_0211a114(char *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisp, void *bmd, int a, int b);
extern void _ZN11ShadowModel10InitCuboidEv(void *thisp);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *thisp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thisp, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393a4(void *p, int v);
extern void func_02039394(void *p, int v);
extern void func_020393d4(void *p, void *v);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
}

extern int data_ov065_0211d88c[];
extern int data_ov065_0211d894[];
/* Same address as the "ambiguous" data_ovNNN_02112198 in every other overlay
   that reaches this InitResources shape (PyramidTop's ov024, etc.) -- a
   shared CLPS_Block. It is NOT ov065's own: config/arm9/overlays/ov065's
   lowest symbol is ~0x02115ee0, and tools/modules.py derives an overlay's
   base from its lowest symbol, so adding an ov065 entry here at 0x02112198
   would drag every other ov065 address out of range.

   SETTLED to ov035 by tools/overlay_residency.py: of the eleven overlays
   whose own symbols.txt declares an "ambiguous" entry at this address
   (ov012/013/024/033/035/043/045/047/052/053/056), residency's loader/level
   constraints rule out every one but ov035 as able to coexist with ov065 --
   `settle()` returns exactly one survivor. Spelled as ov035's own symbol
   name, data_ov035_02112198, rather than a bare unqualified extern. */
extern int data_ov035_02112198;
extern unsigned char data_0209f2c0;
extern short data_ov065_0211c0b0[];

int daObjCtMecha03_c::InitResources()
{
    char *self = (char *)this;
    void *bmd;
    void *kcl;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211d88c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((void *)(self + 0xd4), bmd, 1, -1);
    _ZN11ShadowModel10InitCuboidEv((void *)(self + 0x330));

    mSwingDir = 1;
    mSwingAccel = data_ov065_0211c0b0[data_0209f2c0];
    mSwingAngle = 0x1964;
    mAngleZ = mSwingAngle;

    func_ov065_0211a114(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);

    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov065_0211d894);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (void *)(self + 0x124), kcl, (void *)(self + 0x2ec), 0x1000,
        mAngleY, (void *)&data_ov035_02112198);

    func_020393a4((void *)(self + 0x124), 0x300000);
    func_02039394((void *)(self + 0x124), -0x200000);

    if (data_0209f2c0 != 3) {
        func_020393d4((void *)(self + 0x124),
            (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    }
    return 1;
}
