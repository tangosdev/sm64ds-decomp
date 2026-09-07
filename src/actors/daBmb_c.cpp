//cpp
/* Production translation unit for ov102/daBmb_c -- the Bob-omb.
 *
 * SM64DS RTTI names this class daBmb_c: ov102 0x0214e4fc holds the bytes
 * "7daBmb_c\0", _ZTI7daBmb_c at 0x0214e508 points its +4 word back at that
 * string, and the vtable's -4 header word points at the _ZTI.  The tree used to
 * carry the class under the coined name BobOmb, which appears nowhere in the
 * cartridge; this file and include/daBmb_c.h use the cartridge's own spelling.
 *
 * The base is dEnemyBase_c, read from _ZTI7daBmb_c+8 -> 0x021081c0 in ov002.
 * Vtable LENGTH is NOT evidence here: _ZTV12dEnemyBase_c is 31 slots and so is
 * _ZTV7daBmb_c, so the "31 slots means dActor_c" heuristic gives the wrong
 * answer for this class.  daBmb_c declares no new virtual; it overrides nine
 * inherited slots -- 0, 3, 6, 9, 16, 17, 18, 19 and 29.
 *
 * THE DESTRUCTOR IS INLINE IN THE HEADER, AND DECLARED FIRST.  The cartridge
 * puts D1 at 0x0214a96c below D0 at 0x0214a9b4 and carries no D2 anywhere, which
 * is what mwccarm 2004/b56 emits for an inline in-class destructor; the
 * out-of-line form emits D2/D0/D1 in the wrong order plus a homeless D2.  The
 * typed member list in the header makes the empty body own the ShadowModel,
 * ModelAnim, dBgCh_Actr and dCcAc_c teardowns and the chain into
 * _ZN12dEnemyBase_cD2Ev.  With the destructor inline, OnYoshiTryEat is the first
 * out-of-line virtual the class declares -- the key function -- so this TU owns
 * the vtable and the RTTI group, and every one of those records is licensed
 * against a configured ov102 home.
 *
 * mwccarm emits one .text section per function in the REVERSE of source order,
 * so this file is written ROM-DESCENDING: daBmb_c_classInit (0x0214c6f8) first
 * and daBmb_c::OnAimedAtWithEgg (0x0214aa10) last.  D1 and D0 are
 * compiler-emitted from the inline body and land below everything written here,
 * at the bottom of the run, which is where the cartridge has them.  The TU
 * covers the whole contiguous linker run, 0x0214a96c..0x0214c748, 35 functions:
 * the 34 the tu_map cut reported plus the registry factory, which tu_map missed
 * only because it segments on symbol NAME and `daBmb_c_classInit` does not look
 * like a member.
 *
 * common.h is included FIRST on purpose.  daBmb_c.h reaches math/Matrix.h
 * through ModelAnim.h, and that header spells Matrix4x3 as `Matrix3x3 r;
 * Vector3 t;` where common.h spells it flat as `s32 m[12]`.  Two members
 * whole-struct-assign a Matrix4x3 -- InitResources into +0x394 and
 * func_ov102_0214b53c into +0x31c and through data_020a0e68 -- and only the flat
 * spelling reproduces the cartridge's block move.
 *
 * decl_common.h is deliberately NOT included.  It declares eight of this TU's
 * own members, and its spelling of func_ov102_0214baa0 is `void(void *)` where
 * the definition below takes `char *` -- and that parameter type is measured,
 * not cosmetic: a `void *` parameter plus a `char *` local at the top of the
 * body reorders the frame and the function stops matching.  Under `extern "C"`
 * a differing parameter type is not an overload but an `illegal function
 * overloading` error pointed at the definition.  The same is true of
 * func_ov102_0214b53c, whose pinned stack layout only survives with the `char *`
 * parameter the shard used.  Everything the header would have supplied is
 * declared here instead -- which is also the large-TU precedent (ov002/Player,
 * ov006/dScMgPanel_c, ov006/dScMgLuigi_c).
 *
 * DECLARATION POLICY, because it is what makes a merge this size compile.
 * There is exactly ONE file-scope `extern "C"` region, and it carries only what
 * the C++-NAMED members need: a class member function may not sit inside a
 * linkage specification, so a declaration written in daBmb_c::Behavior's own
 * body would get C++ linkage and the reference would mangle a second time.
 * Every OTHER member keeps its own independently recovered spellings at BLOCK
 * scope inside its own body, where two members may disagree about one symbol
 * without either giving way.  Where a member's spelling collided with the
 * file-scope region it was adapted at the call site with a cast, never by
 * rewriting the region.
 *
 * THAT POLICY IS ALSO THE CEILING ON HOW MANY MEMBERS CAN BE C++ METHODS.
 * Thirteen of the thirty-five are: the nine the cartridge itself names through
 * the vtable and the RTTI group (the two destructors among them, emitted from
 * the header's inline body), plus four of the six mState arms under coined
 * names.  One more, daBmb_c_classInit, is a member of the class in every sense
 * but has to keep C linkage because the ROM symbol is the bare name.  The other
 * twenty-one stay free functions, and the wall is one of exactly two things,
 * never codegen:
 *
 *   SCOPE -- func_ov102_0214bf64 and func_ov102_0214bd90 (mState arms 0 and 2)
 *   both compile to the ROM's exact size as members, and the TU then fails to
 *   LINK: as members they lose their own extern "C" declarations, and the
 *   file-scope region cannot take them because other members already own
 *   contradicting spellings of the same symbols -- decisively func_0200fc44,
 *   which arm 2 recovered with four parameters and func_ov102_0214aa18 with
 *   three.  Note that `tubuild.py verify` calls the broken form 35/35 MATCH with
 *   reloc destinations clean; only mwldarm sees it.
 *
 *   NAMING -- the remaining nineteen.  The cartridge carries their addresses and
 *   no identifier, and nothing in it constrains what they should be called the
 *   way the mState switch constrains the six arms.  Three of them are also
 *   called from outside ov102 (0x0214ad14 from ov002/ov078, 0x0214ae1c from
 *   ov014/ov098, 0x0214b384 from ov078), so naming those is a cross-module
 *   rename, not a local one.
 */

#include "common.h"
#include "daBmb_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

/* func_ov102_0214b248 reaches Sound::PlayLong, which include/Sound.h does not
   declare yet.  Re-opening the namespace here is additive: it does not restate
   anything the header already has. */
namespace Sound {
unsigned int PlayLong(unsigned int, unsigned int, unsigned int, Vector3 const &, short);
}

/* func_ov102_0214bf64's own view of the object.  It reaches fifteen fields by
   name through a shadow struct rather than through daBmb_c, and it is kept that
   way: the member is byte-matched in that spelling.  The tags are uniquified
   because the file scope it now shares with the real headers already has a
   Vector3 and an Animation.  0x350 is the Animation subobject, advanced through
   its mangled entry the way func_ov102_0214be1c and func_ov102_0214bcc8 do. */
struct Bmb_Vec3 { int x, y, z; };
struct Bmb_Bf64Obj {
    char p0[0x5c];
    Bmb_Vec3 f5c;             /* 0x5c */
    char g68[0x8e - 0x68];
    short f8e;                /* 0x8e */
    char g90[0x94 - 0x90];
    short f94;                /* 0x94 */
    char g96[0x98 - 0x96];
    int f98;                  /* 0x98 */
    char ga0[0xa8 - 0x9c];
    int fa8;                  /* 0xa8 */
    char gac[0x350 - 0xac];
    char f350[0x35c - 0x350]; /* 0x350 -- Animation */
    int f35c;                 /* 0x35c */
    char g360[0x38c - 0x360];
    void* f38c;               /* 0x38c */
    char g390[0x3c4 - 0x390];
    Bmb_Vec3 f3c4;            /* 0x3c4 */
    char g3d0[0x3dc - 0x3d0];
    int f3dc;                 /* 0x3dc */
    char g3e0[0x3e8 - 0x3e0];
    unsigned short f3e8;      /* 0x3e8 */
    char g3ea[0x3f5 - 0x3ea];
    unsigned char f3f5;       /* 0x3f5 */
};

/* daBmb_c::Render reaches the model object at +0x300 through slot 5 of its own
   table.  Virtual dispatch, so nothing here names a symbol. */
struct Bmb_VBase {
    virtual void method0() = 0;
    virtual void method1() = 0;
    virtual void method2() = 0;
    virtual void method3() = 0;
    virtual void method4() = 0;
    virtual void method5(char *arg);
};

/* The three shared files this actor claims.  InitResources loads them and
   CleanupResources releases them, so both C++-named members need one agreed
   spelling; the members below that read the loaded handle out of the same bytes
   take their own view with a cast rather than a second declaration. */
extern SharedFilePtr data_ov102_0214e9c0;
extern SharedFilePtr data_ov102_0214e9c8;
extern SharedFilePtr data_ov002_0210d9e0;

extern Matrix4x3 IDENTITY_MATRIX4X3;

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region.  Everything here is reached from a
 * C++-named member, which cannot declare it in its own body.
 * ------------------------------------------------------------------------ */
extern "C" {

/* -- this TU's own members, forward-declared because mwcc lays .text down in
      reverse source order and every one of these calls is a forward reference. */
int   func_ov102_0214aa18(void *self);
int   func_ov102_0214ab1c(void *self);
void  func_ov102_0214ad40(void *self);
void  func_ov102_0214ae1c(void *self);
void  func_ov102_0214b03c(void *self);
void  func_ov102_0214b128(void *self);
int   func_ov102_0214b248(void *self);
void  func_ov102_0214b384(void *self, unsigned int level);
void  func_ov102_0214b53c(char *self);
void  func_ov102_0214baa0(char *self);
void  func_ov102_0214beb4(void *self);
void  func_ov102_0214c0b8(void *self);

/* -- other modules -- */
void  GiveCoins(int who, int count);
int   SurfaceInfo_TestFlag0x20(int *si);

int   _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(void *, void *, void *, unsigned int);
void  _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(void *, void *);
void  _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *, void *, unsigned int);
void  _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void *, void *, void *, unsigned int);

void  _ZN5dCc_c5ClearEv(void *);
void  _ZN5dCc_c6UpdateEv(void *);

void *_ZN8dActor_c10FindWithIDEj(unsigned int);
void  _ZN8dActor_c9UpdatePosEP5dCc_c(void *, void *);

int   _ZNK10dBgCh_Actr10IsOnGroundEv(void *);
int   _ZNK10dBgCh_Actr8IsOnWallEv(void *);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *);

/* ModelBase::SetFile is declared void in include/ModelBase.h -- which is what
   its own matched definition compiles as -- but the ROM leaves DoSetFile's int
   in r0 and InitResources reads it, so the value-returning entry keeps the
   mangled spelling until that signature is settled. */
int   _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, BMD_File *f, int a, int b);

/* Both Init calls keep scalar slots: they carry Fix12<int> BY VALUE, which
   mwccarm passes differently at the call site, so spelling the true types
   breaks the byte match. */
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, dActor_c *a, Fix12i r, Fix12i h, unsigned int d, unsigned int e);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *, dActor_c *a, Fix12i b, Fix12i c, Vector3_16 *d, Fix12i e);

/* Hoisted out of four member bodies.  A class member function may not sit in a
   block-scope linkage specification, so daBmb_c::State1/3/4/5 cannot carry their
   own extern "C" declarations the way the free members below do -- these have to
   be file-scope, and they are spelled to agree with every other user in the TU.
   _Z14ApproachLinearRsss takes the reference its own mangled name encodes; the
   free func_ov102_0214be1c shard had recovered it as a pointer, and State1's two
   call sites bind the same address through the reference instead. */
void  _Z14ApproachLinearRsss(short &v, short target, short step);
void  _ZN9Animation7AdvanceEv(void *a);
int   _ZN8dActor_c13DistToCPlayerEv(void *p);
void  func_ov102_0214b3b8(void *c);

extern signed char   data_0209f2f8;
extern unsigned char data_0209f220;
extern unsigned char data_0209f2d8;

}

/* ==========================================================================
 * ROM ordinal 34 -- daBmb_c_classInit, 0x0214c6f8, size 0x50.
 *
 * The registry factory, folded in from its own one-function shard.  It is the TOP of the
 * cartridge's contiguous run (0x0214c748 is where daShl_c starts), so it is
 * written FIRST here: mwccarm lays .text down in reverse source order.
 *
 * C LINKAGE IS LOAD-BEARING -- the ROM symbol is the bare name, which the
 * legacy C file got for free and a C++ TU has to ask for.
 *
 * THE `+ 2` IS ALSO LOAD-BEARING, and it is what consolidation changed.  The
 * shard wrote the bare `(int)_ZTV7daBmb_c`, and that was right there: the
 * symbol was UNDEF and config/arm9/overlays/ov102/symbols.txt binds it to the
 * SLOT ARRAY at 0x0214e558.  Here the same spelling would bind to this TU's own
 * definition -- OnYoshiTryEat is the key function, so mwcc emits the vtable and
 * addresses the vtable OBJECT, two words of Itanium preamble (0x0214e550)
 * lower.  `+ 2` is int-indexed, so eight bytes, and it makes this store agree
 * with the addend-8 vptr stores the compiler itself emits in the destructor.
 * A relocated word is a wildcard to every byte gate, so `verify` prints the
 * same 35/35 either way; tools/objisolate.py's addend check is what refuses the
 * bare form.
 *
 * The declaration itself lives in include/daBmb_c.h, next to the class and the
 * allocation size this factory proves -- the vtable is a property of daBmb_c,
 * not of this file, and the legacy shard read it from a shared header too.
 *
 * Reconstructed source-style name: SM64DS proves daBmb_c through RTTI,
 * allocation size, vtable identity, and the BOMBHEI registry profile; later EAD
 * lineage supplies classInit.  Exact original spelling is not preserved.
 * Historical alias: BobOmb_Spawn.
 * ======================================================================== */

extern "C" {

// @symbol daBmb_c_classInit
int *daBmb_c_classInit(void)
{
    extern void *_ZN7fBase_cnwEj(unsigned int size);
    extern void  _ZN12dEnemyBase_cC2Ev(void *thiz);
    extern void  _ZN7dCcAc_cC1Ev(char *thiz);
    extern void  _ZN10dBgCh_ActrC1Ev(char *thiz);
    extern void  _ZN9ModelAnimC1Ev(char *thiz);
    extern void  _ZN11ShadowModelC1Ev(char *thiz);

    int *p = (int *)_ZN7fBase_cnwEj(1024);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)(_ZTV7daBmb_c + 2);
        _ZN7dCcAc_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x144);
        _ZN9ModelAnimC1Ev((char *)p + 0x300);
        _ZN11ShadowModelC1Ev((char *)p + 0x364);
    }
    return p;
}

}

/* ==========================================================================
 * ROM ordinal 33 -- daBmb_c::OnYoshiTryEat, 0x0214c6e4, size 0x14.
 * Vtable slot 18.  THE KEY FUNCTION: the first out-of-line virtual this class
 * declares, so this TU owns _ZTV7daBmb_c, _ZTI7daBmb_c and _ZTS7daBmb_c.
 * ======================================================================== */

// @symbol _ZN7daBmb_c13OnYoshiTryEatEv
s32 daBmb_c::OnYoshiTryEat() {
    /* 263 == 0x107, dEnemyBase_c's own byte at that offset. */
    return mEatenByYoshi == 0;
}

/* ==========================================================================
 * ROM ordinal 32 -- daBmb_c::InitResources, 0x0214c510, size 0x1d4.
 * Vtable slot 0.
 *
 * param1's low three bits pick the variant: 2 is the one that starts inert --
 * it sets the collision volume's `hit` bit, clears a behaviour flag and leaves
 * unk_108 clear -- 4 also starts clear, and everything else starts live.
 * ======================================================================== */

// @symbol _ZN7daBmb_c13InitResourcesEv
int daBmb_c::InitResources()
{
    BMD_File* bmd;

    Animation::LoadFile(data_ov102_0214e9c0);
    Animation::LoadFile(data_ov102_0214e9c8);
    bmd = (BMD_File*)Model::LoadFile(data_ov002_0210d9e0);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, bmd, 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;

    mVariant = (unsigned char)(param1 & 7);
    unk_3ec = 0x2000;
    func_ov102_0214c0b8(this);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, this, 0x3c000, 0x50000, 0x200004, 0xa6d380);
    mTerminalVelocity = -0x37000;

    if (mVariant == 2) {
        /* The add sits INSIDE the integer cast, which is load-bearing: not
           interchangeable with `mdCc_c.flags |= 2` or `mFlags &= ~1u`,
           though 0x128 is that field (+0x18) and 0xb0 is mFlags. */
        *(unsigned int*)(((int)this + 0x128)) |= 2;
        *(unsigned int*)(((int)this + 0xb0)) &= ~1u;
        unk_108 = 0;
    } else if (mVariant == 4) {
        unk_108 = 0;
    } else {
        unk_108 = 1;
    }

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    *(Matrix4x3 *)mMatrix = IDENTITY_MATRIX4X3;
    unk_3e8 = 0;
    unk_3ea = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mCarrier = 0;
    unk_3f2 = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    mShouldRender = 1;
    /* dActor_c does not name 0xc8 yet -- it is padding between mClipResult and
       mAreaId -- so this one stays an offset rather than growing a field on a
       header 200+ files include. */
    *(int *)((char *)this + 0xc8) = 0;
    unk_3e0 = 2;
    unk_3f6 = 0;
    mHomeAngleY = mAngleY;
    return 1;
}

/* ==========================================================================
 * ROM ordinal 31 -- daBmb_c::Behavior, 0x0214c1bc, size 0x354.
 * Vtable slot 6.
 * ======================================================================== */

// @symbol _ZN7daBmb_c8BehaviorEv
int daBmb_c::Behavior()
{
    int flag;
    int r0;
    void *other;

    if (unk_3f6 != 0) {
        func_ov102_0214ae1c(this);
        return 1;
    }
    if (func_ov102_0214ab1c(this)) {
        return 1;
    }
    if (func_ov102_0214aa18(this)) {
        return 1;
    }

    r0 = _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(this, (char *)&mWithMeshClsn, (char *)&mModelAnim, 0);
    if (r0 != 0) {
        if (r0 == 2) {
            func_ov102_0214ae1c(this);
        }
        return 1;
    }

    if (mDeathState != 0) {
        _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(this, (char *)&mWithMeshClsn);
        func_ov102_0214b128(this);
        flag = mFlags & 0x100;
        flag = flag != 0;
        if (flag != false) {
            mDeathState = 0;
        } else if (mDeathState != 0) {
            func_ov102_0214b53c((char *)this);
            _ZN5dCc_c5ClearEv((char *)&mdCc_c);
            _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
            return 1;
        }
    }

    func_ov102_0214b03c(this);
    if (mState != 5) {
        if (mVertAccel != 0) {
            if ((*(int*)(&mdCc_c.hitFlags) & 0x10) != 0) {
                short v[3];
                other = _ZN8dActor_c10FindWithIDEj(*(unsigned int*)(&mdCc_c.otherOwner));
                v[0] = -0x2000;
                v[1] = 0;
                v[2] = 0;
                _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(this, v, other, 0x32000);
                _ZN8dActor_c9UpdatePosEP5dCc_c(this, (char *)&mdCc_c);
                _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(this, (char *)&mWithMeshClsn, 0);
                _ZN5dCc_c5ClearEv((char *)&mdCc_c);
                return 1;
            }

            _ZN8dActor_c9UpdatePosEP5dCc_c(this, (char *)&mdCc_c);
            if (data_0209f2f8 == 6 && data_0209f220 == 3) {
                if (mHorzSpeed == 0x5000) {
                    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(this, (char *)&mWithMeshClsn, 3);
                } else {
                    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(this, (char *)&mWithMeshClsn, 2);
                }
            } else {
                _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(this, (char *)&mWithMeshClsn, 2);
            }

            if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn)) {
                if (SurfaceInfo_TestFlag0x20((int*)((char*)_ZNK10dBgCh_Actr14GetFloorResultEv((char *)&mWithMeshClsn)+4))) {
                    func_ov102_0214ae1c(this);
                    return 1;
                }
                if (_ZNK10dBgCh_Actr8IsOnWallEv((char *)&mWithMeshClsn) && mState == 0) {
                    func_ov102_0214beb4(this);
                }
            }
        }

        r0 = func_ov102_0214b248(this);
        if (r0 == 0) {
            return 0;
        }

        if (*(unsigned int*)(&mdCc_c.otherOwner) != 0) {
            if ((*(int*)(&mdCc_c.hitFlags) & 0x4000) != 0) {
                func_ov102_0214b384(this, 4);
            }
            if (mState == 4) {
                unsigned char b = mVariant;
                if (b == 2 || b == 3) {
                    other = _ZN8dActor_c10FindWithIDEj(*(unsigned int*)(&mdCc_c.otherOwner));
                    if (other != 0) {
                        int flag2 = *(unsigned short*)((char*)other+0xc);
                        flag2 = flag2 == 0xbd;
                        if (flag2 != false) {
                            func_ov102_0214b384(this, 2);
                            {
                                int *p128 = (int *)(((unsigned long long)(unsigned)(&mdCc_c.flags)) & 0xFFFFFFFFFFFFFFFFULL);
                                *p128 |= 0x4000;
                            }
                        }
                    }
                }
            }
        }

        func_ov102_0214b53c((char *)this);
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
        func_ov102_0214ad40(this);
    }
    return 1;
}

/* ==========================================================================
 * ROM ordinal 30 -- daBmb_c::Render, 0x0214c168, size 0x54.
 * Vtable slot 9.
 * ======================================================================== */

// @symbol _ZN7daBmb_c6RenderEv
int daBmb_c::Render()
{
    int result = 1;
    if (mShouldRender != 0) {
        int flags = mFlags;
        int b = (flags & 0x40000) != 0;
        if (!b) {
            Bmb_VBase *obj = (Bmb_VBase*)((char *)&mModelAnim);
            obj->method5((char *)&mScaleX);
        }
    }
    return result;
}

/* ==========================================================================
 * ROM ordinal 29 -- daBmb_c::CleanupResources, 0x0214c12c, size 0x3c.
 * Vtable slot 3.  Releases the three shared files InitResources claimed and
 * touches no field of its own.
 * ======================================================================== */

// @symbol _ZN7daBmb_c16CleanupResourcesEv
int daBmb_c::CleanupResources()
{
    data_ov002_0210d9e0.Release();
    data_ov102_0214e9c0.Release();
    data_ov102_0214e9c8.Release();
    return 1;
}

/* ==========================================================================
 * ROM ordinal 28 -- func_ov102_0214c0b8, 0x0214c0b8, size 0x74.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214c0b8
void func_ov102_0214c0b8(void *cv)
{
    extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
    char *c = (char *)cv;

    *(int*)(c + 0x3dc) = 0;
    if (*(unsigned char*)(c + 0x3f5) == 1)
        *(int*)(c + 0x98) = 0;
    else
        *(int*)(c + 0x98) = 0x5000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, *(void **)((char *)&data_ov102_0214e9c0 + 4), 0, 0x1000, 0);
    *(short*)(c + 0x3e8) = 0x200;
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0x390) = 0;
}

}

/* ==========================================================================
 * ROM ordinal 27 -- func_ov102_0214bf64, 0x0214bf64, size 0x154.
 *
 * mState arm 0, and it STAYS A FREE FUNCTION.  The conversion itself is byte-clean
 * -- compiled as daBmb_c::State0() it emits exactly 0x154 -- but the TU then will
 * not LINK, and the wall is SCOPE, not codegen and not naming.  A member function
 * may not sit inside a block-scope linkage specification, so this body would lose
 * the extern "C" declarations it carries, and the file-scope region cannot take
 * them: it already owes func_ov102_0214b988 a Vec3_HorzAngle and a Vec3_Dist that
 * take const Vector3 *, where this body recovered them over its own Bmb_Vec3
 * shadow.  Measured, not assumed: mwldarm reported them Undefined and named
 * daBmb_c::State0() as the referrer, on a build that tubuild verify had just
 * called 35/35 MATCH with reloc destinations clean.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214bf64
void func_ov102_0214bf64(void *ov)
{
    extern unsigned short DecIfAbove0_Short(unsigned short* p);
    extern void _Z14ApproachLinearRiii(int& v, int target, int step);
    extern void _Z14ApproachLinearRsss(short& v, short target, short step);
    extern short Vec3_HorzAngle(const Bmb_Vec3* v0, const Bmb_Vec3* v1);
    extern int Vec3_Dist(const Bmb_Vec3* a, const Bmb_Vec3* b);
    extern void func_ov102_0214b988(void* c);
    extern void _ZN9Animation7AdvanceEv(void *a);

    Bmb_Bf64Obj *o = (Bmb_Bf64Obj *)ov;

    DecIfAbove0_Short(&o->f3e8);
    if (o->f3f5 != 1) {
        if (o->f38c == 0) {
            _Z14ApproachLinearRiii(o->f98, 0x5000, 0x2aa);
            func_ov102_0214b988(o);
            if (o->f38c != 0) {
                o->f98 = 0;
                o->fa8 = 0x14000;
                o->f3dc = 2;
            } else {
                if (o->f3f5 == 5) {
                    s16 ang = Vec3_HorzAngle(&o->f5c, &o->f3c4);
                    _Z14ApproachLinearRsss(o->f94, ang, 0x200);
                    o->f8e = o->f94;
                } else {
                    if (o->f3e8 == 0 || Vec3_Dist(&o->f5c, &o->f3c4) >= 0x500000) {
                        o->f98 = 0x5000;
                        func_ov102_0214beb4(o);
                    }
                }
            }
        } else {
            _Z14ApproachLinearRiii(o->f98, 0x10000, 0x1000);
            s16 target = Vec3_HorzAngle(&o->f5c, (Bmb_Vec3*)((char*)o->f38c + 0x5c));
            if (o->f3f5 == 3) {
                int b = (*(int*)((char*)o->f38c + 0x35c) != 0);
                if (b)
                    target = o->f94;
            }
            _Z14ApproachLinearRsss(o->f94, target, 0x800);
            o->f8e = o->f94;
        }
        o->f35c = o->f98 >> 3;
    }
    _ZN9Animation7AdvanceEv(o->f350);
}

}

/* ==========================================================================
 * ROM ordinal 26 -- func_ov102_0214beb4, 0x0214beb4, size 0xb0.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214beb4
void func_ov102_0214beb4(void *cv)
{
    extern short Vec3_HorzAngle(const void* a, const void* b);
    extern int Vec3_Dist(const void* a, const void* b);
    extern int RandomIntInternal(int* seed);
    extern int data_0209e650;

    char *c = (char *)cv;

    *(int*)(c + 0x3dc) = 1;
    *(unsigned char*)(((int)c + 0x3f2)) += 1;
    char* o = *(char**)(c + 0x38c);
    if (o == 0) {
        *(short*)(c + 0x300 + 0xee) = Vec3_HorzAngle(c + 0x5c, c + 0x3c4);
        if (Vec3_Dist(c + 0x5c, c + 0x3c4) >= 0x500000) return;
        unsigned int r = RandomIntInternal(&data_0209e650);
        *(short*)(((int)c + 0x3ee)) += (short)(r >> 0x10);
        return;
    }
    *(short*)(c + 0x300 + 0xee) = Vec3_HorzAngle(c + 0x5c, o + 0x5c);
}

}

/* ==========================================================================
 * ROM ordinal 25 -- daBmb_c::State1, 0x0214be1c, size 0x98.
 *
 * mState arm 1 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): turns the facing angle (+0x94) toward the stored target angle (+0x3ee) at 0x400 a
 * frame with no target actor and 0x800 with one, mirrors it into +0x8e, advances
 * the animation, and once the two angles meet -- and only while mState is not 2 --
 * hands off to func_ov102_0214c0b8.
 * ======================================================================== */

// @symbol _ZN7daBmb_c6State1Ev
void daBmb_c::State1() {
    char *c = (char *)this;
    if (*(int*)(c + 0x38c) == 0) {
        *(int*)(c + 0x35c) = 0x800;
        _Z14ApproachLinearRsss(*(short*)(c + 0x94), *(short*)(c + 0x3ee), 0x400);
    } else {
        *(int*)(c + 0x35c) = 0x1000;
        _Z14ApproachLinearRsss(*(short*)(c + 0x94), *(short*)(c + 0x3ee), 0x800);
    }
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    _ZN9Animation7AdvanceEv(c + 0x350);
    if (*(int*)(c + 0x3dc) == 2) return;
    if (*(short*)(c + 0x94) != *(short*)(c + 0x3ee)) return;
    func_ov102_0214c0b8(c);
}

/* ==========================================================================
 * ROM ordinal 24 -- func_ov102_0214bd90, 0x0214bd90, size 0x8c.
 *
 * mState arm 2, and it STAYS A FREE FUNCTION for the same reason arm 0 does, with
 * a harder instance of it: this body recovered func_0200fc44 with FOUR parameters
 * and func_ov102_0214aa18 recovered the same symbol with THREE.  Both spellings
 * byte-match their own body; one file-scope declaration cannot serve both without
 * changing an argument count at a call site, which is not byte-neutral.  It also
 * spells dBgCh_Actr::JustHitGround over char * where aa18 spells it over void *.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214bd90
void func_ov102_0214bd90(char* r4){
  extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char* c);
  extern void func_0200fc44(char* c, struct Vector3* v, int a, int z);

  if (_ZNK10dBgCh_Actr13JustHitGroundEv(r4 + 0x144)) {
    struct Vector3 v;
    v.x = *(int*)(r4 + 0x5c);
    v.y = *(int*)(r4 + 0x60);
    v.z = *(int*)(r4 + 0x64);
    func_0200fc44(r4, &v, 1, v.z);
  }
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(r4 + 0x144)) {
    if (*(int*)(r4 + 0x38c) == 0) {
      *(int*)(r4 + 0x98) = 0x5000;
    }
    func_ov102_0214beb4(r4);
  } else {
    ((daBmb_c *)r4)->State1();
  }
}

}

/* ==========================================================================
 * ROM ordinal 23 -- func_ov102_0214bd20, 0x0214bd20, size 0x70.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214bd20
void func_ov102_0214bd20(char* c)
{
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int a, int b, unsigned int u);

    *(int*)(c + 0x3dc) = 3;
    *(int*)(c + 0x9c) = 0;
    *(int*)(c + 0x98) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, *(void **)((char *)&data_ov102_0214e9c8 + 4), 0, 0x1000, 0);
    func_ov102_0214b384(c, 0x96);
    {
        int* p = (int*)(((int)c + 0x128));
        *p = *p | 2;
        *p = *p & ~4;
    }
}

}

/* ==========================================================================
 * ROM ordinal 22 -- daBmb_c::State3, 0x0214bcc8, size 0x58.
 *
 * mState arm 3 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): the held arm: on mFlags bit 0x400 it runs func_ov102_0214b3b8 (which checks
 * mState == 3, runs func_ov102_0214bc20, copies the carrier's facing angle and
 * clears mCarrier), on bit 0x100 it does nothing at all, and otherwise it hands off
 * to func_ov102_0214c0b8.  Always advances the animation.
 * ======================================================================== */

// @symbol _ZN7daBmb_c6State3Ev
void daBmb_c::State3()
{
  char *c = (char *)this;
  int r1 = *((int *) (c + 0xb0));
  int b = (int) ((r1 & 0x400) != 0);
  if (b)
  {
    func_ov102_0214b3b8(c);
  }
  else
  {
    int b2 = (int) ((r1 & 0x100) != 0);
    if (b2)
    {
    }
    else
    {
      func_ov102_0214c0b8(c);
    }
  }
  _ZN9Animation7AdvanceEv(c + 0x350);
}

/* ==========================================================================
 * ROM ordinal 21 -- func_ov102_0214bc20, 0x0214bc20, size 0xa8.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214bc20
void func_ov102_0214bc20(char* c)
{
    extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
    typedef struct { int v[4]; } Quad;
    extern Quad data_ov102_0214e514;
    extern Quad data_ov102_0214e524;

    Quad a1 = data_ov102_0214e514;
    Quad a2 = data_ov102_0214e524;
    void *p = *(void**)(c + 0x390);
    int idx = 0;
    if (p != 0) {
        idx = *(int*)((char*)p + 8);
        if ((unsigned)idx > 4) idx = 4;
    }
    *(int*)(c + 0x98) = a1.v[idx];
    *(int*)(c + 0xa8) = a2.v[idx];
    *(int*)(c + 0x3dc) = 4;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, *(void **)((char *)&data_ov102_0214e9c0 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x9c) = -0x2000;
}

}

/* ==========================================================================
 * ROM ordinal 20 -- daBmb_c::State4, 0x0214bbd8, size 0x48.
 *
 * mState arm 4 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): the thrown arm: unless the collision actor reports neither ground nor wall contact
 * it calls func_ov102_0214b384(4) and zeroes the forward speed.
 * ======================================================================== */

// @symbol _ZN7daBmb_c6State4Ev
int daBmb_c::State4() {
    void *c = this;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char*)c + 0x144) == 0) {
        int r = _ZNK10dBgCh_Actr8IsOnWallEv((char*)c + 0x144);
        if (r == 0) return r;
    }
    func_ov102_0214b384(c, 4);
    *(int*)((char*)c + 0x98) = 0;
    return 0;
}

/* ==========================================================================
 * ROM ordinal 19 -- func_ov102_0214baa0, 0x0214baa0, size 0x138.
 *
 * VOID, and that is measured, not stylistic.  The separate shard this came from
 * was `void`; daBmb_c::OnTurnIntoEgg's shard independently declared it `int` and
 * tail-forwarded its r0.  One TU cannot hold both spellings, and the `int` form
 * costs bytes: with a non-void return type mwccarm keeps r0 reserved, so the
 * final `*(int *)(self + 8) = 0` materialises its zero in r1 (0x0214bbbc/0xbbc0
 * differ) and every explicit `return 0;` adds an instruction the cartridge does
 * not have.  Void restores r0 and the function matches exactly.  The call site
 * pays nothing: the cartridge's OnTurnIntoEgg is `bl 0x214baa0` followed by its
 * own epilogue, which is what `func_ov102_0214baa0(this);` with no return
 * statement emits -- byte-identical to the `return f(this);` the shard used.
 * ======================================================================== */

#define LAUNDER_PTR(ptr) \
    ((int *)(int)(ptr))
#define LAUNDER_32(base, offset) \
    ((int *)(int)((long long)((int)(base) + (offset))))

extern "C" {

// @symbol func_ov102_0214baa0
void func_ov102_0214baa0(char *self)
{
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
    extern void func_0203568c(int *value, int target);
    extern void func_02035684(int *value, int target);

    u8 variant;

    *LAUNDER_32(self, 0xb0) &= ~0x10000000;
    variant = *(u8 *)(self + 0x3f5);
    if (!(variant < 2 || variant == 5)) {
        _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
        return;
    }

    *(int *)(self + 0x3dc) = 5;
    *(int *)(self + 0x98) = 0;
    _ZN5dCc_c5ClearEv(self + 0x110);

    *LAUNDER_PTR(self + 0xb0) &= 0xfff1fffe;
    *(int *)(self + 0x5c) = *(int *)(self + 0x3c4);
    *(int *)(self + 0x60) = *(int *)(self + 0x3c8);
    *(int *)(self + 0x64) = *(int *)(self + 0x3cc);
    *(u8 *)(self + 0x3f3) = 0;
    *(int *)(self + 0x38c) = 0;
    *(int *)(self + 0x390) = 0;
    *(s16 *)(self + 0x3ea) = 0;
    *(int *)(self + 0x80) = 0x1000;
    *(int *)(self + 0x84) = 0x1000;
    *(int *)(self + 0x88) = 0x1000;
    *(s16 *)(self + 0x8c) = 0;
    *(s16 *)(self + 0x8e) = 0;
    *(s16 *)(self + 0x90) = 0;
    *(s16 *)(self + 0x92) = 0;
    *(s16 *)(self + 0x94) = 0;
    *(s16 *)(self + 0x96) = 0;

    {
        s16 angle = *(s16 *)(self + 0x3f0);
        *(s16 *)(self + 0x8e) = angle;
        s16 copy = *(s16 *)(self + 0x8e);
        *(s16 *)(self + 0x94) = copy;
    }

    *LAUNDER_PTR(self + 0x128) &= ~0x4002;
    *LAUNDER_PTR(self + 0x128) |= 4;
    *(int *)(self + 0x114) = 0x3c000;
    *(int *)(self + 0x118) = 0x50000;

    func_0203568c((int *)(self + 0x144), 0x32000);
    func_02035684((int *)(self + 0x144), 0x32000);
    *(int *)(self + 8) = 0;
}

}

/* ==========================================================================
 * ROM ordinal 18 -- daBmb_c::State5, 0x0214ba30, size 0x70.
 *
 * mState arm 5 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): the dormant arm, which func_ov102_0214baa0 puts the actor into after clearing
 * mShouldRender: once the camera player is at least 0x7d0000 away -- or the actor is
 * off-camera and data_0209f2d8 is 1 -- it runs func_ov102_0214c0b8, sets mFlags
 * 0x10000001 and turns rendering back on.
 * ======================================================================== */

// @symbol _ZN7daBmb_c6State5Ev
void daBmb_c::State5()
{
    char *c = (char *)this;
    if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x7d0000)
        return;
    if ((*(int*)(c + 0xb0) & 8) == 0) {
        int b = (data_0209f2d8 == 1);
        if (b == 0)
            return;
    }
    func_ov102_0214c0b8(c);
    {
        int* f = (int*)(((int)c + 0xb0));
        *f = *f | 0x10000001;
    }
    *(unsigned char*)(c + 0x3f3) = 1;
}

/* ==========================================================================
 * ROM ordinal 17 -- func_ov102_0214b988, 0x0214b988, size 0xa8.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b988
void func_ov102_0214b988(void *thiz)
{
    extern void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
    extern int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
    extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
    extern int AngleDiff(int, int);

    unsigned char *c = (unsigned char *)thiz;
    unsigned char *pl;
    struct Vector3 v;

    *(int *)(c + 0x38c) = 0;
    pl = (unsigned char *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (!pl) return;
    if (Vec3_Dist((struct Vector3 *)(c + 0x5c), (struct Vector3 *)(pl + 0x5c)) > 0x190000) return;
    {
        int *src = (int *)(pl + 0x5c);
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
    }
    if (AngleDiff(Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), &v), *(short *)(c + 0x8e)) >= *(unsigned short *)(c + 0x3ec)) return;
    *(int *)(c + 0x38c) = (int)pl;
    func_ov102_0214b384(c, 0x96);
}

}

/* ==========================================================================
 * ROM ordinal 16 -- func_ov102_0214b53c, 0x0214b53c, size 0x44c.
 *
 * The volatile pins and the chained u32 masks are load-bearing: they are what
 * hold the cartridge's stack layout and its compare destinations.  Do not
 * simplify them.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b53c
void func_ov102_0214b53c(char *c)
{
  extern void *func_ov002_020e496c(char *c);
  extern int _ZN6Player14IsFrontSlidingEv(void *self);
  extern int _ZN6Player17LostGrabbedObjectEv(void *self);
  extern void Math_Function_0203b14c(void *out, int a1, int a2, int a3, int a4);
  extern void *_ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void *self, void *player, const void *v);
  extern void Matrix4x3_FromRotationY(void *m, int angle);
  extern void MulMat4x3Mat4x3(void *a, void *b, void *out);
  extern void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh);
  extern void Vec3_LslInPlace(Vector3 *v, int sh);
  extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(int sys, unsigned int kind, int fx, int a1, int a2, const void *v, void *cb);
  extern void func_ov102_0214b444(void *c);
  extern int data_ov102_0214ea18[];
  extern int data_ov102_0214ea1c[];
  extern int data_ov102_0214ea20[];
  extern Matrix4x3 data_020a0e68;
  extern s16 data_02082214[];

  volatile u16 tmp[6]; /* angle-home pins */
  Vector3 p;
  volatile Vector3 dead; /* stack-layout placeholder; never a live value */
  int x0;
  Vector3 d;
  int xSaved;
  char *pPos;
  char *src;
  /* Plain short-circuit.  The shard carried a chain of `& 0xFFFFFFFFu` masks here;
     that lever is C-only -- under -lang c++ the `&&` is a bool and the mask forces
     mwccarm to materialise it in a register, costing three instructions. */
  if ((*((u8 *) (c + 0x3f5))) == 2 && (src = *((char **) (c + 0xc8))) != 0)
  {
    *((Matrix4x3 *) (c + 0x31c)) = *((Matrix4x3 *) src);
  }
  else
  {
    int haveMtx = (int) (((*((u32 *) (c + 0xb0))) & 0x4000) != 0);
    char *player0;
    if (((haveMtx != 0) && ((player0 = *((char **) (c + 0x390))) != 0)) && ((*((int *) (player0 + 0xc8))) != 0))
    {
      char *ret = (char *) func_ov002_020e496c(player0);
      u8 idx = 0;
      if (_ZN6Player14IsFrontSlidingEv(*((char **) (c + 0x390))) != 0)
      {
        idx = 1;
      }
      if ((_ZN6Player17LostGrabbedObjectEv(*((char **) (c + 0x390))) != 0) && (((u32) (((u32) ((*((int *) (ret + 0x58))) << 4)) >> 0x10)) < 0xe))
      {
        idx = 1;
      }
      if ((*((int *) ((*((char **) (c + 0x390))) + 8))) == 2)
      {
        idx = (u8) (idx + 2);
      }
      int off = idx * 0xc;
      Math_Function_0203b14c(c + 0x3d0, *((int *) (((char *) data_ov102_0214ea18) + off)), 0x800, 0x3e8000, 4);
      Math_Function_0203b14c(c + 0x3d4, *((int *) (((char *) data_ov102_0214ea1c) + off)), 0x800, 0x3e8000, 4);
      Math_Function_0203b14c(c + 0x3d8, *((int *) (((char *) data_ov102_0214ea20) + off)), 0x800, 0x3e8000, 4);
      Matrix4x3 *rmtx = (Matrix4x3 *) _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(c, *((char **) (c + 0x390)), c + 0x3d0);
      *((Matrix4x3 *) (c + 0x31c)) = *rmtx;
    }
    else
    {
      *((int *) (c + 0x3d0)) = 0;
      *((int *) (c + 0x3d4)) = 0;
      *((int *) (c + 0x3d8)) = 0;
      Matrix4x3_FromRotationY(c + 0x31c, *((s16 *) (c + 0x8e)));
      *((int *) (c + 0x340)) = (*((int *) (c + 0x5c))) >> 3;
      *((int *) (c + 0x344)) = (*((int *) (c + 0x60))) >> 3;
      *((int *) (c + 0x348)) = (*((int *) (c + 0x64))) >> 3;
    }
  }
  int haveMtx2 = (int) (((*((u32 *) (c + 0xb0))) & 0x4000) != 0);
  if ((haveMtx2 != 0) || ((*((u16 *) (c + 0x3ea))) != 0))
  {
    {
      Matrix4x3 *g = &data_020a0e68;
      char *m = c + 0x31c;
      p.x = 0;
      p.y = 0;
      p.z = 0;
      *g = *((Matrix4x3 *) m);
    }
    MulMat4x3Mat4x3((*((char **) (c + 0x314))) + 0x60, &data_020a0e68, &data_020a0e68);
    {
      int ty = *((int *) (((char *) (&data_020a0e68)) + 0x28));
      int tx = *((int *) (((char *) (&data_020a0e68)) + 0x24));
      p.y = ty;
      int tz = *((int *) (((char *) (&data_020a0e68)) + 0x2c));
      p.z = tz;
      p.x = tx;
      Vec3_Lsl(&d, &p, 3);
    }
    {
      int dy = *((int *) (((char *) (&d)) + 4));
      int dx = *((int *) (((char *) (&d)) + 0));
      x0 = dx;
      p.y = dy;
      int dz = *((int *) (((char *) (&d)) + 8));
      p.x = x0;
      p.z = dz;
    }
    int haveMtx3 = (int) (((*((u32 *) (c + 0xb0))) & 0x4000) != 0);
    if (haveMtx3 != 0)
    {
      *((int *) (c + 0x3f8)) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*((int *) (c + 0x3f8)), 0x13, p.x, p.y, p.z, 0, 0);
    }
    if ((*((u16 *) (c + 0x3ea))) != 0)
    {
      int haveShadow = (int) (((*((u32 *) (c + 0xb0))) & 0x40000) != 0);
      if (haveShadow != 0)
      {
        char *obj = *((char **) (c + 0xd0));
        if (obj != 0)
        {
          char *pos = obj + 0x5c;
          int dx = *((int *) pos);
          int gbase = (int) &data_020a0e68;
          dead.x = dx;
          int dy = *((int *) (pos + 4));
          int off5ec = 0x5ec;
          dead.y = dy;
          int dz = *((int *) (pos + 8));
          dead.z = dz;
          data_020a0e68 = *((Matrix4x3 *) (((char *) *((int *) (c + 0xd0))) + off5ec));
          {
            int tx = *(volatile int *) ((char *) &data_020a0e68 + 0x24); /* pin mtx translation loads */
            int ty = *(volatile int *) ((char *) &data_020a0e68 + 0x28);
            int tz = *(volatile int *) ((char *) &data_020a0e68 + 0x2c);
            p.x = tx;
            p.z = tz;
            pPos = (char *) &p;
            p.y = ty;
            Vec3_LslInPlace((Vector3 *) pPos, 3);
          }
          char *p2 = *((char **) (c + 0xd0));
          u16 a8c = *((volatile u16 *) (p2 + 0x8c)); /* pin angle loads */
          u16 a8e = *((volatile u16 *) (p2 + 0x8e));
          tmp[1] = a8e;
          tmp[0] = a8c;
          xSaved = p.x;
          x0 = *((volatile u16 *) (p2 + 0x90));
          s16 angY = (s16) tmp[1];
          tmp[2] = (u16) x0;
          u16 v580 = *((volatile u16 *) (p2 + 0x580));
          tmp[4] = *((u16 *) (p2 + 0x582));
          tmp[3] = v580;
          s16 v582s = (s16) tmp[4];
          tmp[5] = *((u16 *) (p2 + 0x584));
          int diff = (angY + 0x1800) - v582s;
          tmp[1] = (u16) diff;
          {
            u16 ang8e = tmp[1];
            u16 v582 = (u16) v582s;
            int py = p.y + 0x14000;
            p.y = py;
            p.y = py + (((int) data_02082214[(v582 >> 4) * 2]) * 0x1e);
            int idxX2 = (ang8e >> 4) * 2;
            p.x = xSaved + (((int) data_02082214[idxX2]) * 0x3c);
            p.z = p.z + (((int) data_02082214[idxX2 + 1]) * 0x3c);
          }
        }
      }
      *((int *) (c + 0x3fc)) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*((int *) (c + 0x3fc)), 0x19, p.x, p.y, p.z, 0, 0);
    }
  }
  func_ov102_0214b444(c);
}

}

/* ==========================================================================
 * ROM ordinal 15 -- func_ov102_0214b444, 0x0214b444, size 0xf8.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b444
void func_ov102_0214b444(void *cv)
{
    extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        int self, int sm, int mat, int fix, int t, unsigned int j);

    int c = (int)cv;
    int v;
    int b = (*(int*)(c + 0xb0) & 0x40000) != 0;
    if (b)
        return;

    v = *(int*)(c + 0x60);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv((void *)(c + 0x144)) == 0) {
        dBgCh_Gnd rg;
        rg.SetObjAndPos(*(const Vector3*)(c + 0x5c), (dActor_c*)0);
        if (rg.DetectClsn() != 0)
            v = rg.clsnY;
    }

    {
        int b2 = (*(int*)(c + 0xb0) & 0x4000) != 0;
        if (b2) {
            int *p = *(int**)(c + 0x390);
            if (p != 0) {
                if (*(int*)((int)p + 0xc8) != 0)
                    v = *(int*)((int)p + 0x60);
            }
        }
    }

    *(int*)(c + 0x3b8) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3bc) = v >> 3;
    *(int*)(c + 0x3c0) = *(int*)(c + 0x64) >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x364, c + 0x394, *(int*)(c + 0x80) * 0x50, 0x1e000, 0xf);
}

}

/* ==========================================================================
 * ROM ordinal 14 -- func_ov102_0214b3f0, 0x0214b3f0, size 0x54.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b3f0
void func_ov102_0214b3f0(void *c, int a1) {
    extern void func_ov102_0214bd20(void *c);

    unsigned int v = *(int*)((char*)c + 0x3dc) - 3;
    if (v <= 1u) return;
    unsigned short h = *(unsigned short*)((char*)c + 0x3ea);
    if (h > 4u) goto do_call;
    if (h != 0u) return;
do_call:
    *(int*)((char*)c + 0x390) = a1;
    func_ov102_0214bd20(c);
}

}

/* ==========================================================================
 * ROM ordinal 13 -- func_ov102_0214b3b8, 0x0214b3b8, size 0x38.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b3b8
void func_ov102_0214b3b8(void *c)
{
    extern void func_ov102_0214bc20(void *c);

    if (*(int *)((char *)c + 0x3dc) != 3) return;
    func_ov102_0214bc20(c);
    void *p = *(void **)((char *)c + 0x390);
    *(short *)((char *)c + 0x94) = *(short *)((char *)p + 0x8e);
    *(void **)((char *)c + 0x390) = 0;
}

}

/* ==========================================================================
 * ROM ordinal 12 -- func_ov102_0214b384, 0x0214b384, size 0x34.
 *
 * CALLED FROM OUTSIDE THIS TU: ov078 0x0212519c, and ov102's own
 * QuestionBlock run at 0x02149278.  It keeps external linkage.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b384
void func_ov102_0214b384(void *arg0, unsigned int arg1) {
    char *p = (char *)arg0;
    unsigned int cur = *(unsigned short *)(p + 0x3ea);
    if (cur == 0 || cur > arg1) {
        *(unsigned short *)(p + 0x3ea) = (unsigned short)arg1;
    }
    *(unsigned int *)(p + 0xb0) &= ~1u;
}

}

/* ==========================================================================
 * ROM ordinal 11 -- func_ov102_0214b248, 0x0214b248, size 0x13c.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b248
int func_ov102_0214b248(void *cv)
{
    extern unsigned short DecIfAbove0_Short(unsigned short *p);
    extern void func_0203568c(int *p, int v);
    extern void func_02035684(int *p, int v);
    extern void func_ov002_020ef228(void *c, int arg);

    char *c = (char *)cv;

    if (DecIfAbove0_Short((unsigned short *)(c + 0x3ea))) {
        *(unsigned int *)(c + 0x3e4) = Sound::PlayLong(
            *(unsigned int *)(c + 0x3e4), 3, 0x188, *(Vector3 *)(c + 0x74), 0);
    }

    {
        unsigned short st = *(unsigned short *)(c + 0x3ea);
        if (st != 0 && st <= 4) {
            *(int *)(((int)c + 0x128)) &= ~0x8000;
            if (*(unsigned short *)(c + 0x3ea) == 1) {
                func_ov102_0214ae1c(c);
                return 0;
            }
            {
                int v = ((5 - *(unsigned short *)(c + 0x3ea)) << 12) / 4 + 0x1000;
                *(int *)(c + 0x88) = v;
                *(int *)(c + 0x84) = *(int *)(c + 0x88);
                *(int *)(c + 0x80) = *(int *)(c + 0x84);
                *(int *)(c + 0x114) = v * 0x3c;
                *(int *)(c + 0x118) = v * 0x50;
                func_0203568c((int *)(c + 0x144), v * 0x3c);
                func_02035684((int *)(c + 0x144), v * 0x3c);
                func_ov002_020ef228(c + 0x144, (int)c);
            }
            {
                int *f = (int *)(((long long)(int)(c + 0x128)));
                *f &= ~4;
                if (*(unsigned short *)(c + 0x3ea) == 2) {
                    *f |= 0x4000;
                }
            }
        } else {
            func_ov102_0214b128(c);
        }
    }
    return 1;
}

}

/* ==========================================================================
 * ROM ordinal 10 -- func_ov102_0214b128, 0x0214b128, size 0x120.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b128
void func_ov102_0214b128(void *cv) {
    extern int _ZN8dActor_c7FindEggER5dCc_c(void*, void*);
    extern int func_02010304(void* target, char* p);
    extern void func_ov102_0214b3f0(void* c, int a1);
    extern void func_ov102_0214bc20(void* c);
    extern void _ZN10dBgCh_Actr15ClearGroundFlagEv(void*);

    char *c = (char *)cv;
    int t;
    if (_ZN8dActor_c7FindEggER5dCc_c(c, c + 0x110) || (*(int*)(c + 0x130) & 0x20000)) {
        int* p = (int*)(((int)c + 0xb0));
        *(short*)(c + 0x3ea) = 4;
        *p = *p & ~1;
        *(int*)(c + 0x98) = 0;
        return;
    }
    t = func_02010304(c, c + 0x110);
    if (t) {
        func_ov102_0214b3f0(c, t);
        return;
    }
    t = *(int*)(c + 0x130);
    if (t & 0x40000) {
        func_ov102_0214b384(c, 4);
        return;
    }
    if (!(t & 0x380)) return;
    t = *(int*)(c + 0x3dc);
    if (t < 0) return;
    if (t > 2) return;
    {
        void* r5 = _ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c + 0x134));
        if (!r5) return;
        func_ov102_0214bc20(c);
        *(short*)(c + 0x94) = *(short*)((char*)r5 + 0x8e);
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x110);
        _ZN10dBgCh_Actr15ClearGroundFlagEv(c + 0x144);
    }
}

}

/* ==========================================================================
 * ROM ordinal 9 -- func_ov102_0214b03c, 0x0214b03c, size 0xec.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b03c
void func_ov102_0214b03c(void *cv){
  extern int _ZNK9Animation12WillHitFrameEi(void*, int);
  extern int func_0201267c(int, void*);
  extern int func_ov102_0214bf64(void*);
  extern int func_ov102_0214bd90(void*);

  char *c = (char *)cv;
  if(*(int*)(c+0x3dc) < 2 && *(int*)(c+0x360) == *((int *)((char *)&data_ov102_0214e9c0 + 4))){
    if(_ZNK9Animation12WillHitFrameEi(c+0x350, 0) != 0 || _ZNK9Animation12WillHitFrameEi(c+0x350, 0x10) != 0){
      func_0201267c(0x132, c+0x74);
    }
  }
  switch(*(int*)(c+0x3dc)){
  case 0: func_ov102_0214bf64(c); return;
  case 1: ((daBmb_c *)c)->State1(); return;
  case 2: func_ov102_0214bd90(c); return;
  case 3: ((daBmb_c *)c)->State3(); return;
  case 4: ((daBmb_c *)c)->State4(); return;
  case 5: ((daBmb_c *)c)->State5(); return;
  }
}

}

/* ==========================================================================
 * ROM ordinal 8 -- func_ov102_0214ae1c, 0x0214ae1c, size 0x220.
 *
 * CALLED FROM OUTSIDE THIS TU: ov014 0x021122c0 and ov098 0x0213ad80.  It
 * keeps external linkage.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214ae1c
void func_ov102_0214ae1c(void *cv) {
    typedef struct Bmb_Ae1cVec3 { int x, y, z; } Vec3;
    extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int p, Vec3 *pos, void *rot, int a, int b);
    extern void func_0201267c(int a, void *b);
    extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
    extern void func_ov002_020ef228(void *a, void *b);
    extern void _ZN6Player9DropActorEv(void *self);
    extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, Vec3 *v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);
    extern void func_ov002_020d718c(void *a);

    char *c = (char *)cv;
    Vec3 v;
    int y, z, w;
    *(unsigned int *)(((int)c + 0x128)) &= ~0x8000;
    y = *(int *)(c + 0x60);
    z = *(int *)(c + 0x64);
    w = y + 0x78000;
    v.x = *(int *)(c + 0x5c);
    v.y = w;
    v.z = z;
    if (*(unsigned char *)(c + 0x108)) {
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x120, 2, &v, 0, *(signed char *)(c + 0xcc), -1);
        *(unsigned char *)(c + 0x108) = 0;
    }
    func_0201267c(0x10c, c + 0x74);
    v.y = *(int *)(c + 0x60) + 0x64000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x14, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x15, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x16, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x17, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x18, v.x, v.y, v.z);
    func_ov002_020ef228(c + 0x144, c);
    if (*(void **)(c + 0x390)) {
        Vec3 w;
        _ZN6Player9DropActorEv(*(void **)(c + 0x390));
        w.x = *(int *)(c + 0x5c);
        w.y = *(int *)(c + 0x60);
        w.z = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void **)(c + 0x390), &w, 2, 0xc000, 1, 0, 1);
        *(void **)(c + 0x390) = 0;
    } else if (*(unsigned int *)(c + 0x134)) {
        void *a = _ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0x134));
        if (a) {
            if (*(unsigned int *)(c + 0x130) & 0x400000) {
                Vec3 w;
                w.x = *(int *)(c + 0x5c);
                w.y = *(int *)(c + 0x60);
                w.z = *(int *)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &w, *(unsigned int *)(c + 0x3e0), 0xc000, 1, 0, 1);
            }
        }
    }
    if (*(void **)(c + 0xd0)) {
        Vec3 w;
        w.x = *(int *)(c + 0x5c);
        w.y = *(int *)(c + 0x60);
        w.z = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void **)(c + 0xd0), &w, *(unsigned int *)(c + 0x3e0), 0xc000, 1, 0, 1);
        func_ov002_020d718c(*(void **)(c + 0xd0));
        *(void **)(c + 0xd0) = 0;
    }
    func_ov102_0214baa0(c);
}

}

/* ==========================================================================
 * ROM ordinal 7 -- daBmb_c::OnTurnIntoEgg, 0x0214adc8, size 0x54.
 * Vtable slot 19.
 * ======================================================================== */

// @symbol _ZN7daBmb_c13OnTurnIntoEggER6Player
int daBmb_c::OnTurnIntoEgg(Player &player)
{
    if (unk_108 == 1) {
        Sound::PlayBank3(0x11, *(Vector3 *)&mCamSpacePosX);
        GiveCoins(player.mPlayerNo, 1);
        player.Heal(0x100);
    }
    func_ov102_0214baa0((char *)this);
}

/* ==========================================================================
 * ROM ordinal 6 -- func_ov102_0214ad40, 0x0214ad40, size 0x88.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214ad40
void func_ov102_0214ad40(void *cv)
{
    char *c = (char *)cv;

    if ((unsigned char)(*(unsigned char *)(c + 0x3f5) + 0xfe) > 1)
        return;
    if (*(int *)(c + 0x60) >= *(int *)(c + 0x3c8) - 0x12c000)
        return;
    {
        unsigned short v = *(unsigned short *)(c + 0x3ea);
        if (v > 4)
            ;
        else if (v != 0)
            return;
    }
    func_ov102_0214b384(c, 4);
    *(unsigned char *)(c + 0x3f5) = 3;
    {
        int *p = (int *)(((int)c + 0x128));
        int v = *p;
        *p = v & ~2;
    }
    func_ov102_0214c0b8(c);
}

}

/* ==========================================================================
 * ROM ordinal 5 -- func_ov102_0214ad14, 0x0214ad14, size 0x2c.
 *
 * CALLED FROM OUTSIDE THIS TU: ov002 0x020f1740, ov078 0x02125284, and
 * ov102's own QuestionBlock run at 0x0214926c.  It keeps external linkage.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214ad14
void func_ov102_0214ad14(void *c)
{
    extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);

    func_ov102_0214b384(c, 0x96);
    *(void **)((char *)c + 0x38c) = _ZN8dActor_c13ClosestPlayerEv(c);
    func_ov102_0214c0b8(c);
}

}

/* ==========================================================================
 * ROM ordinal 4 -- func_ov102_0214ab1c, 0x0214ab1c, size 0x1f8.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214ab1c
int func_ov102_0214ab1c(void *selfv)
{
    extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void *self, void *clsn);
    extern int _ZN6Player16IsInsideOfCannonEv(void *player);
    extern int _ZN6Player22IsBeingShotOutOfCannonEv(void *player);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int i, int fx, unsigned int j);
    extern int _ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(void *self, void *clsn);

    u8 *self = (u8 *)selfv;
    void *player;

    int r4 = _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(self, self + 0x144);
    if (r4 == 0) {
        goto ret0;
    }

    *(unsigned int *)(((int)(self + 0xb0))) &= ~0x10000000u;

    player = *(void **)(self + 0xd0);
    if (player != 0) {
        if (_ZN6Player16IsInsideOfCannonEv(player) != 0) {
            goto shocked;
        }
        if (_ZN6Player22IsBeingShotOutOfCannonEv(player) != 0) {
            goto shocked;
        }
        if (func_ov102_0214b248(self) == 0) {
            *(unsigned int *)(((int)(self) + 0xb0)) &= ~0xe0000u;
            self[0x107] = 0;
            return 1;
        }
        goto after_cannon;
    shocked:
        func_ov102_0214b384(self, 3);
    }
after_cannon:

    if (self[0x3f5] == 2) {
        self[0x3f5] = 3;
        *(int *)(self + 0x3dc) = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, *(void **)((char *)&data_ov102_0214e9c0 + 4), 0, 0x1000, 0);
        *(u16 *)(self + 0x3e8) = 0x200;
        *(int *)(self + 0x9c) = -0x2000;
        *(unsigned int *)(((int)(self + 0x128))) &= ~2u;
    }

    if (r4 == 2) {
        func_ov102_0214b384(self, 0x50);
    }

    if (r4 == 3) {
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x144) != 0) {
            self[0x3f4] = 3;
        }
    }

    if (self[0x107] != 0) {
        if (*(u16 *)(self + 0x104) == 5) {
            *(unsigned int *)(((int)(self + 0x128))) &= ~0x8000u;
        }
    }

    if (_ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(self, self + 0x110) != 0) {
        goto hit;
    }
    if (r4 != 3 || _ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x144) == 0) {
        goto skip_hit;
    }

hit:
    func_ov102_0214b384(self, 4);
    self[0x107] = 0;
    return 1;

skip_hit:
    _ZN5dCc_c5ClearEv(self + 0x110);
    if (self[0x107] != 0) {
        if (*(u16 *)(self + 0x104) == 0) {
            _ZN5dCc_c6UpdateEv(self + 0x110);
        }
    }
    func_ov102_0214b53c((char *)self);
    return 1;

ret0:
    return 0;
}

}

/* ==========================================================================
 * ROM ordinal 3 -- func_ov102_0214aa18, 0x0214aa18, size 0x104.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214aa18
int func_ov102_0214aa18(void *selfv)
{
    extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *);
    extern void func_0200fc44(char *c, void *v, int x);

    char *s = (char*)selfv;
    if (*(unsigned char*)(s + 0x3f5) == 2) {
        if (*(int*)(s + 0xc8) != 0) {
            func_ov102_0214b53c(s);
            return 1;
        }
        _ZN8dActor_c9UpdatePosEP5dCc_c(s, s + 0x110);
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(s, s + 0x144, 0);
        func_ov102_0214b53c(s);
        func_ov102_0214ad40(s);
        _ZN5dCc_c5ClearEv(s + 0x110);
        _ZN5dCc_c6UpdateEv(s + 0x110);
        if (_ZNK10dBgCh_Actr13JustHitGroundEv(s + 0x144)) {
            Vector3 v;
            v.x = *(int*)(s + 0x5c);
            v.y = *(int*)(s + 0x60);
            v.z = *(int*)(s + 0x64);
            func_0200fc44(s, &v, 1);
        }
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(s + 0x144) == 0)
            return 1;
        int *fl = (int*)(((int)s + 0x128));
        *fl = *fl & ~2;
        *(unsigned char*)(s + 0x3f5) = 3;
        func_ov102_0214c0b8(s);
        return 1;
    }
    return 0;
}

}

/* ==========================================================================
 * ROM ordinal 2 -- daBmb_c::OnAimedAtWithEgg, 0x0214aa10, size 0x8.
 * Vtable slot 29.
 *
 * Below this, at 0x0214a9b4 and 0x0214a96c, the compiler emits D0 and D1 from
 * the header's inline destructor body.  Nothing is written for them here.
 * ======================================================================== */

// @symbol _ZN7daBmb_c16OnAimedAtWithEggEv
s32 daBmb_c::OnAimedAtWithEgg() {
    return 204800;
}
