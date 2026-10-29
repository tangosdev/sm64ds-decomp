#ifndef DSCSTAGE_C_H
#define DSCSTAGE_C_H

#include "dScene_c.h"
#include "dExtSimpleModel_c.h"
#include "dBgW_Kc.h"
#include "Fog.h"

struct SceneRelated;
struct LVL_Overlay;
struct dExtAnmTexSRT_c;

/* Particle::SysTracker, embedded at dScStage_c+0x50. The one definition lives in
 * include/Particle__SysTracker.h; see notes/scene-provenance.md and
 * notes/system-provenance.md for the field evidence. */
#include "Particle__SysTracker.h"

/* One per level texture animation, eight slots' worth at dScStage_c+0x8bc.
 * dScStage_c::LoadTextureTransformers fills them: for each entry of the level's
 * animation table (data_0209f340) that carries a BTA file it news a 0x14-byte
 * dExtAnmTexSRT_c, constructs it, and stores it in the slot, striding 0xc.
 * dScStage_c::Render advances the transformer when the flag at +0x04 is set;
 * dScStage_c::RenderModelTransparent reads the same flag to decide whether that
 * part's materials get the transparent bit; dScStage_c::CleanupResources destroys
 * the transformer through its vtable and then walks the list at +0x08, whose
 * next pointer is at +0x0c, freeing every block. */
struct StageTexAnimSlot {
    struct dExtAnmTexSRT_c *mTransformer; /* 0x00 - heap */
    u8 mActive;         /* 0x04 */
    u8 pad_05[0x3];
    void *mBlockList; /* 0x08 - singly linked, next pointer at +0x0c */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char StageTexAnimSlot_size_must_be_0xc[sizeof(struct StageTexAnimSlot) == 0xc ? 1 : -1];
#endif

/* The level loader's minimap-change sentinel: link a record into area idx's
 * height-sorted mBlockList so that above the given height the minimap switches
 * to the given id (0xf for none). The height is a plain Fix12i; a by-value
 * Fix12<int> would cost the caller a literal-pool reload
 * (notes/mwccarm-codegen.md 6az). The id is signed: LoadSimpleObjects
 * sign-extends it into r2 (lsl/asr #24), where an unsigned id would mask. */
void LoadMinimapChangeObject(int, Fix12i, s8);

/* The playable level: fBase_c -> dBase_c -> dScene_c -> dScStage_c (dScStage_c in
 * the ROM's own type graph). A leaf; it adds no virtual of its own and
 * overrides six of dScene_c's, plus the destructor pair.
 *
 * The destructor is declared first, deliberately: that makes ~dScStage_c the key
 * function rather than InitResources. Derivation, vtable census and the field
 * evidence: notes/scene-provenance.md.
 *
 * Field NAMES cannot change codegen. Offsets and widths are observed, and
 * tools/check_header_offsets.py walks them from dScene_c's asserted 0x50.
 */
struct dScStage_c : dScene_c {
    Particle::SysTracker mSysTracker;  /* 0x050 */
    dExtSimpleModel_c mModel;          /* 0x86c */
    StageTexAnimSlot mTexAnimSlots[8]; /* 0x8bc - dExtSimpleModel_c ends here, and dBgW_Kc
                                           does not start until 0x91c */
    dBgW_Kc mMeshCollider;             /* 0x91c */
    /* Two fog setups, 0x28 apart: LoadFog fills both by hand and then walks
       this array with `dst += 0x28` per level fog record. */
    Fog mFog[2]; /* 0x96c */
    /* Allocated by LoadSkybox, destroyed through its vtable by CleanupResources. */
    dExtSimpleModel_c *mSkyboxModel; /* 0x9bc */

    /* Declared first, deliberately: makes ~dScStage_c the key function. */
    virtual ~dScStage_c();

    /* --- overrides, in _ZTV10dScStage_c order --- */
    virtual s32 InitResources();        /* slot  0 */
    virtual bool BeforeInitResources(); /* slot  1 */
    virtual s32 CleanupResources();     /* slot  3 */
    virtual s32 Behavior();             /* slot  6 */
    virtual s32 Render();               /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    /* --- non-virtual, and take `this` --- */
    void LoadFog();
    void LoadSkybox();
    void LoadModel();
    void LoadTextureTransformers();
    void RenderFog();
    void RenderModel();
    void RenderModelTransparent();

    int GetSkyboxID();
    /* Member, not static: Behavior hands it `this` in r0 (`mov r0,r4`) even
       though the body never reads it. */
    void CheckInput();

    /* --- static: reached with no object. The pause-screen and menu group all
           take their first declared argument in r0. --- */
    static void LoadClsnAndObjects(LVL_Overlay &ovl, u32 param, dBgW_Kc &mc);
    static void RenderVsModeNewStar();
    static void RenderVsModeCountdown();
    static void RenderBouncingArrows();
    static void VE_Init();
    static void VE_Update();
    static void LC_Render();
    static void LC_Update();
    static void CheckCameraInput();
    static void PS_Init();
    static void PS_Render();
    static void PS_Cleanup();
    static void PS_UpdateSaveMenu(bool held);
    static void PS_UpdateOptionsMenu();
    static void PS_UpdateOkAndBackButtons(bool held);
    static void UpdateMenuButtons(bool held);
    static void SetVramBanks();
    static void ResetMeshColliders();
    static void RenderNumber(u8 h, int i, int i2, bool b, int i3);
    static void LoadGraphics2D(bool e3d, int level);
    static void PS_Update();
    static int IsPauseDisabled();
    static int CanPause();
    static void UpdateMessage();
    /* Trailing extent the ROM's `new dScStage_c` size literal proves; see
       tools/opnew_sizes.py. */
    u8 pad_9c0[0x4];
    /* Latches the two-phase level load. InitResources runs its whole first
       block only while this is zero, sets it when data_0209fc68 says a wait is
       needed, and from then on returns -1 -- "call me again" -- until
       func_020308a8 reports the load finished. */
    s32 mWaitingForLoad; /* 0x9c4 */

    /* Scene-graph hooks: vtable slots 1/2 of dScStage_c::graphCallback_c.
       Non-virtual on purpose -- declaring them virtual would make this TU emit
       a vtable the delink ranges do not own. */
    int GraphCallback1();
    int GraphCallback2();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Stage_size_must_be_0x9c8[sizeof(struct dScStage_c) == 0x9c8 ? 1 : -1];
#endif

#endif
