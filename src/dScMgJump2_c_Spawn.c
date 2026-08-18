#include "types.h"
void *_ZN7fBase_cnwEj(u32 sz);
void func_ov004_020b2adc(void *p);
void func_020733a8(void *o, int a, int b, void *f1, void *f2);
void _ZN8Particle10SysTrackerC1Ev(void *p);
void _ZN5ModelC1Ev(void *p);
extern char _ZTV14dScMgD3DBase_c[];
extern char _ZTV12dScMgJump2_c[];
void func_ov006_020c893c();
void func_ov006_020c8a04();
void func_ov006_020c6f70();
void func_ov006_020eed64();
void func_ov006_020efc08();
void func_ov006_020c6f3c();

// @symbol dScMgJump2_c_Spawn
/* recovered: vtable identified. Formerly mangled _ZN8PathLift17BaseInitResourcesEv --
   a name matched across an OVERLAY BOUNDARY at a near-identical address to the real
   PathLift (ov002, _ZN8PathLiftD1Ev and friends), neither this class's method nor a
   BaseInitResources. This body is operator new(0x5a78), dScMgD3DBase_c's
   constructor, dScMgD3DBase_c's vtable, mSysTracker, then dScMgJump2_c's own vtable
   and every member -- a factory for dScMgJump2_c. Renamed per the fallback
   convention (no known game object name for this minigame; compare
   daObjEmmLog_c_Spawn, the tree's other `<ClassName>_Spawn` factory) rather than
   the game-name-based MgBounceAndPounce_Spawn that dScMgJump_c's own factory
   carries. See include/dScMgJump2_c.h. */
void *dScMgJump2_c_Spawn()
{
    char *p = (char *)_ZN7fBase_cnwEj(0x5a78);
    if (p) {
        char *e;
        char *end;
        func_ov004_020b2adc(p);
        *(char **)p = _ZTV14dScMgD3DBase_c;
        *(u16 *)(p + 0x4664) = 0;
        e = p + 0x466c;
        end = p;
        end = end + 0x47e4;
        do {
            e += 0xbc;
        } while (e != end);
        _ZN8Particle10SysTrackerC1Ev(p + 0x47e4);
        *(char **)p = _ZTV12dScMgJump2_c;
        func_020733a8(p + 0x500c, 3, 0xb8, (void *)func_ov006_020c8a04, (void *)func_ov006_020c893c);
        func_020733a8(p + 0x5234, 6, 0xf0, (void *)func_ov006_020c6f70, (void *)func_ov006_020c6f3c);
        func_020733a8(p + 0x57d4, 0x10, 0x24, (void *)func_ov006_020efc08, (void *)func_ov006_020eed64);
        _ZN5ModelC1Ev(p + 0x5a14);
    }
    return p;
}
