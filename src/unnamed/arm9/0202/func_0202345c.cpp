//cpp
#include "types.h"
#include "dFader_c.h"

/* The soft reset: holding L+R+START+SELECT (pad bits 0x30c, which
   func_0203bc7c turns into data_020a0e44) asks the running scene to fade out
   and restart. dScene_c::BeforeBehavior runs the fade out with
   gSoftResetFader and then destroys the scene. */

extern unsigned char data_0209b2fc;
extern unsigned char gMgSaveDirty;
extern void func_0202fd2c(void);
extern void Wipe_Reset(void);
extern void func_02011c8c(void);
extern "C" {

extern int GetSoundMode(void);
extern void SetSoundMode(int mode);
extern void func_0203bc50(void);
extern void func_0203bb14(void);
}

extern void ResetInput(void);
extern unsigned char data_020a0e44;

/* The pending request. dScene_c::BeforeBehavior and
   dScDSMT_c::InitResources read it. */
u8 gSoftResetPending;

/* Keeps a held combo from firing again until it is let go. */
static u8 sSoftResetComboHeld;

/* Turns the combo off. func_02030aa4 raises it for a link session and
   func_02030a38 drops it; the minigame scene raises it while gMgSaveDirty is
   1 and drops it once the records are saved. The lid-close handler
   func_02019ac4 reads it too: while it is set, closing the lid only turns
   the screens off. */
u8 gSoftResetDisabled;

/* The fader running the reset's fade out. dScene_c::BeforeBehavior stores
   &gSoftResetBrightnessFader (a dFdBrightness_c) here, dScene_c::ResetFadersAndSound
   clears it. */
dFader_c *gSoftResetFader;

/* Clear the soft reset request, with data_0209b2fc and gMgSaveDirty, and
   re-apply the current sound mode. dScene_c::BeforeBehavior calls it when
   the request lands on a scene whose actorID is 0; dScDSMT_c::InitResources
   calls it too. */
void func_02023544(void) {
    gSoftResetPending = 0;
    func_0202fd2c();
    data_0209b2fc = 0;
    gMgSaveDirty = 0;
    Wipe_Reset();
    func_02011c8c();
    SetSoundMode(GetSoundMode());
}

/* Once a frame (func_020197b8, after the pads are read): while a reset is
   pending, clear every player's buttons and touch (func_0203bc50,
   func_0203bb14, ResetInput); otherwise raise one on a fresh combo. */
void func_02023498(void) {
    if (gSoftResetPending != 0) {
        func_0203bc50();
        func_0203bb14();
        ResetInput();
        return;
    }
    if (sSoftResetComboHeld != 0) {
        if (data_020a0e44 == 0)
            sSoftResetComboHeld = 0;
        return;
    }
    if (gSoftResetDisabled != 0)
        return;
    if (data_020a0e44 != 0) {
        gSoftResetPending = 1;
        sSoftResetComboHeld = 1;
    }
}

/* Step the soft reset's fader, if one is running (func_02019390). */
void func_0202345c(void) {
    dFader_c *fader = gSoftResetFader;
    if (!fader)
        return;
    fader->AdvanceFade();
}
