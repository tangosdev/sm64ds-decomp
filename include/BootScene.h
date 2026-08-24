#ifndef BOOTSCENE_H
#define BOOTSCENE_H

#include "dScene_c.h"

/* The boot/intro scene: fBase_c -> dBase_c -> dScene_c -> BootScene
 * (dScBoot_c in the ROM's own type graph; the coinage comes from its
 * destructor symbols). A leaf -- nothing derives from it.
 *
 * It draws the title/language screen and runs the two-button menu that can
 * erase all save data. Derivation, vtable and layout evidence:
 * notes/scene-provenance.md.
 *
 * Field NAMES cannot change codegen. Offsets and widths are observed.
 */
struct BootScene : dScene_c {
    u16 mFadeTimer;         /* 0x050 -- frames left before the scene fades out */
    u8  mState;             /* 0x052 -- menu state machine; see Behavior */
    u8  mSelectedButton;    /* 0x053 -- 0 = language, 1 = erase */
    u8  mButtonFlashTimer;  /* 0x054 -- suppresses the highlight while it runs */
    u8  mInputLockTimer;    /* 0x055 -- swallows input after a press */
    u8  mEraseEffectTimer;  /* 0x056 -- "erase all save data" countdown */
    u8  pad_057[0x1];

    /* Declared first, deliberately: that makes ~BootScene the key function,
       and it is only ever defined out of line in _ZN9BootSceneD1Ev.cpp /
       _ZN9BootSceneD0Ev.cpp, so no TU here emits a vtable group. */
    virtual ~BootScene();

    virtual s32 InitResources();          /* slot 0 */
    virtual s32 Behavior();               /* slot 6 */
};

/* Holds the chain to the size func_02023624.c's operator new(0x58) evidences.
   A silently-added member anywhere fails this. */
typedef char BootScene_size_must_be_0x58[sizeof(BootScene) == 0x58 ? 1 : -1];

#endif
