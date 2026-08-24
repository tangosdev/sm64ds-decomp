#ifndef STAGE_H
#define STAGE_H

#include "dScene_c.h"
#include "Model.h"
#include "dBgW_Kc.h"
#include "Fog.h"

struct SceneRelated;
struct LVL_Overlay;

/* Particle::SysTracker, embedded at Stage+0x50. Declared locally rather than
 * pulled from include/Particle.h / include/Particle__SysTracker.h, which are
 * two separate generated shadows of this same class; merging them is its own
 * change with its own blast radius. See notes/scene-provenance.md.
 *
 * The destructor is declared, never defined -- src/_ZN8Particle10SysTrackerD1Ev.cpp
 * supplies it as an extern "C" free function, and this declaration only lets
 * Stage's implicit destructor find it by name. NOT virtual: the class has no
 * RTTI record and no _ZTV in the ROM, so it must not gain a vtable pointer. */
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
   and it is also exactly the gap Stage's own D1/D0 give this member. */
typedef char SysTracker_size_must_be_0x81c[sizeof(SysTracker) == 0x81c ? 1 : -1];
}

/* The playable level: fBase_c -> dBase_c -> dScene_c -> Stage (dScStage_c in
 * the ROM's own type graph). A leaf; it adds no virtual of its own and
 * overrides six of dScene_c's, plus the destructor pair.
 *
 * The destructor is declared first, deliberately: that makes ~Stage the key
 * function rather than InitResources. Derivation, vtable census and the field
 * evidence: notes/scene-provenance.md.
 *
 * Field NAMES cannot change codegen. Offsets and widths are observed, and
 * tools/check_header_offsets.py walks them from dScene_c's asserted 0x50.
 */
struct Stage : dScene_c {
    Particle::SysTracker mSysTracker;  /* 0x050 */
    Model mModel;             /* 0x86c */
    u8  pad_8bc[0x60];       /* Model ends 0x8bc; dBgW_Kc does not start until 0x91c */
    dBgW_Kc mMeshCollider; /* 0x91c */
    Fog unk_96c;       /* 0x96c */
    u8  pad_992[0x2];
    u8  unk_994;            /* 0x994 */
    u8  pad_995[0x1f];
    u8  unk_9b4;            /* 0x9b4 */
    u8  unk_9b5;            /* 0x9b5 */
    u8  unk_9b6;            /* 0x9b6 */
    u8  pad_9b7[0x1];
    u8  unk_9b8;            /* 0x9b8 */
    u8  pad_9b9[0x3];
    /* Allocated by LoadSkybox, destroyed through its vtable by CleanupResources. */
    Model *skyboxModel;     /* 0x9bc */

    /* Declared first, deliberately: makes ~Stage the key function. */
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

    int GetSkyboxID();

    /* --- static: reached with no object. The pause-screen and menu group all
           take their first declared argument in r0. --- */
    static void CheckInput();
    static void LoadClsnAndObjects(LVL_Overlay &ovl, u32 param, dBgW_Kc &mc);
    static void RenderVsModeNewStar();
    static void RenderVsModeCountdown();
    static void RenderBouncingArrows();
    static void VE_Init();
    static void VE_Update();
    static void LC_Render();
    /* PS_Init is deliberately NOT declared here; it uses its own local shadow
       of Stage instead. See notes/scene-provenance.md before wiring it up. */
    static void PS_Cleanup();
    static void PS_UpdateSaveMenu(bool held);
    static void UpdateMenuButtons(bool held);
    static void SetVramBanks();
    static void ResetMeshColliders();
    static int  IsPauseDisabled();
    static int  CanPause();
    /* Trailing extent the ROM's `new Stage` size literal proves; see
       tools/opnew_sizes.py. */
    u8 pad_9c0[0x8];

    /* Scene-graph hooks: vtable slots 1/2 of dScStage_c::graphCallback_c.
       Non-virtual on purpose -- declaring them virtual would make this TU emit
       a vtable the delink ranges do not own. */
    int  GraphCallback1();
    int  GraphCallback2();
};

typedef char Stage_size_must_be_0x9c8[sizeof(struct Stage) == 0x9c8 ? 1 : -1];

#endif
