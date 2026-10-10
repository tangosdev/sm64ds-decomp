//cpp
/* daPropeller_Heyho_Fire_c -- the propeller Shy Guy's fireball (registry
 * profile PROPELLER_HEYHO_FIRE).
 *
 * A short-lived projectile. InitResources binds the shared model, sizes the
 * hit cylinder and the wall-and-floor probe, and installs the state at
 * 0x02111190 through func_ov002_020fed2c, which stores the state pointer at
 * +0x350 and runs the state's first handler (func_ov002_020fed18 starts
 * mStateTimer at 200). Behavior counts mStateTimer down, runs the current
 * state's main handler, falls under gravity and rebuilds the model matrix
 * (func_ov002_020fed7c). The main handler func_ov002_020fec94 bursts the fireball in a puff when its timer runs out
 * or it touches ground or a wall, and otherwise asks func_ov002_020feb50
 * whether it hit the player: when the actor whose ID is at +0x134 is the
 * player (type 0xbf) and not vanished, the fireball bursts and hurts it --
 * unless the player is metal, which turns the fireball round instead.
 *
 * This file is the whole linker unit 0x020feabc..0x020ff014, 13 functions:
 * D1 and D0 (src/actors/dM3dGLin.cpp ends exactly at 0x020feabc below
 * them), the five helpers func_ov002_020feb50 through func_ov002_020fed7c,
 * CleanupResources, OnPendingDestroy, Render, Behavior, InitResources, and
 * last the registry factory daPropeller_Heyho_Fire_c_classInit
 * (0x020fefcc), which closes ov002's .text at 0x020ff014. The out-of-line
 * destructor is the key function, so this TU also emits the vtable and the
 * RTTI.
 *
 * It replaces the one-function sources for
 * _ZN24daPropeller_Heyho_Fire_cD1Ev, _ZN24daPropeller_Heyho_Fire_cD0Ev,
 * func_ov002_020feb50, func_ov002_020fec94, func_ov002_020fed18,
 * func_ov002_020fed2c, func_ov002_020fed7c,
 * _ZN24daPropeller_Heyho_Fire_c16CleanupResourcesEv,
 * _ZN24daPropeller_Heyho_Fire_c16OnPendingDestroyEv,
 * _ZN24daPropeller_Heyho_Fire_c6RenderEv,
 * _ZN24daPropeller_Heyho_Fire_c8BehaviorEv,
 * _ZN24daPropeller_Heyho_Fire_c13InitResourcesEv and
 * daPropeller_Heyho_Fire_c_classInit. Each member keeps the provenance
 * notes its source carried.
 *
 * `#pragma defer_codegen off` keeps this file in ROM order.
 *
 * Leftover: the helpers keep their C-ABI cartridge names and reach the
 *   fireball through raw offsets; daPropeller_Heyho_Fire_c.h names only the
 *   fields Behavior and InitResources use.
 * Leftover: the callees with Fix12<int> parameters (dCcAc_c::Init,
 *   dBgCh_Actr::Init, Player::Hurt) stay spelled as mangled extern-C free
 *   functions. A real method call homes a class-typed by-value argument and
 *   size-DIFFs the caller (notes/mwccarm-codegen.md 6az).
 */

#pragma defer_codegen off

#include "daPropeller_Heyho_Fire_c.h"
#include "types.h"
#include "common.h"
#include "SharedFilePtr.h"

struct BMD_File;

/* The scratch matrix at 0x020a0e68 as func_ov002_020fed7c copies it: twelve
   plain words. A struct copy of the C++ Matrix4x3 does not reproduce it. */
struct M48 { int w[12]; };

/* func_ov002_020fed2c's view of the fireball: the state pointer at 0x350,
   aimed at a table of int-returning member-function pointers. The receiver
   is completed after the pointer type is formed, as the legacy shard did. */
struct C; typedef int (C::*PMF)();
struct C { char pad[0x350]; PMF *pp; };

/* Render's view of the model at 0x300: slot 5 of its vtable draws it. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0x300]; Base base; };

extern "C" {
/* Defined below. */
void func_ov002_020feb50(char *self);
int func_ov002_020fec94(void *c);
int func_ov002_020fed18(char *p);
int func_ov002_020fed2c(C *c, PMF *p);
void func_ov002_020fed7c(char *c);

/* Was spelled `func_0211d610`, a name no symbols.txt defines. ov002's relocs
   record both loads (0x020fee10, 0x020fefc0) as `overlays(65,75)`; ov065's
   row there is the one unambiguous `kind:bss`, and CleanupResources already
   named it. It is the fireball's shared model file. */
extern int data_ov065_0211d610[];
extern M48 data_020a0e68;

/* Two handler words each. InitResources installs the copy at data_ov002_02111190.
 * Both loads of a pair happen before either store; an `{int a, b}` copy does not. */
struct HeyhoFnPair { int w[2]; };
extern HeyhoFnPair data_ov002_0210d600;
extern HeyhoFnPair data_ov002_0210d5f8;
struct HeyhoStateTable {
    HeyhoFnPair a;
    HeyhoFnPair b;
};
extern HeyhoStateTable data_ov002_02111190;

extern char *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN5Sound4PlayEjjRK7Vector3(unsigned int a, unsigned int b, Vector3 const &v);
extern void _ZN8dActor_c13SmallPoofDustEv(void *self);
extern void func_02012694(unsigned int a, Vector3 const &v);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char *self, Vector3 const &v, unsigned int a, int b, unsigned int c, unsigned int d, unsigned int e);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, short rx, short ry, short rz);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char *self, BMD_File *f, int a, int b);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(char *self, dActor_c *a, int r, int h, u32 f1, u32 f2);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(char *self, dActor_c *a, int r, int h, Vector3_16 *rot, int f);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0-1 -- _ZN24daPropeller_Heyho_Fire_cD1Ev, 0x020feabc, size 0x40 */
/*                     _ZN24daPropeller_Heyho_Fire_cD0Ev, 0x020feafc, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN24daPropeller_Heyho_Fire_cD1Ev
// @symbol _ZN24daPropeller_Heyho_Fire_cD0Ev
/* One native destructor definition emits both ROM variants. D1 is one
 * vtable store and a destructor call per member, every one a consequence of
 * `struct daPropeller_Heyho_Fire_c : dEnemyBase_c` and the members that
 * declaration types, destroyed in reverse declaration order, then
 * dEnemyBase_c::~dEnemyBase_c. D0 is the same destruction followed by the
 * inherited actor-heap operator delete -- dEnemyBase_c's, reachable because
 * it is this class's immediate base. This body is the evidence for the
 * header: each member's size closes exactly on the next one's offset. Being
 * the first out-of-line virtual, it makes this file the key function's
 * home, so the vtable and RTTI are emitted here too. */
daPropeller_Heyho_Fire_c::~daPropeller_Heyho_Fire_c()
{
}

#ifdef _MSC_VER
/* The host uses the flat ROM D0 name, which MSVC never emits: it folds the
 * Itanium destructor variants into the one ~daPropeller_Heyho_Fire_c()
 * above. This arm spells out what the deleting destructor does -- the D1
 * body, called qualified so it is a direct call, then the class-specific
 * operator delete. Nothing here reaches mwccarm. */
extern "C" daPropeller_Heyho_Fire_c *_ZN24daPropeller_Heyho_Fire_cD0Ev(daPropeller_Heyho_Fire_c *thiz)
{
    thiz->daPropeller_Heyho_Fire_c::~daPropeller_Heyho_Fire_c();
    daPropeller_Heyho_Fire_c::operator delete(thiz);
    return thiz;
}
#endif

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020feb50, 0x020feb50, size 0x144 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020feb50
/* recovered: shared common types */
extern "C" void func_ov002_020feb50(char* self) {
    char* a;
    int b;
    int flags;
    Vector3 v;

    if (*(s32*)(self + 0x134) == 0) {
        return;
    }
    a = _ZN8dActor_c10FindWithIDEj(*(s32*)(self + 0x134));
    if (a == 0) {
        return;
    }
    b = (*(u16*)(a + 0xc) == 0xbf);
    if (b == 0) {
        return;
    }
    if (*(u8*)(a + 0x6fb) != 0) {
        return;
    }
    if (*(u8*)(a + 0x6f9) != 0) {
        *(s32*)(self + 0x354) = 1;
        _ZN5Sound4PlayEjjRK7Vector3(0, 0xa5, *(Vector3*)(self + 0x74));
        {
            s16* p = (s16*)(((int)self + 0x94));
            *p = *p - 0x8000;
        }
        *(s32*)(self + 0xa4) = 0;
        *(s32*)(self + 0xa8) = 0x1e000;
        *(s32*)(self + 0xac) = 0;
        *(s32*)(self + 0x9c) = -0x4000;
        return;
    }
    flags = *(s32*)(self + 0x130);
    _ZN8dActor_c13SmallPoofDustEv(self);
    func_02012694(0x166, *(Vector3*)(self + 0x74));
    _ZN7fBase_c18MarkForDestructionEv(self);
    if ((flags & 0x10) != 0) {
        return;
    }
    v.x = *(s32*)(self + 0x5c);
    v.y = *(s32*)(self + 0x60);
    v.z = *(s32*)(self + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, v, 1, 0xc000, 1, 0, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020fec94, 0x020fec94, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020fec94
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020fec94(void* c) {
  if (*(unsigned short*)((char*)c+0x100)==0
      || _ZNK10dBgCh_Actr10IsOnGroundEv((char*)c+0x144)
      || _ZNK10dBgCh_Actr8IsOnWallEv((char*)c+0x144)) {
    _ZN8dActor_c13SmallPoofDustEv(c);
    func_02012694(0x166, *(Vector3*)((char*)c+0x74));
    _ZN7fBase_c18MarkForDestructionEv(c);
    return 1;
  }
  if (*(int*)((char*)c+0x354)==0) func_ov002_020feb50((char*)c);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020fed18, 0x020fed18, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020fed18
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020fed18(char *p)
{
    *(short *)(p + 0x100) = 200;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov002_020fed2c, 0x020fed2c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020fed2c
extern "C" int func_ov002_020fed2c(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov002_020fed7c, 0x020fed7c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020fed7c
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020fed7c(char *c) {
    int v[3];
    Vec3_Asr(v, c+0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
    *(M48*)(c+0x31c) = data_020a0e68;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN24daPropeller_Heyho_Fire_c16CleanupResourcesEv, 0x020fedf0, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN24daPropeller_Heyho_Fire_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 1 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
int daPropeller_Heyho_Fire_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov065_0211d610)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN24daPropeller_Heyho_Fire_c16OnPendingDestroyEv, 0x020fee14, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN24daPropeller_Heyho_Fire_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daPropeller_Heyho_Fire_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN24daPropeller_Heyho_Fire_c6RenderEv, 0x020fee18, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN24daPropeller_Heyho_Fire_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daPropeller_Heyho_Fire_c::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN24daPropeller_Heyho_Fire_c8BehaviorEv, 0x020fee44, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN24daPropeller_Heyho_Fire_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* This file used to carry `struct dActor_c { char pad[0x350]; Holder* h; }` and
 * cast `this` to it. That stand-in was how the state pointer at 0x350 was
 * reached; daPropeller_Heyho_Fire_c.h declares it now, so the cast and the three dummy structs
 * are gone.
 */
int daPropeller_Heyho_Fire_c::Behavior()
{
    DecIfAbove0_Short((unsigned short*)&mStateTimer);
    State* h = mCurrentState;
    /* Reads the handler's pointer word directly rather than as `&h->mMain`:
       taking the ADDRESS of a pointer-to-member makes mwcc materialise the
       whole 8-byte pmf. Reading one to CALL it is free. */
    if (*(int*)((char*)h + 8) != 0) {
        (this->*(h->mMain))();
    }
    {
        /* Gravity, clamped at terminal velocity. unk_0ac is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int spd = mVertSpeed;
        int pos = mVertAccel;
        int lim = mTerminalVelocity;
        int ac = unk_0ac;
        int np = spd + pos;
        if (np >= lim) lim = np;
        mVertSpeed = lim;
        unk_0ac = ac;
        UpdatePosWithOnlySpeed(&mdCcAc_c);
    }
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleY = mPrevAngleY;
    func_ov002_020fed7c((char*)this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN24daPropeller_Heyho_Fire_c13InitResourcesEv, 0x020feef4, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN24daPropeller_Heyho_Fire_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
int daPropeller_Heyho_Fire_c::InitResources()
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211d610);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x300, f, 1, -1) == 0) return 0;
    unk_358 = param1 & 0xff;
    if (unk_358 > 1) unk_358 = 0;
    mTerminalVelocity = -0x64000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this)+0x110, (struct dActor_c*)((char*)this), 0xa000, 0xa000, 0x200004, 0);
    mAngleY = mPrevAngleY;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x144, (struct dActor_c*)((char*)this), 0xa000, 0xa000, 0, 0);
    func_ov002_020fed2c((C *)this, (PMF *)&data_ov002_02111190);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- daPropeller_Heyho_Fire_c_classInit, 0x020fefcc, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol daPropeller_Heyho_Fire_c_classInit
/* recovered: vtable identified, globals resolved */
/* Reconstructed source-style name: SM64DS proves daPropeller_Heyho_Fire_c through RTTI,
 * allocation size, vtable identity, and the PROPELLER_HEYHO_FIRE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Bullet_Spawn. `new daPropeller_Heyho_Fire_c`
 * is the whole sequence: fBase_c::operator new(0x35c), dEnemyBase_c's C2,
 * the vtable store, then the dCcAc_c, dBgCh_Actr and Model constructors in
 * declaration order. */
extern "C" daPropeller_Heyho_Fire_c *daPropeller_Heyho_Fire_c_classInit(void)
{
    return new daPropeller_Heyho_Fire_c;
}

/* Retail copy order: data_ov002_0210d600, then data_ov002_0210d5f8.
 * mwcc emits __sinit_daPropeller_Heyho_Fire_c.cpp. */
HeyhoStateTable data_ov002_02111190 = { data_ov002_0210d600, data_ov002_0210d5f8 };
