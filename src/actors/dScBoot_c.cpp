//cpp
/* dScBoot_c -- the boot scene: the title/language screen with the two-button
 * menu that can erase all save data. Class root
 * fBase_c -> dBase_c -> dScene_c -> dScBoot_c (the ROM's own RTTI name), a
 * leaf. Layout
 * and derivation evidence: include/dScBoot_c.h, notes/scene-provenance.md.
 *
 * This TU is the class's scene methods: the button-row redraw helper
 * func_02005348, the slot-6 state machine Behavior and the slot-0
 * InitResources, .text 0x02005348..0x02005d94. The destructor pair and the
 * dScBoot_c_classInit factory live in the second TU, src/d_s_boot.cpp.
 * Source order is the reverse of the ROM's -- mwccarm emits .text in
 * reverse source order. Do not reorder.
 *
 * func_02005348 is a real dScBoot_c member: its only caller is Behavior,
 * always on `this`, and the body reads mSelectedButton and mButtonFlashTimer.
 * Kept under its address-derived label; symbols.txt carries the mangled
 * spelling _ZN9dScBoot_c13func_02005348Ev (S33).
 *
 * #pragma opt_common_subs off is file-global last-wins in mwccarm 2004/b56.
 * Behavior's countdown read-modify-writes need it; the other two functions
 * verify with it set, so it costs nothing.
 *
 * Do NOT tidy the `volatile' reads or the LADR() writes in Behavior: they
 * are per-site and measured. Each is a read-modify-write on a countdown,
 * where spelling the member plainly lets mwccarm CSE the field address and
 * costs an instruction. The plain member reads sitting next to them are
 * fine as they are.
 */
#include "dScBoot_c.h"

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

u32 _ZN3G2S13GetBG1CharPtrEv(void);
u32 LoadCompressedFileAt(u16 fileID, void *target);
int LoadFile(int handle);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
u32 _ZN3G2S12GetBG1ScrPtrEv(void);
u32 func_02012790(u32 a);
void func_0201a2f8(void);
void _ZN2GX12SetBankForBGEt(u16 bank);
void _ZN2GX13SetBankForOBJEt(u16 bank);
void _ZN2GX15SetBankForSubBGEt(u16 bank);
void _ZN2GX16SetBankForSubOBJEt(u16 bank);
void DecompressLZ16(const void *src, void *dst);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 offset, u32 size);
void *_ZN2G212GetBG0ScrPtrEv(void);
u32 _ZN3G2S13GetBG2CharPtrEv(void);
u32 _ZN3G2S12GetBG2ScrPtrEv(void);
int func_0201a244(int fn, int a, int b, int c, int d);
}

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
s32 dScBoot_c::InitResources()
{
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX12SetBankForBGEt(1);
    _ZN2GX13SetBankForOBJEt(2);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x38000000) | 0x8000000;
    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & 0xfffffdf1) | 0x20e;
    _ZN2GX6DispOnEv();

    *(volatile u32 *)0x4001000 |= 0x10000;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x1710;
    *(volatile u16 *)0x4000008 = *(volatile u16 *)0x4000008 & ~0x40;
    SetBg0Offset(0, 0);

    DecompressLZ16(&data_020918c4, func_02054efc());
    _ZN2GX10LoadBGPlttEPKvjj(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_020916d8, _ZN2G212GetBG0ScrPtrEv());

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

    DecompressLZ16(&data_020918c4, (void *)_ZN3G2S13GetBG2CharPtrEv());
    _ZN3GXS10LoadBGPlttEPKvjj(&data_020914e0, 0, 2);
    _ZN3GXS10LoadBGPlttEPKvjj(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_02091570, (void *)_ZN3G2S12GetBG2ScrPtrEv());

    data_0209d45c = 1;
    data_0209d454 = 4;
    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | 0x100;
    *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x400;

    mFadeTimer = 0x3c;
    mState = 0;
    mButtonFlashTimer = 0;
    mInputLockTimer = 0;
    data_0208ee44 = 1;
    func_020233f4();

    data_0209f5e8.color = 0x7fff;
    Sound::Play2D(4, 0);
    data_0209f1e8 = (u8)func_0201a244((int)func_0201a2f8, 0, 0xf, 0, 0x1000);
    return 1;
}

// @symbol _ZN9dScBoot_c8BehaviorEv
/* dScBoot_c::Behavior -- vtable slot 6.
 *
 * The boot menu's state machine, in mState: wait out the fade (0/7), watch
 * the touch screen for the two language/erase buttons (1/3), confirm (2/6),
 * and run the "erase all save data" countdown (4/5). data_0208ee44 is the
 * frame step, so every countdown decrements by it rather than by one.
 */
s32 dScBoot_c::Behavior()
{
    u16 keysHeld, keysPressed;
    int r4;

    r4 = data_0208ee44;
    keysHeld = *(u16 *)((char *)data_020a0e58 + gActivePlayerSlot * 4);
    keysPressed = *(u16 *)((char *)data_020a0e5a + gActivePlayerSlot * 4);

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
    if (data_0209f5bc->IsAtStart() != 0) {
        int state;

        if (*(volatile u8 *)&mButtonFlashTimer != 0) {
            *(u8 *)LADR(&mButtonFlashTimer) -= r4;
            if (mButtonFlashTimer == 0) {
                func_02005348();
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
                    StartSceneFade(func_0203da3c() != 0 ? 6 : 1, 0, 0);
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
                    LoadCompressedFileAt(0x22d, (void *)_ZN3G2S12GetBG1ScrPtrEv());
                    LoadCompressedFileAt(0x9803, (char *)_ZN3G2S12GetBG1ScrPtrEv() + 0x800);
                    func_0201cd08(0x29a);
                    mSelectedButton = 1;
                    func_02005348();
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
            int padIndex = gActivePlayerSlot;
            u8 touchDown;

            r4 = 0;
            touchDown = gTouchHeld[padIndex * 4];
            if (touchDown != 0 && gTouchEdge[padIndex * 4] != 0) {
                r4 = 1;
            }
            if (r4 != 0 || (keysPressed & 0x39)) {
                int ok2;

                if (touchDown != 0 && gTouchEdge[padIndex * 4] != 0) {
                    ok2 = 1;
                } else {
                    ok2 = 0;
                }
                /* left button: touch x 0x28..0x78, y 0x98..0xb8, or Left */
                if ((ok2 != 0
                     && (u8)(gTouchX[padIndex * 4] - 0x28) < 0x50
                     && (u8)(gTouchY[padIndex * 4] - 0x98) < 0x20)
                    || (keysPressed & 0x20)) {
                    if (mSelectedButton == 0) {
                        mButtonFlashTimer = 0x10;
                    }
                    mSelectedButton = 0;
                    func_02005348();
                    func_02012790(0);
                    if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0x50
                        && (u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20) {
                        mInputLockTimer = 0x20;
                        if (mState == 1) {
                            mState = 2;
                        } else {
                            mState = 4;
                        }
                    }
                } else {
                    int ok3;

                    if (touchDown != 0 && gTouchEdge[padIndex * 4] != 0) {
                        ok3 = 1;
                    } else {
                        ok3 = 0;
                    }
                    /* right button: touch x 0x88..0xd8, same rows, or Right */
                    if ((ok3 != 0
                         && (u8)(gTouchX[padIndex * 4] - 0x88) < 0x50
                         && (u8)(gTouchY[padIndex * 4] - 0x98) < 0x20)
                        || (keysPressed & 0x10)) {
                        if (mSelectedButton == 1) {
                            mButtonFlashTimer = 0x10;
                        }
                        mSelectedButton = 1;
                        func_02005348();
                        func_02012790(0);
                        if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x88) < 0x50) {
                            if ((u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20) {
                                mInputLockTimer = 0x20;
                                mState = 6;
                            }
                        }
                    } else if (keysPressed & 9) {
                        mButtonFlashTimer = 0x10;
                        mInputLockTimer = 0x20;
                        func_02005348();
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
            func_02005348();
            mState = 3;
            break;

        case 4:
            *(volatile u16 *)0x0400100a = (u16)((*(volatile u16 *)0x0400100a & 0x43) | 0x218);
            Message::DisplaySaveStatusText(0x29c);
            mEraseEffectTimer = 0x78;
            mState = 5;
            break;

        case 5:
            if (mEraseEffectTimer == 0x3c) {
                SaveData::EraseAllSaveData();
            }
            if (*(volatile u8 *)&mEraseEffectTimer != 0) {
                u8 t;

                *(u8 *)LADR(&mEraseEffectTimer) -= r4;
                t = mEraseEffectTimer;
                if (t == 0x3c) {
                    Message::DisplaySaveStatusText(0x29d);
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

// @symbol _ZN9dScBoot_c13func_02005348Ev
/* Repaints the two menu buttons on the sub screen's BG1 tilemap: the 0x10-tile
 * strip at +0x4c0, two rows of four palette layers. The selected row's tiles
 * get palette bank 2 while the flash timer is up, bank 1 otherwise; the
 * unselected row keeps bank 1. */
void dScBoot_c::func_02005348()
{
    int i;
    unsigned int new_var;
    for (i = 0; i < 2; i++)
    {
        int v;
        unsigned short *base;
        unsigned short *row;
        int j;
        if ((i == mSelectedButton) && (mButtonFlashTimer == 0))
        {
            v = 0x2000;
        }
        else
        {
            v = 0x1000;
        }
        v = (unsigned short) v;
        base = (unsigned short *) (((char *) _ZN3G2S12GetBG1ScrPtrEv()) + 0x4c0);
        new_var = v;
        row = base + (i * 0x10);
        for (j = 0; j < 0x10; j++)
        {
            row[j] = v + (row[j] & 0x3ff);
            (&row[j])[0x20] = new_var + ((&row[j])[0x20] & 0x3ff);
            (&row[j])[0x40] = new_var + ((&row[j])[0x40] & 0x3ff);
            (&row[j])[0x60] = v + ((&row[j])[0x60] & 0x3ff);
        }
    }
}
