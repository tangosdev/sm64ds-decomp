//cpp
// @symbol _ZN9BootScene8BehaviorEv
/* BootScene::Behavior -- vtable slot 6.
 *
 * The boot menu's state machine, in mState: wait out the fade (0/7), watch the
 * touch screen for the two language/erase buttons (1/3), confirm (2/6), and run
 * the "erase all save data" countdown (4/5). data_0208ee44 is the frame step,
 * so every countdown decrements by it rather than by one.
 *
 * DO NOT TIDY THE `volatile' READS OR THE LADR() WRITES: they are per-SITE and
 * measured. Each one is a read-modify-write on a countdown, where spelling the
 * member plainly lets mwccarm CSE the field address and costs an instruction.
 * The plain member reads sitting next to them are fine as they are.
 */
#include "BootScene.h"
#include "decl_Message.h"
#include "decl_SaveData.h"
#include "decl_common.h"
#include "MessageBank.h"

extern "C" {
extern u8 data_0209f5bc[];
extern u8 data_0209f1e8;
extern u8 data_020a0e40;
extern u16 data_020a0e58[];
extern u16 data_020a0e5a[];
extern int data_0208ee44;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern u8 data_0209d454;

void _ZN8dScene_c14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
u32 _ZN3G2S13GetBG1CharPtrEv(void);
u32 LoadCompressedFileAt(u16 fileID, void *target);
int LoadFile(int handle);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void *_ZN3G2S12GetBG1ScrPtrEv(void);
void func_02012790(int a);
}

#define LADR(p) ((void *)(unsigned int)(p))

#pragma opt_common_subs off

s32 BootScene::Behavior()
{
    u16 keysHeld, keysPressed;
    int r4;

    r4 = data_0208ee44;
    keysHeld = *(u16 *)((char *)data_020a0e58 + data_020a0e40 * 4);
    keysPressed = *(u16 *)((char *)data_020a0e5a + data_020a0e40 * 4);

    if (data_0209f1e8 == 0) {
        data_0209f1e8 = (u8)func_0201a1bc();
        if (data_0209f1e8 == 0) {
            if ((int)*(volatile u16 *)&mFadeTimer > (int)(r4 * 2)) {
                *(u16 *)LADR(&mFadeTimer) -= r4;
            }
            return 1;
        }
        LoadMessageBankForLanguage();
    }

    {
    void *obj = *(void **)data_0209f5bc;
    int (**vt)(void *) = *(int (***)(void *))obj;
    if (vt[5](obj) != 0) {
        int state;

        if (*(volatile u8 *)&mButtonFlashTimer != 0) {
            *(u8 *)LADR(&mButtonFlashTimer) -= r4;
            if (mButtonFlashTimer == 0) {
                func_02005348(this);
            }
        }
        if (*(volatile u8 *)&mInputLockTimer != 0) {
            *(u8 *)LADR(&mInputLockTimer) -= r4;
            return 1;
        }

        state = mState;
        switch (state) {
        case 0:
        case 7:
            if (*(volatile u16 *)&mFadeTimer != 0) {
                *(u16 *)LADR(&mFadeTimer) -= r4;
                if (mFadeTimer == 0) {
                    _ZN8dScene_c14StartSceneFadeEjjt(func_0203da3c() != 0 ? 6 : 1, 0, 0);
                } else if (mState == 0 && keysHeld == 0xf03) { /* A+B+X+Y+L+R */
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
                    LoadCompressedFileAt(langFileID, (void *)_ZN3G2S13GetBG1CharPtrEv());
                    palette = LoadFile(0x9807);
                    _ZN3GXS10LoadBGPlttEPKvjj((const void *)palette, 0, 0x1c0);
                    Deallocate((void *)palette);
                    LoadCompressedFileAt(0x22d, _ZN3G2S12GetBG1ScrPtrEv());
                    LoadCompressedFileAt(0x9803, (char *)_ZN3G2S12GetBG1ScrPtrEv() + 0x800);
                    func_0201cd08(0x29a);
                    mSelectedButton = 1;
                    func_02005348(this);
                    SetSubBg0Offset(0, 0);
                    SetSubBg1Offset(0, 0);
                    data_0209d454 |= 3;
                    data_0209d454 &= ~4;
                    mState = 1;
                }
            }
            break;

        case 1:
        case 3:
        {
            int padIndex = data_020a0e40;
            u8 touchDown;

            r4 = 0;
            touchDown = data_020a0de8[padIndex * 4];
            if (touchDown != 0 && data_020a0de9[padIndex * 4] != 0) {
                r4 = 1;
            }
            if (r4 != 0 || (keysPressed & 0x39)) {
                int ok2;

                if (touchDown != 0 && data_020a0de9[padIndex * 4] != 0) {
                    ok2 = 1;
                } else {
                    ok2 = 0;
                }
                /* left button: touch x 0x28..0x78, y 0x98..0xb8, or Left */
                if ((ok2 != 0
                     && (u8)(data_020a0dea[padIndex * 4] - 0x28) < 0x50
                     && (u8)(data_020a0deb[padIndex * 4] - 0x98) < 0x20)
                    || (keysPressed & 0x20)) {
                    if (mSelectedButton == 0) {
                        mButtonFlashTimer = 0x10;
                    }
                    mSelectedButton = 0;
                    func_02005348(this);
                    func_02012790(0);
                    if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x28) < 0x50
                        && (u8)(data_020a0deb[data_020a0e40 * 4] - 0x98) < 0x20) {
                        mInputLockTimer = 0x20;
                        if (mState == 1) {
                            mState = 2;
                        } else {
                            mState = 4;
                        }
                    }
                } else {
                    int ok3;

                    if (touchDown != 0 && data_020a0de9[padIndex * 4] != 0) {
                        ok3 = 1;
                    } else {
                        ok3 = 0;
                    }
                    /* right button: touch x 0x88..0xd8, same rows, or Right */
                    if ((ok3 != 0
                         && (u8)(data_020a0dea[padIndex * 4] - 0x88) < 0x50
                         && (u8)(data_020a0deb[padIndex * 4] - 0x98) < 0x20)
                        || (keysPressed & 0x10)) {
                        if (mSelectedButton == 1) {
                            mButtonFlashTimer = 0x10;
                        }
                        mSelectedButton = 1;
                        func_02005348(this);
                        func_02012790(0);
                        if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x88) < 0x50) {
                            if ((u8)(data_020a0deb[data_020a0e40 * 4] - 0x98) < 0x20) {
                                mInputLockTimer = 0x20;
                                mState = 6;
                            }
                        }
                    } else if (keysPressed & 9) {
                        mButtonFlashTimer = 0x10;
                        mInputLockTimer = 0x20;
                        func_02005348(this);
                        func_02012790(0);
                        if (mSelectedButton == 0) {
                            if (mState == 1) {
                                mState = 2;
                            } else {
                                mState = 4;
                            }
                        } else {
                            mState = 6;
                        }
                    }
                }
            }
            break;
        }

        case 2:
            func_0201cd08(0x29b);
            mSelectedButton = 1;
            func_02005348(this);
            mState = 3;
            break;

        case 4:
            *(volatile u16 *)0x0400100a = (u16)((*(volatile u16 *)0x0400100a & 0x43) | 0x218);
            _ZN7Message21DisplaySaveStatusTextEt(0x29c);
            mEraseEffectTimer = 0x78;
            mState = 5;
            break;

        case 5:
            if (mEraseEffectTimer == 0x3c) {
                _ZN8SaveData16EraseAllSaveDataEv();
            }
            if (*(volatile u8 *)&mEraseEffectTimer != 0) {
                u8 t;

                *(u8 *)LADR(&mEraseEffectTimer) -= r4;
                t = mEraseEffectTimer;
                if (t == 0x3c) {
                    _ZN7Message21DisplaySaveStatusTextEt(0x29d);
                } else if (t == 0) {
                    data_0209d454 &= ~3;
                    data_0209d454 |= 4;
                    mFadeTimer = 0x3c;
                    mState = 7;
                }
            }
            break;

        case 6:
            data_0209d454 &= ~3;
            data_0209d454 |= 4;
            mFadeTimer = 0x3c;
            mState = 0;
            break;
        }
    }
    }
    return 1;
}
