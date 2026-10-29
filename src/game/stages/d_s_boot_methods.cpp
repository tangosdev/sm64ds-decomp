//cpp
/* leftovers:
 * - Source order stays highest address first.
 * - func_02005348 stays that address. It is a dScBoot_c method:
 *   Behavior is its only caller, and the body reads mSelectedButton
 *   and mButtonFlashTimer. symbols.txt has the mangled spelling.
 * - opt_common_subs off stays file-global. Behavior's countdown
 *   updates need it.
 * - The volatile reads and LADR() writes in Behavior stay. A plain
 *   member read lets the compiler reuse the field address and drops
 *   an instruction.
 * - The destructor pair and dScBoot_c_classInit live in src/game/stages/d_s_boot.cpp.
 *
 * dScBoot_c, the title screen. fBase_c, dBase_c, dScene_c, then this
 * leaf. .text 0x02005324..0x02005d94 is the button redraw, Behavior
 * and InitResources.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScBoot_c_AssetId { ASSET_D_2D_FRAMEWORK_INITIALIZE_NSC_BIN = 0x022d };

#include "dScBoot_c.h"
#include "G2.h"
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

#pragma opt_common_subs off

extern "C" {
extern dFdBrightness_c *data_0209f5bc;
extern u8 data_0209f1e8;
extern u16 data_020a0e58[];
extern u16 data_020a0e5a[];
extern int data_0208ee44;
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern dFdColor_c data_0209f5e8;

u32 LoadCompressedFileAt(u16 fileID, void *target);
int LoadFile(int handle);


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

#define LADR(p) ((void *)(unsigned int)(p))

// @symbol _ZN9dScBoot_c13InitResourcesEv
/* dScBoot_c::InitResources -- vtable slot 0, arm9 0x02005a58.
 *
 * The very first thing the game draws. It hands the four VRAM banks to
 * BG/OBJ on both screens, brings up a text-mode main BG and three sub BGs,
 * decompresses the same 0x020918c4 tile set into both screens' character
 * memory with the 0x020914e0 palette, starts the boot countdown at 0x3c
 * frames, and starts the job func_0201a244(func_0201a2f8, ...) whose handle
 * lands in data_0209f1e8 (Behavior polls it).
 *
 * The register writes stay as literal volatile stores to the ARM7/9 I/O
 * block: this tree has no register header, and the read-modify-write masks
 * are the ROM's own.
 */
s32 dScBoot_c::InitResources() {
    GX::DisableAllBanks();
    GX::SetBankForBG(1);
    GX::SetBankForOBJ(2);
    GX::SetBankForSubBG(4);
    GX::SetBankForSubOBJ(8);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    GX::SetGraphicsMode(1, 0, 0);
    GXS::SetGraphicsMode(0);

    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x38000000) | 0x8000000;
    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & 0xfffffdf1) | 0x20e;
    GX::DispOn();

    *(volatile u32 *)0x4001000 |= 0x10000;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x1710;
    *(volatile u16 *)0x4000008 = *(volatile u16 *)0x4000008 & ~0x40;
    SetBg0Offset(0, 0);

    DecompressLZ16(&data_020918c4, func_02054efc());
    GX::LoadBGPltt(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_020916d8, G2::GetBG0ScrPtr());

    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1100;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;
    *(volatile u16 *)0x4001008 = *(volatile u16 *)0x4001008 & ~3;
    *(volatile u16 *)0x4001008 = *(volatile u16 *)0x4001008 & ~0x40;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x18;
    SetSubBg0Offset(0, 0);

    *(volatile u16 *)0x400100a = *(volatile u16 *)0x400100a & ~3;
    *(volatile u16 *)0x400100a = *(volatile u16 *)0x400100a & ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0x118;
    SetSubBg1Offset(0, 0);

    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~3;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x310;
    SetSubBg2Offset(0, 0);

    DecompressLZ16(&data_020918c4, (void *)G2S::GetBG2CharPtr());
    GXS::LoadBGPltt(&data_020914e0, 0, 2);
    GXS::LoadBGPltt(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_02091570, (void *)G2S::GetBG2ScrPtr());

    data_0209d45c = 1;
    data_0209d454 = 4;
    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | 0x100;
    *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x400;

    mFadeTimer = 0x3c;
    mState = STATE_BOOT_WAIT;
    mButtonFlashTimer = 0;
    mInputLockTimer = 0;
    data_0208ee44 = 1;
    func_020233f4();

    data_0209f5e8.color = 0x7fff;
    Sound::Play2D(4, 0);
    data_0209f1e8 = (u8)func_0201a244(func_0201a2f8, 0, 0xf, 0, 0x1000);
    return 1;
}

// @symbol _ZN9dScBoot_c8BehaviorEv
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
    keysHeld = *(u16 *)((char *)data_020a0e58 + gActivePlayerSlot * 4);
    keysPressed = *(u16 *)((char *)data_020a0e5a + gActivePlayerSlot * 4);

    if (data_0209f1e8 == 0) {
        data_0209f1e8 = (u8)func_0201a1bc();
        if (data_0209f1e8 == 0) {
            if ((int)*(volatile u16 *)&mFadeTimer > (int)(frameStep * 2)) {
                *(u16 *)LADR(&mFadeTimer) -= frameStep;
            }
            return 1;
        }
        Message::LoadText();
    }

    if (data_0209f5bc->IsAtStart() != 0) {
        int state;

        if (*(volatile u8 *)&mButtonFlashTimer != 0) {
            *(u8 *)LADR(&mButtonFlashTimer) -= frameStep;
            if (mButtonFlashTimer == 0) {
                func_02005348();
            }
        }
        if (*(volatile u8 *)&mInputLockTimer != 0) {
            *(u8 *)LADR(&mInputLockTimer) -= frameStep;
            return 1;
        }

        state = mState;
        switch (state) {
        case STATE_BOOT_WAIT:
        case STATE_POST_ERASE_WAIT:
            if (*(volatile u16 *)&mFadeTimer != 0) {
                *(u16 *)LADR(&mFadeTimer) -= frameStep;
                if (mFadeTimer == 0) {
                    StartSceneFade(func_0203da3c() != 0 ? 6 : 1, 0, 0);
                } else if (mState == STATE_BOOT_WAIT && keysHeld == 0xf03) { /* A+B+X+Y+L+R */
                    u16 langFileID;
                    int palette;

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
                    LoadCompressedFileAt(langFileID, (void *)G2S::GetBG1CharPtr());
                    palette = LoadFile(0x9807);
                    GXS::LoadBGPltt((const void *)palette, 0, 0x1c0);
                    Deallocate((void *)palette);
                    LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_INITIALIZE_NSC_BIN,
                                         (void *)G2S::GetBG1ScrPtr());
                    LoadCompressedFileAt(0x9803, (char *)G2S::GetBG1ScrPtr() + 0x800);
                    func_0201cd08(0x29a);
                    mSelectedButton = 1;
                    func_02005348();
                    SetSubBg0Offset(0, 0);
                    SetSubBg1Offset(0, 0);
                    data_0209d454 |= 3;
                    data_0209d454 &= ~4;
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
            if (touchStarted != 0 || (keysPressed & 0x39)) {
                int leftTouchStarted;

                if (touchDown != 0 && gTouchEdge[playerSlot * 4] != 0) {
                    leftTouchStarted = 1;
                } else {
                    leftTouchStarted = 0;
                }
                /* left button: touch x 0x28..0x78, y 0x98..0xb8, or Left */
                if ((leftTouchStarted != 0 && (u8)(gTouchX[playerSlot * 4] - 0x28) < 0x50 &&
                     (u8)(gTouchY[playerSlot * 4] - 0x98) < 0x20) ||
                    (keysPressed & 0x20)) {
                    if (mSelectedButton == 0) {
                        mButtonFlashTimer = 0x10;
                    }
                    mSelectedButton = 0;
                    func_02005348();
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
                        (keysPressed & 0x10)) {
                        if (mSelectedButton == 1) {
                            mButtonFlashTimer = 0x10;
                        }
                        mSelectedButton = 1;
                        func_02005348();
                        func_02012790(0);
                        if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x88) < 0x50) {
                            if ((u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20) {
                                mInputLockTimer = 0x20;
                                mState = STATE_CANCEL_ERASE;
                            }
                        }
                    } else if (keysPressed & 9) {
                        mButtonFlashTimer = 0x10;
                        mInputLockTimer = 0x20;
                        func_02005348();
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
            func_02005348();
            mState = STATE_FINAL_CONFIRM;
            break;

        case STATE_BEGIN_ERASE:
            *(volatile u16 *)0x0400100a = (u16)((*(volatile u16 *)0x0400100a & 0x43) | 0x218);
            Message::DisplaySaveStatusText(0x29c);
            mEraseEffectTimer = 0x78;
            mState = STATE_ERASING;
            break;

        case STATE_ERASING:
            if (mEraseEffectTimer == 0x3c) {
                SaveData::EraseAllSaveData();
            }
            if (*(volatile u8 *)&mEraseEffectTimer != 0) {
                u8 eraseFramesLeft;

                *(u8 *)LADR(&mEraseEffectTimer) -= frameStep;
                eraseFramesLeft = mEraseEffectTimer;
                if (eraseFramesLeft == 0x3c) {
                    Message::DisplaySaveStatusText(0x29d);
                } else if (eraseFramesLeft == 0) {
                    data_0209d454 &= ~3;
                    data_0209d454 |= 4;
                    mFadeTimer = 0x3c;
                    mState = STATE_POST_ERASE_WAIT;
                }
            }
            break;

        case STATE_CANCEL_ERASE:
            data_0209d454 &= ~3;
            data_0209d454 |= 4;
            mFadeTimer = 0x3c;
            mState = STATE_BOOT_WAIT;
            break;
        }
    }
    return 1;
}

// @symbol _ZN9dScBoot_c13func_02005348Ev
/* Repaints two 16-by-4-tile buttons on the sub screen's BG1 map at +0x4c0.
 * They sit side by side in a 32-tile-wide map. The selected button uses
 * palette bank 2 when the flash timer is zero; all other tiles use bank 1. */
void dScBoot_c::func_02005348() {
    int button;
    unsigned int paletteBits;
    for (button = 0; button < 2; button++) {
        int selectedPaletteBits;
        unsigned short *buttonMap;
        unsigned short *tiles;
        int column;
        if ((button == mSelectedButton) && (mButtonFlashTimer == 0)) {
            selectedPaletteBits = 0x2000;
        } else {
            selectedPaletteBits = 0x1000;
        }
        selectedPaletteBits = (unsigned short)selectedPaletteBits;
        buttonMap = (unsigned short *)(((char *)G2S::GetBG1ScrPtr()) + 0x4c0);
        paletteBits = selectedPaletteBits;
        tiles = buttonMap + (button * 0x10);
        for (column = 0; column < 0x10; column++) {
            tiles[column] = selectedPaletteBits + (tiles[column] & 0x3ff);
            (&tiles[column])[0x20] = paletteBits + ((&tiles[column])[0x20] & 0x3ff);
            (&tiles[column])[0x40] = paletteBits + ((&tiles[column])[0x40] & 0x3ff);
            (&tiles[column])[0x60] = selectedPaletteBits + ((&tiles[column])[0x60] & 0x3ff);
        }
    }
}

// @symbol func_02005324
/* Absorbed from the retired arm9 helper func_02005324
 * (arm9 0x02005324..0x02005348): pre-talk camera backup plus event-flag
 * setup, returns 1. Defined last because this file emits .text in reverse
 * source order, so the lead body lands at the run's left edge. The camera
 * call is spelled as the member it is (dCamera_c.h); func_0200cb58 comes
 * from decl_common.h. */
extern "C" int func_02005324(void *p) {
    ((dCamera_c *)p)->SaveCameraStateBeforeTalk();
    func_0200cb58(p, 0x19);
    return 1;
}
