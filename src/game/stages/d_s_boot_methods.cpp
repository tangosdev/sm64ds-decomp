//cpp
/* leftovers:
 * - Source order stays highest address first.
 * - UpdateButtonPalettes (0x02005348) is a coined name. It is a
 *   dScBoot_c method: Behavior is its only caller, and the body reads
 *   mSelectedButton and mButtonFlashTimer.
 * - opt_common_subs off stays file-global. Behavior's countdown
 *   updates need it: without it the compiler reuses each timer's
 *   field address and Behavior drops an instruction.
 * - The destructor pair and dScBoot_c_classInit live in src/game/stages/d_s_boot.cpp.
 *
 * dScBoot_c, the title screen. fBase_c, dBase_c, dScene_c, then this
 * leaf. .text 0x02005324..0x02005d94 is the button redraw, Behavior
 * and InitResources.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScBoot_c_AssetId { ASSET_D_2D_FRAMEWORK_INITIALIZE_NSC_BIN = 0x022d };

#include "dScBoot_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "AsyncJob.h"

#include "dCamera_c.h"
#include "Sound.h"
#include "Message.h"
#include "SaveData.h"
#include "dFdColor_c.h"
#include "decl_common.h"
#include "MessageBank.h"
#include "PlayerInput.h"
#include "private/controller_input.h"
#include "nitro/pad/pad.h"

#pragma opt_common_subs off

extern dFdBrightness_c *gSceneFader;
/* Whether the boot job InitResources starts has finished. d_s_boot's own
   .bss; only InitResources and Behavior read it. */
extern u8 sBootJobDone;
extern PadData data_020a0e58[];
extern int data_0208ee44;
extern u8 gSubScreenLayers;
extern u8 gMainScreenLayers;
extern dFdColor_c gSceneColorFader;
extern "C" {

u32 LoadCompressedFileAt(int fileID, void *target);
void *LoadFile(int handle);

void DecompressLZ16(const void *src, void *dst);
}

/* These names describe the observed transitions; original state names are unknown. */
enum BootMenuState {
    STATE_BOOT_WAIT = 0,
    STATE_CONFIRM_ERASE = 1,
    STATE_OPEN_FINAL_CONFIRM = 2,
    STATE_FINAL_CONFIRM = 3,
    STATE_BEGIN_ERASE = 4,
    STATE_ERASING = 5,
    STATE_CANCEL_ERASE = 6,
    STATE_POST_ERASE_WAIT = 7
};

/* dScBoot_c::InitResources -- vtable slot 0, arm9 0x02005a58.
 *
 * The very first thing the game draws. It hands the four VRAM banks to
 * BG/OBJ on both screens, brings up a text-mode main BG and three sub BGs,
 * decompresses the same 0x020918c4 tile set into both screens' character
 * memory with the 0x020914e0 palette, starts the boot countdown at 0x3c
 * frames, and starts the background job func_0201a244(func_0201a2f8, ...).
 * Its return value, 1 when the job already ran synchronously, seeds
 * sBootJobDone; Behavior polls func_0201a1bc until the job has finished and
 * then loads the message text.
 *
 * The register writes use include/nitro/hw/registers.h's names (DISPCNT,
 * POWCNT1 and the BGnCNT of both engines); the read-modify-write masks are
 * the ROM's own.
 */
s32 dScBoot_c::InitResources() {
    GX::DisableAllBanks();
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    GX_SetBGScrOffset(1);
    GX_SetPower(GX_POWER_ALL);
    GX_DispOn();

    GXS_DispOn();
    G2_SetBG0Priority(1);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb800,
                     GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);
    G2_BG0Mosaic(0);
    SetBg0Offset(0, 0);

    DecompressLZ16(&data_020918c4, G2_GetBG0CharPtr());
    GX_LoadBGPltt(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_020916d8, G2_GetBG0ScrPtr());

    G2_SetBG3Priority(0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                         GX_BG_CHARBASE_0x00000);
    G2_BG3Mosaic(0);
    G2S_SetBG0Priority(0);
    G2S_BG0Mosaic(0);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01);
    SetSubBg0Offset(0, 0);

    G2S_SetBG1Priority(0);
    G2S_BG1Mosaic(0);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0800,
                      GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01);
    SetSubBg1Offset(0, 0);

    G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1800,
                          GX_BG_CHARBASE_0x10000);
    SetSubBg2Offset(0, 0);

    DecompressLZ16(&data_020918c4, G2S_GetBG2CharPtr());
    GXS_LoadBGPltt(&data_020914e0, 0, 2);
    GXS_LoadBGPltt(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_02091570, G2S_GetBG2ScrPtr());

    gMainScreenLayers = DISPLAY_LAYER_BG0;
    gSubScreenLayers = 4;
    GX_SetVisiblePlane(DISPLAY_LAYER_BG0);
    GXS_SetVisiblePlane(DISPLAY_LAYER_BG2);

    mFadeTimer = 0x3c;
    mState = STATE_BOOT_WAIT;
    mButtonFlashTimer = 0;
    mInputLockTimer = 0;
    data_0208ee44 = 1;
    func_020233f4();

    gSceneColorFader.color = 0x7fff;
    Sound::Play2D(4, 0);
    sBootJobDone = func_0201a244(func_0201a2f8, 0, 0xf, 0, 0x1000);
    return 1;
}

/* dScBoot_c::Behavior -- vtable slot 6.
 *
 * The boot menu's state machine, in mState: wait out the fade (0/7), watch
 * the touch screen for the two confirmation buttons (1/3), confirm (2/6),
 * and run the "erase all save data" countdown (4/5). data_0208ee44 is the
 * frame step, so every countdown decrements by it rather than by one.
 */
s32 dScBoot_c::Behavior() {
    u16 keysHeld, keysPressed;
    int frameStep;

    frameStep = data_0208ee44;
    keysHeld = data_020a0e58[gActivePlayerSlot].held;
    keysPressed = data_020a0e58[gActivePlayerSlot].pressed;

    if (sBootJobDone == 0) {
        sBootJobDone = func_0201a1bc();
        if (sBootJobDone == 0) {
            if (mFadeTimer > frameStep * 2) {
                mFadeTimer -= frameStep;
            }
            return 1;
        }
        Message::LoadText();
    }

    if (gSceneFader->IsAtStart() != 0) {
        if (mButtonFlashTimer != 0) {
            mButtonFlashTimer -= frameStep;
            if (mButtonFlashTimer == 0) {
                UpdateButtonPalettes();
            }
        }
        if (mInputLockTimer != 0) {
            mInputLockTimer -= frameStep;
            return 1;
        }

        switch (mState) {
        case STATE_BOOT_WAIT:
        case STATE_POST_ERASE_WAIT:
            if (mFadeTimer != 0) {
                mFadeTimer -= frameStep;
                if (mFadeTimer == 0) {
                    StartSceneFade(func_0203da3c() != 0 ? 6 : 1, 0, 0);
                } else if (mState == STATE_BOOT_WAIT &&
                           keysHeld == (PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_Y |
                                        PAD_BUTTON_L | PAD_BUTTON_R)) {
                    u16 langFileID;
                    void *palette;

                    if (GetOwnerLanguage() == 5) {
                        langFileID = 0xb00d;
                    } else if (GetOwnerLanguage() == 4) {
                        langFileID = 0xac0d;
                    } else if (GetOwnerLanguage() == 3) {
                        langFileID = 0xa80d;
                    } else if (GetOwnerLanguage() == 2) {
                        langFileID = 0xa40d;
                    } else {
                        langFileID = 0xa00d;
                    }
                    LoadCompressedFileAt(langFileID, G2S_GetBG1CharPtr());
                    palette = LoadFile(0x9807);
                    GXS_LoadBGPltt(palette, 0, 0x1c0);
                    Deallocate(palette);
                    LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_INITIALIZE_NSC_BIN,
                                         G2S_GetBG1ScrPtr());
                    LoadCompressedFileAt(0x9803, (char *)G2S_GetBG1ScrPtr() + 0x800);
                    func_0201cd08(0x29a);
                    mSelectedButton = 1;
                    UpdateButtonPalettes();
                    SetSubBg0Offset(0, 0);
                    SetSubBg1Offset(0, 0);
                    gSubScreenLayers |= 3;
                    gSubScreenLayers &= ~4;
                    mState = STATE_CONFIRM_ERASE;
                }
            }
            break;

        case STATE_CONFIRM_ERASE:
        case STATE_FINAL_CONFIRM: {
            int playerSlot = gActivePlayerSlot;
            u8 touchDown;

            int touchStarted = 0;
            touchDown = gTouchHeld[playerSlot * 4];
            if (touchDown != 0 && gTouchEdge[playerSlot * 4] != 0) {
                touchStarted = 1;
            }
            if (touchStarted != 0 ||
                (keysPressed & (PAD_BUTTON_A | PAD_BUTTON_START | PAD_KEY_LEFT | PAD_KEY_RIGHT))) {
                int leftTouchStarted;

                if (touchDown != 0 && gTouchEdge[playerSlot * 4] != 0) {
                    leftTouchStarted = 1;
                } else {
                    leftTouchStarted = 0;
                }
                /* left button: touch x 0x28..0x78, y 0x98..0xb8, or Left */
                if ((leftTouchStarted != 0 && (u8)(gTouchX[playerSlot * 4] - 0x28) < 0x50 &&
                     (u8)(gTouchY[playerSlot * 4] - 0x98) < 0x20) ||
                    (keysPressed & PAD_KEY_LEFT)) {
                    if (mSelectedButton == 0) {
                        mButtonFlashTimer = 0x10;
                    }
                    mSelectedButton = 0;
                    UpdateButtonPalettes();
                    func_02012790(0);
                    if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0x50 &&
                        (u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20) {
                        mInputLockTimer = 0x20;
                        if (mState == STATE_CONFIRM_ERASE) {
                            mState = STATE_OPEN_FINAL_CONFIRM;
                        } else {
                            mState = STATE_BEGIN_ERASE;
                        }
                    }
                } else {
                    int rightTouchStarted;

                    if (touchDown != 0 && gTouchEdge[playerSlot * 4] != 0) {
                        rightTouchStarted = 1;
                    } else {
                        rightTouchStarted = 0;
                    }
                    /* right button: touch x 0x88..0xd8, same rows, or Right */
                    if ((rightTouchStarted != 0 && (u8)(gTouchX[playerSlot * 4] - 0x88) < 0x50 &&
                         (u8)(gTouchY[playerSlot * 4] - 0x98) < 0x20) ||
                        (keysPressed & PAD_KEY_RIGHT)) {
                        if (mSelectedButton == 1) {
                            mButtonFlashTimer = 0x10;
                        }
                        mSelectedButton = 1;
                        UpdateButtonPalettes();
                        func_02012790(0);
                        if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x88) < 0x50) {
                            if ((u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20) {
                                mInputLockTimer = 0x20;
                                mState = STATE_CANCEL_ERASE;
                            }
                        }
                    } else if (keysPressed & (PAD_BUTTON_A | PAD_BUTTON_START)) {
                        mButtonFlashTimer = 0x10;
                        mInputLockTimer = 0x20;
                        UpdateButtonPalettes();
                        func_02012790(0);
                        if (mSelectedButton == 0) {
                            if (mState == STATE_CONFIRM_ERASE) {
                                mState = STATE_OPEN_FINAL_CONFIRM;
                            } else {
                                mState = STATE_BEGIN_ERASE;
                            }
                        } else {
                            mState = STATE_CANCEL_ERASE;
                        }
                    }
                }
            }
            break;
        }

        case STATE_OPEN_FINAL_CONFIRM:
            func_0201cd08(0x29b);
            mSelectedButton = 1;
            UpdateButtonPalettes();
            mState = STATE_FINAL_CONFIRM;
            break;

        case STATE_BEGIN_ERASE:
            G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                              GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01);
            Message::DisplaySaveStatusText(0x29c);
            mEraseEffectTimer = 0x78;
            mState = STATE_ERASING;
            break;

        case STATE_ERASING:
            if (mEraseEffectTimer == 0x3c) {
                SaveData::EraseAllSaveData();
            }
            if (mEraseEffectTimer != 0) {
                u8 eraseFramesLeft;

                mEraseEffectTimer -= frameStep;
                eraseFramesLeft = mEraseEffectTimer;
                if (eraseFramesLeft == 0x3c) {
                    Message::DisplaySaveStatusText(0x29d);
                } else if (eraseFramesLeft == 0) {
                    gSubScreenLayers &= ~3;
                    gSubScreenLayers |= 4;
                    mFadeTimer = 0x3c;
                    mState = STATE_POST_ERASE_WAIT;
                }
            }
            break;

        case STATE_CANCEL_ERASE:
            gSubScreenLayers &= ~3;
            gSubScreenLayers |= 4;
            mFadeTimer = 0x3c;
            mState = STATE_BOOT_WAIT;
            break;
        }
    }
    return 1;
}

/* Repaints two 16-by-4-tile buttons on the sub screen's BG1 map, from map
 * row 19 (byte 0x4c0). They sit side by side in the 32-tile-wide map. The
 * selected button uses palette bank 2 when the flash timer is zero; all
 * other tiles use bank 1. */
void dScBoot_c::UpdateButtonPalettes() {
    int button;
    for (button = 0; button < 2; button++) {
        u16(*rows)[0x20];
        int column;
        u16 paletteBits = (button == mSelectedButton && mButtonFlashTimer == 0) ? 0x2000 : 0x1000;
        rows = (u16(*)[0x20])((u16 *)G2S_GetBG1ScrPtr() + 19 * 0x20 + button * 0x10);
        for (column = 0; column < 0x10; column++) {
            rows[0][column] = paletteBits + (rows[0][column] & 0x3ff);
            rows[1][column] = paletteBits + (rows[1][column] & 0x3ff);
            rows[2][column] = paletteBits + (rows[2][column] & 0x3ff);
            rows[3][column] = paletteBits + (rows[3][column] & 0x3ff);
        }
    }
}

/* Absorbed from the retired arm9 helper func_02005324
 * (arm9 0x02005324..0x02005348): pre-talk camera backup plus event-flag
 * setup, returns 1. Defined last because this file emits .text in reverse
 * source order, so the lead body lands at the run's left edge. The camera
 * calls are spelled as the members they are (dCamera_c.h). */
int func_02005324(void *p) {
    ((dCamera_c *)p)->SaveCameraStateBeforeTalk();
    ((dCamera_c *)p)->SetMode(0x19);
    return 1;
}
