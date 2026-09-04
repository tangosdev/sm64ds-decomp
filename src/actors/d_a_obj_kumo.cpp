//cpp
/* Production translation unit for ov039/daObjKumo_c, hand-curated.
 * 8 function(s), .text 0x021111a0..0x021113b4.
 *
 * A cloud actor (`kumo` is the ROM's own word for it). It derives DIRECTLY
 * from dActor_c -- the cartridge's RTTI says so -- and adds one member, a
 * Model at 0xd4, which closes the class on the factory's literal 0x124. It
 * fills four vtable slots: InitResources (0), CleanupResources (3),
 * Behavior (6) and Render (9).
 *
 * ONE OF THE SEVEN IS NOT A METHOD. func_ov039_02111214 sits between D0 and
 * CleanupResources in the ROM's own .text order, so it is part of this TU and
 * cannot be split out; it is a file-local helper that InitResources calls to
 * seed the model's rotation matrix and scale from the actor's fields. It keeps
 * C linkage and its unmangled ROM name, which is why it is wrapped in its own
 * `extern "C"` block below rather than being made a member.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN11daObjKumo_cD1Ev.cpp
 *   [1] 0x021111d0  src/_ZN11daObjKumo_cD0Ev.cpp
 *   [2] 0x02111214  src/func_ov039_02111214.c
 *   [3] 0x02111254  src/_ZN11daObjKumo_c16CleanupResourcesEv.cpp
 *   [4] 0x02111278  src/_ZN11daObjKumo_c6RenderEv.cpp
 *   [5] 0x021112a0  src/_ZN11daObjKumo_c8BehaviorEv.cpp
 *   [6] 0x0211132c  src/_ZN11daObjKumo_c13InitResourcesEv.cpp
 *   [7] 0x0211137c  src/Cloud_Spawn.c
 *
 * THE EIGHTH IS THE FACTORY. Cloud_Spawn is the OBJ_KUMO registry profile's
 * spawn function and sits immediately after InitResources in the ROM's own
 * .text order, so it is part of this TU; it was outside only because the
 * promotion predated the profile-reconstruction campaign. It keeps C linkage
 * and is written first here, being the highest-address member.
 */

#include "daObjKumo_c.h"
#include "SharedFilePtr.h"
#include "decl_common.h"

extern "C" {
extern void Matrix4x3_FromRotationY(void *, int);
extern int data_ov039_021118e4[];
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
/* Defined further down this same file -- InitResources is written above it
   because the emitted order is the reverse of the source order. */
void func_ov039_02111214(char *t);
extern int data_ov039_021118e0;

/* The factory's own dependencies, restated here rather than pulled in through
   decl_Actor.h / decl_ActorBase.h / decl_Model.h as the legacy file did -- this
   TU declares in place, and adding those headers would change what the seven
   already-matching members see. _ZTV11daObjKumo_c needs no restatement: the
   decl_common.h this TU already includes declares it (line 476 there). */
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN8dActor_cC2Ev(void *self);
extern void _ZN5ModelC1Ev(void *self);
}

/* FILE-GLOBAL BY MEASUREMENT. `opt_propagation` is not positional in mwccarm
   2004/b56 -- it is file-global last-wins -- so the legacy Behavior source's
   `#pragma opt_propagation off` has to sit at the top of the merged TU. All
   seven members were re-verified with it here: 7/7 MATCH, so no other member
   pays for it. */
#pragma opt_propagation off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- Cloud_Spawn, 0x0211137c, size 0x38                        */
/* -------------------------------------------------------------------------- */
// @symbol Cloud_Spawn
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV11daObjKumo_c */
/* The OBJ_KUMO profile's factory. 292 = 0x124 is the whole object, matching the
   header's size assert; the inlined dActor_c constructor runs first, this class
   stores its own vptr over the base's, and the one member -- Model at 0xd4 --
   is constructed last, which is what closes the class on 0x124. */
extern "C" int *Cloud_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(292);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV11daObjKumo_c;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN11daObjKumo_c13InitResourcesEv, 0x0211132c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjKumo_c::InitResources()
{
  void *f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov039_021118e4);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, f, 1, 2);
  func_ov039_02111214((char *)this);
  data_ov039_021118e0++;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN11daObjKumo_c8BehaviorEv, 0x021112a0, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_c8BehaviorEv
int daObjKumo_c::Behavior()
{
    int d = Vec3_Dist((const Vector3*)&mCamSpacePosX, (const Vector3*)&mPosX);
    int rank = 1;
    int base = 2;
    dActor_c *actor = FindWithActorID(0x13a, 0);
    while (actor) {
        if (actor != this) {
            int d2 = Vec3_Dist((const Vector3*)&mCamSpacePosX, (const Vector3*)&actor->mPosX);
            if (d > d2)
                rank++;
        }
        actor = FindWithActorID(0x13a, actor);
    }
    mModel.SetPolygonID((base + rank) & 0xff);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN11daObjKumo_c6RenderEv, 0x02111278, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daObjKumo_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN11daObjKumo_c16CleanupResourcesEv, 0x02111254, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_c16CleanupResourcesEv
int daObjKumo_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov039_021118e4)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov039_02111214, 0x02111214, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov039_02111214(char *t)
{
    Matrix4x3_FromRotationY(t + 0xf0, *(short *)(t + 0x8e));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11daObjKumo_cD0Ev, 0x021111d0, size 0x44               */
/* ROM ordinal 0 -- _ZN11daObjKumo_cD1Ev, 0x021111a0, size 0x30               */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_cD1Ev
// @symbol _ZN11daObjKumo_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKumo_c() {}` in the header is
   the whole source of both variants: from an inline body mwcc emits D1 and
   then D0 -- the cartridge's own order -- and no D2. Writing the body out of
   line here instead flips them to D0-before-D1 and the isolation step rejects
   the object.

   Their bodies are one vptr store, then mModel destroyed, then ~dActor_c --
   the direct-base chain the RTTI states. D0's trailing deallocation is the
   inline `operator delete` it inherits, which is why nothing here names a
   heap. */
