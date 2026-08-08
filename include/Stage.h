#ifndef STAGE_H
#define STAGE_H

#include "Scene.h"

struct Model;
struct SceneRelated;
struct LVL_Overlay;
struct MeshCollider;

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
 * role to ~Stage, which is only ever defined as an extern "C" free function in
 * _ZN5StageD0Ev.c and _ZN5StageD1Ev.c.
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
    u8  unk_050;            /* 0x050 */
    u8  pad_051[0x81b];
    u8  unk_86c;            /* 0x86c */
    u8  pad_86d[0x7];
    u8  unk_874;            /* 0x874 */
    u8  pad_875[0x47];
    u8  unk_8bc;            /* 0x8bc */
    u8  pad_8bd[0x5f];
    u8  unk_91c;            /* 0x91c */
    u8  pad_91d[0x4f];
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
    /* PS_Init is deliberately NOT declared here. src/_ZN5Stage7PS_InitEv.c
       still hand-spells it, because the tree contains a SECOND file for the same
       symbol -- src/_ZN5Stage7PS_InitEv.cpp -- which delinks.txt does not name
       and nothing has ever compiled. That .cpp is not simply the migrated
       version waiting to be enrolled: it only reproduces the bytes because its
       private `struct G2x` declares SetBlendBrightness's middle parameter as
       `int`, which mangles to _ZN3G2x18SetBlendBrightnessEPVtis and resolves to
       nothing. Correct the type to match the real symbol (...EPVtts) and the
       function stops matching. Deciding which of the two is right needs its own
       change; see the PR. */
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
