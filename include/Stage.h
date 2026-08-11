#ifndef STAGE_H
#define STAGE_H

#include "Scene.h"
#include "Model.h"
#include "MeshCollider.h"

struct SceneRelated;
struct LVL_Overlay;

/* Particle::SysTracker, embedded at Stage+0x50. Not its own header yet --
 * include/Particle.h and include/Particle__SysTracker.h are two SEPARATE
 * gen_header.py shadows of this SAME class (confirmed by
 * src/_ZN8Particle10SysTrackerC1Ev.c, which writes fields through
 * `struct Particle *self` up to unk_818, and
 * src/_ZN8Particle10SysTracker10InitialiseEv.cpp / 6UpdateEv.c, which read
 * unk_004/unk_008 through `struct Particle__SysTracker *self` -- the same
 * offsets Particle.h also carries). Their union is what is declared here:
 * Particle.h's full 34-field layout, last field unk_818 (1 byte, ends
 * 0x819), padded to 0x81c for 4-byte alignment -- which is exactly the gap
 * Stage's own D1/D0 give this member (0x50..0x86c). Declared locally
 * rather than merging the two real headers because neither Stage source
 * file includes them and a merge is its own change with its own blast
 * radius across every other file that already casts through one shadow or
 * the other.
 *
 * The destructor is declared, never defined here -- src/_ZN8Particle10SysTrackerD1Ev.cpp
 * already supplies _ZN8Particle10SysTrackerD1Ev as an extern "C" free
 * function; this declaration only lets Stage's implicit destructor find it
 * by name. Not virtual: dtor_variant_audit.py already established
 * Particle::SysTracker has no RTTI record and no _ZTV, so it is not
 * polymorphic and must not gain a vtable pointer here. */
namespace Particle {
struct SysTracker {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    u8  unk_008;            /* 0x008 */
    u8  pad_009[0x747];
    s32 unk_750;            /* 0x750 */
    u8  mParticle1;          /* 0x754 */
    u8  pad_755[0x7];
    s32 unk_75c;            /* 0x75c */
    u8  mParticle2;          /* 0x760 */
    u8  pad_761[0x7];
    s32 unk_768;            /* 0x768 */
    u8  unk_76c;            /* 0x76c */
    u8  pad_76d[0x7];
    s32 unk_774;            /* 0x774 */
    u8  unk_778;            /* 0x778 */
    u8  pad_779[0x7];
    s32 unk_780;            /* 0x780 */
    u8  unk_784;            /* 0x784 */
    u8  pad_785[0x7];
    s32 unk_78c;            /* 0x78c */
    u8  unk_790;            /* 0x790 */
    u8  pad_791[0x7];
    s32 unk_798;            /* 0x798 */
    u8  mParticle3;          /* 0x79c */
    u8  pad_79d[0x7];
    s32 unk_7a4;            /* 0x7a4 */
    u8  mParticle4;          /* 0x7a8 */
    u8  pad_7a9[0x7];
    s32 unk_7b0;            /* 0x7b0 */
    u8  unk_7b4;            /* 0x7b4 */
    u8  pad_7b5[0xb];
    s32 unk_7c0;            /* 0x7c0 */
    u8  unk_7c4;            /* 0x7c4 */
    u8  pad_7c5[0xf];
    u8  unk_7d4;            /* 0x7d4 */
    u8  pad_7d5[0xf];
    u8  unk_7e4;            /* 0x7e4 */
    u8  pad_7e5[0xb];
    u8  unk_7f0;            /* 0x7f0 */
    u8  pad_7f1[0x3];
    u8  unk_7f4;            /* 0x7f4 */
    u8  pad_7f5[0x3];
    u8  unk_7f8;            /* 0x7f8 */
    u8  pad_7f9[0x3];
    s32 unk_7fc;            /* 0x7fc */
    u8  unk_800;            /* 0x800 */
    u8  pad_801[0x3];
    s32 unk_804;            /* 0x804 */
    u8  unk_808;            /* 0x808 */
    u8  pad_809[0x7];
    u8  unk_810;            /* 0x810 */
    u8  pad_811[0x3];
    u8  unk_814;            /* 0x814 */
    u8  pad_815[0x3];
    u8  unk_818;            /* 0x818 */
    u8  pad_819[0x3];       /* rounds 0x819 up to the 0x81c alignment boundary */

    ~SysTracker();
};

/* Deterministic from the field list above (0x819 + the explicit 3-byte pad),
   not from compiler struct-alignment rounding -- and it is also exactly the
   gap Stage's own D1/D0 give this member, a second independent check on the
   same number. */
typedef char SysTracker_size_must_be_0x81c[sizeof(SysTracker) == 0x81c ? 1 : -1];
}

/* The playable level: ActorBase -> ActorDerived -> Scene -> Stage.
 *
 * The generated header this replaces named no base and re-declared the whole of
 * ActorBase inline -- uniqueID at 0x004, actorID at 0x00c, the three list nodes,
 * a pad to 0x050 -- so `Stage` and `Scene` were unrelated types even though the
 * ROM has one derived from the other. Everything below 0x050 is gone from this
 * file now; it comes from the base chain, which owns it.
 *
 * DERIVATION. The ROM's type graph (tools/rtti_extract.py) has dScStage_c at
 * 0x02092158, vtable 0x020921c0 -- which is _ZTV5Stage -- and its single base is
 * dScene_c. It is a leaf: no record in the image names dScStage_c as a base.
 *
 * VTABLE. _ZTV5Stage is 18 slots, the same shape Scene and ActorBase have, and
 * Stage adds no virtual of its own. It overrides six functionally --
 *
 *     0  InitResources        3  CleanupResources     6  Behavior
 *     9  Render              12  OnPendingDestroy     1  BeforeInitResources
 *
 * -- plus the destructor pair at 16/17. The remaining ten still point at Scene's
 * Before/After hooks or at ActorBase.
 *
 * KEY FUNCTION. Slot 0 is Stage::InitResources, so declaration order matters
 * here in the way include/Actor.h warns about: whichever non-inline virtual is
 * declared first is the key function, and CW 1.2 emits the vtable group into the
 * TU that DEFINES it -- colliding with the copy the module's gap object supplies
 * from ROM data. The destructor is declared first, which is free for a derived
 * class (an override takes its base's slot wherever it is written) and pins the
 * role to ~Stage. tools/objisolate.py makes that TU eligible anyway (see
 * include/ActorDerived.h and include/Scene.h), so ~Stage is now a real method,
 * defined identically -- `Stage::~Stage() {}` -- in both src/_ZN5StageD1Ev.cpp
 * and src/_ZN5StageD0Ev.cpp. Unlike Scene, Stage does NOT need to define it
 * inline in the class body: Stage is a leaf (no record in the image names
 * dScStage_c as a base, per DERIVATION above), so nothing derives from it that
 * would need to inline ITS destructor in turn. Stage's own destructor inlining
 * Scene's (and Scene inlining ActorDerived's) is what required Scene.h's move;
 * see the note there.
 *
 * SIZE IS DELIBERATELY NOT ASSERTED, unlike the three headers above this one.
 * The last field here is the last one any matched function has been observed to
 * touch, which is not the same as the last field the object has; asserting a
 * size would turn "we have not seen past 0x9bc" into "the object ends at 0x9bc".
 * Nothing needs the number either -- dScStage_c is a leaf, so no derived header
 * has to start its own fields after it. The offsets below ARE checked:
 * tools/check_header_offsets.py walks them from Scene's asserted 0x50.
 *
 * Field NAMES are placeholders and cannot change codegen. Offsets and widths are
 * observed.
 */
struct Stage : Scene {
    Particle::SysTracker mSysTracker;  /* 0x050 */
    Model mModel;             /* 0x86c */
    u8  pad_8bc[0x60];       /* Model ends 0x8bc; MeshCollider does not start until 0x91c */
    MeshCollider mMeshCollider; /* 0x91c */
    u8  unk_96c;            /* 0x96c */
    u8  pad_96d[0x1f];
    u8  unk_98c;            /* 0x98c */
    u8  pad_98d[0x7];
    u8  unk_994;            /* 0x994 */
    u8  pad_995[0x1f];
    u8  unk_9b4;            /* 0x9b4 */
    u8  unk_9b5;            /* 0x9b5 */
    u8  unk_9b6;            /* 0x9b6 */
    u8  pad_9b7[0x1];
    u8  unk_9b8;            /* 0x9b8 */
    u8  pad_9b9[0x3];
    /* 0x9bc. Typed, not guessed, and evidenced from both ends: LoadSkybox
       `new`s 0x50 bytes, runs Model::Model on them and stores the result here;
       CleanupResources loads it back and destroys it through its vtable. The
       generated header called it `u8 unk_9bc`, so both files that use it had to
       cast a pointer out of a byte to reach it. */
    Model *skyboxModel;     /* 0x9bc */

    /* Declared first, deliberately -- see KEY FUNCTION above. */
    virtual ~Stage();

    /* --- overrides, in _ZTV5Stage order --- */
    virtual s32  InitResources();                      /* slot  0 */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual s32  CleanupResources();                   /* slot  3 */
    virtual s32  Behavior();                           /* slot  6 */
    virtual s32  Render();                             /* slot  9 */
    virtual void OnPendingDestroy();                   /* slot 12 */

    /* --- non-virtual, and take `this` --- */
    void LoadFog();
    void LoadSkybox();
    void LoadModel();
    void LoadTextureTransformers();
    void RenderFog();
    void RenderModel();
    void RenderModelTransparent();

    /* Reached from LoadSkybox with r0 still holding `this` and no argument set
       up, which is a member call. Its body reads the level record through a
       global rather than through a field, so the bytes alone cannot confirm the
       `this`; the call site is what decides it. */
    int GetSkyboxID();

    /* --- static: reached with no object. The pause-screen and menu group all
           take their first declared argument in r0. --- */
    static void CheckInput();
    static void LoadClsnAndObjects(LVL_Overlay &ovl, u32 param, MeshCollider &mc);
    static void RenderVsModeNewStar();
    static void RenderVsModeCountdown();
    static void RenderBouncingArrows();
    static void VE_Init();
    static void VE_Update();
    static void LC_Render();
    /* PS_Init is deliberately NOT declared here, even though it is now real
       C++: src/_ZN5Stage7PS_InitEv.cpp defines Stage::PS_Init() and is
       byte-verified and enrolled. It uses its own LOCAL shadow `class Stage`
       (the same pattern src/_ZN5Stage8BehaviorEv.cpp already used) instead of
       including this header, on purpose -- a once-real landmine lived at this
       exact filename: an untracked, un-enrolled second .cpp for this symbol
       whose private `struct G2x` declared SetBlendBrightness's middle
       parameter as `int`, mangling to _ZN3G2x18SetBlendBrightnessEPVtis,
       which resolves to nothing and would silently NOT be what byte-matches.
       The surviving file never declares a G2x type at all -- it calls the
       correctly-mangled _ZN3G2x18SetBlendBrightnessEPVtts symbol directly, as
       the .c file it replaced did -- so the landmine cannot recur through it.
       Wiring PS_Init to this header instead is a separate, low-risk change
       that does not need to happen in the same slice that fixed D0/D1. */
    static void PS_Cleanup();
    static void PS_UpdateSaveMenu(bool held);
    static void UpdateMenuButtons(bool held);
    static void SetVramBanks();
    static void ResetMeshColliders();
    static int  IsPauseDisabled();
    static int  CanPause();
    static int  GraphCallback1();
    static int  GraphCallback2(SceneRelated *scene);
};

#endif
