//cpp
/* Production translation unit for ov009/daObjMcWater_c, hand-curated.
 * 8 function(s), .text 0x02111a70..0x02111dc4.
 *
 * The castle water plane (profile MC_WATER): the moat outside and the flooded
 * basement inside are the same actor. It never moves under its own power. Its
 * whole per-frame job is to keep the surface texture sliding, and its whole
 * init job is to put the collision sheet somewhere and tell the rest of the
 * level how low the water is.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov009 0x021139e0  "14daObjMcWater_c"
 *   _ZTI  ov009 0x021139d4  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov009 0x02113a18  the ADDRESS POINT itself: the two words below it
 *                           at 0x02113a10 are a zero offset-to-top and
 *                           &_ZTI (0x021139d4).
 *   size  0x338             the factory's own `mov r0, #0x338` at 0x02111d90.
 * The coined `CastleWater` spelling this class used to carry is gone; the ROM's
 * own type string is where the class name now comes from.
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the cartridge's 32-word table point
 * inside ov009; every other slot still holds dBgActor_c's arm9 word, including
 * slot 31 (Kill, 0x020ee55c), so nothing else is overridden.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in the header and
 * declared FIRST, so it emits last, which is where the cartridge has it.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x02111a70  src/_ZN14daObjMcWater_cD1Ev.cpp
 *   [1] 0x02111abc  src/_ZN14daObjMcWater_cD0Ev.cpp
 *   [2] 0x02111b1c  src/func_ov009_02111b1c.cpp
 *   [3] 0x02111bd4  src/_ZN14daObjMcWater_c16CleanupResourcesEv.cpp
 *   [4] 0x02111c18  src/_ZN14daObjMcWater_c6RenderEv.cpp
 *   [5] 0x02111c4c  src/_ZN14daObjMcWater_c8BehaviorEv.cpp
 *   [6] 0x02111c74  src/_ZN14daObjMcWater_c13InitResourcesEv.cpp
 *   [7] 0x02111d8c  src/daObjMcWater_c_classInit.c
 */

#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
#include "common.h"
#include "daObjMcWater_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * data_ov009_02113c68 is a PAIR of words, not one: InitResources hands the
 * first to Model::LoadFile as a SharedFilePtr and then reads the second as the
 * loaded BMD to prepare the texture animation against, so it is declared as a
 * two-word struct rather than as the SharedFilePtr the collision handle next
 * to it is.
 *
 * data_0209caa0 carried two different spellings in the legacy files -- a bare
 * int in InitResources, a three-int record in the helper. The record wins here
 * and InitResources reaches the same word through a byte offset, which is what
 * its own body already spelled.
 *
 * TextureTransformer::SetFile and dBgW_KcMbg::SetFile keep their mangled free
 * spelling: both take Fix12<int> BY VALUE, and an ordinary member call would
 * trigger mwccarm's by-value-class parameter homing and change the ROM ABI.
 *
 * func_ov009_02111b1c is this class's own private helper -- see its definition
 * at the bottom of this file. It is a global symbol the cartridge calls with
 * `this`, so it keeps C linkage and its existing name; renaming it would move
 * a symbol other config still names. */
typedef int Fix12i;

struct Vector3_16;
struct D0209caa0 { int a, b, c; };
struct BMD_File; struct BTA_File; struct KCL_File; struct CLPS_Block;
struct DataPtr { int f[2]; };

/* Model's own draw entry, slot 5 of _ZTV5Model. The legacy Render reached it
 * through a six-slot stand-in rather than through Model's real declaration,
 * and that stand-in is what the matched body was measured against. */
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

extern "C" {
void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int a, unsigned int b, const Vector3 &, const Vector3_16 *, int, int);
extern unsigned char data_0209f2d8[];
extern D0209caa0 data_0209caa0;
void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, struct BTA_File &bta);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *self, struct BTA_File &bta, int a, int b, unsigned int c);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
void func_ov009_02111b1c(char *thiz);
extern struct DataPtr data_ov009_02113c68;
extern struct BTA_File data_ov009_02112bc4;
extern struct SharedFilePtr data_ov009_02113c70;
extern struct CLPS_Block data_ov009_02112c38;
extern int data_0209f32c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjMcWater_c_classInit, 0x02111d8c, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol daObjMcWater_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
/* resolved: VT0 = _ZTV14daObjMcWater_c */
/* Reconstructed source-style name. Historical alias: CastleWater_Spawn. */
int *daObjMcWater_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(824);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* This TU defines every virtual of the class, so mwccarm emits the
           vtable HERE and its _ZTV names the emitted OBJECT -- the two-word
           Itanium preamble at 0x02113a10 -- not the address point. The [2]
           steps over the preamble onto the address point the cartridge
           actually stores, 0x02113a18. */
        p[0] = (int)&_ZTV14daObjMcWater_c[2];
        /* The TextureTransformer at 0x320 is the one member with a constructor
           of its own; the cartridge runs it here, after the base, rather than
           through a member-init list. */
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN14daObjMcWater_c13InitResourcesEv, 0x02111c74, size 0x118 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c13InitResourcesEv
/* Loads the model and the collision mesh, wires up the texture scroll, places
 * the platform, and enables the collider.
 *
 * Two things reach outside this object. One level flag (bit 0x80000 of
 * data_0209caa0+8, and only when data_0209f2d8 != 1) DROPS the water to
 * -0x2bc000 before anything else -- that is the castle moat draining after the
 * cannon is opened. And near the end it lowers the GLOBAL data_0209f32c to
 * mPosY - 0x64000 if that is lower, so the water publishes a plane the rest of
 * the level reads rather than keeping its height to itself.
 *
 * The collider is given mClsnMat and mAngleY directly, which is why those two
 * are real members of the base rather than markers.
 */
int daObjMcWater_c::InitResources()
{
    char *self = (char *)this;
    int b = (int)(data_0209f2d8[0] == 1);
    if (b == 0) {
        if (*(int *)((char *)&data_0209caa0 + 8) & 0x80000) {
            mPosY = -0x2bc000;
        }
    }
    {
        struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(struct SharedFilePtr *)&data_ov009_02113c68);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, bmd, 1, 0x14);
    }
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File((void *)data_ov009_02113c68.f[1], data_ov009_02112bc4);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(&mTexTransformer, data_ov009_02112bc4, 0, 0x1000, 0);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov009_02113c70);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, mClsnMat, 0x1000, mAngleY, data_ov009_02112c38);
    }
    ((dBgW *)&mMeshCollider)->Enable((dActor_c *)(self));
    {
        int v = mPosY - 0x64000;
        if (data_0209f32c > v) data_0209f32c = v;
    }
    func_ov009_02111b1c(self);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN14daObjMcWater_c8BehaviorEv, 0x02111c4c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c8BehaviorEv
/* The whole frame: force the scroll rate, advance the texture animation.
 *
 * The speed is rewritten to 0x1000 EVERY frame rather than once at init, so the
 * scroll runs at a fixed rate no matter what else touched it.
 */
int daObjMcWater_c::Behavior()
{
    mTexTransformer.speed = 0x1000;
    ((Animation *)&mTexTransformer)->Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN14daObjMcWater_c6RenderEv, 0x02111c18, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c6RenderEv
/* Pushes the texture transform into the model's components, then draws through
 * the model's own vtable slot 5. The components are not a separate sub-object:
 * 0x0dc is Model::data, +0x8 inside the Model at 0x0d4 -- the cartridge's own
 * destructor proves the Model's extent (tools/dtor_members.py), so this is the
 * model's own field rather than a marker.
 */
int daObjMcWater_c::Render()
{
    _ZN18TextureTransformer6UpdateER15ModelComponents(&mTexTransformer, &mModel.data);
    Sub *b = (Sub *)&mModel;
    b->m(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN14daObjMcWater_c16CleanupResourcesEv, 0x02111bd4, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c16CleanupResourcesEv
/* Releases the model and collision files, then disables the mesh collider --
 * but only if it is still enabled, so a second cleanup is harmless.
 */
int daObjMcWater_c::CleanupResources()
{
    ((SharedFilePtr *)((void *)&data_ov009_02113c68))->Release();
    ((SharedFilePtr *)((void *)&data_ov009_02113c70))->Release();
    if (((dBgW *)&mMeshCollider)->IsEnabled())
        ((dBgW *)&mMeshCollider)->Disable();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov009_02111b1c, 0x02111b1c, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov009_02111b1c
/* This class's private "scatter the surface effects" helper, called once from
 * InitResources with `this`. It spawns seven copies of actor 0xc5 -- the water
 * ring/splash decoration -- at seven fixed positions, and WHICH seven depends
 * on where the level is: one table for the outside moat, and two more for the
 * basement, chosen by the same 0x80000 flag InitResources reads. Outside, it
 * gives up entirely if the water has already been dropped (mPosX > 0).
 *
 * KEPT AT RAW OFFSETS AND C LINKAGE ON PURPOSE. The cartridge gives it a global
 * symbol and other config still names it, so it is not yet a member; promoting
 * it to a real method is a separate change with its own codegen risk. The
 * comments say what each offset is.
 */
extern "C" {
void func_ov009_02111b1c(char *thiz)
{
    char *c = thiz;
    Vector3 *vec;
    int i;
    int b = (int)(data_0209f2d8[0] == 1);
    if (b != 0) {
        if (*(int *)(c + 0x5c) > 0) return;   /* mPosX -- already drained */
        vec = data_ov009_02113de0;
    } else {
        vec = (data_0209caa0.c & 0x80000) ? data_ov009_02113e34 : data_ov009_02113d8c;
    }
    for (i = 0; i < 7; i++) {
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0xc5, 0, vec[i], 0, *(signed char *)(c + 0xcc), -1);   /* 0xcc = mAreaID */
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14daObjMcWater_cD0Ev, 0x02111abc, size 0x60         */
/* ROM ordinal 0 -- _ZN14daObjMcWater_cD1Ev, 0x02111a70, size 0x4c         */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_cD1Ev
// @symbol _ZN14daObjMcWater_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjMcWater_c.h and declared FIRST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it first is what makes this TU the vtable's home, so _ZTV and the
 * RTTI pair land here rather than in whichever other TU happens to name them.
 *
 * Both bodies are the same short chain: this class's vptr store, the
 * TextureTransformer at 0x320, then dBgActor_c's own -- inlined, its destructor
 * is defined in its class body -- which is the dBgW_KcMbg at 0x124, the Model
 * at 0x0d4, and dActor_c. D0's trailing deallocation is the inherited inline
 * operator delete, which is why nothing here names a heap.
 */
