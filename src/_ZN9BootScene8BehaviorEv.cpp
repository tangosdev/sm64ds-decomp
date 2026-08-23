//cpp
// @symbol _ZN9BootScene8BehaviorEv
/* BootScene::Behavior -- vtable slot 6, arm9 0x02005418.
 *
 * The boot menu's whole state machine, in unk_052: wait out the fade (0/7),
 * watch the touch screen for the two language/erase buttons (1/3), confirm
 * (2/6), and run the "erase all save data" countdown (4/5). unk_050 is the
 * frame countdown, unk_053 which button is highlighted, unk_054/unk_055 the
 * two debounce timers, unk_056 the erase countdown. data_0208ee44 is the frame
 * step, so every countdown decrements by it rather than by one.
 *
 * MIGRATED DESPITE include/BootScene.h's "NOT CONVERTED" note -- see
 * src/_ZN9BootScene13InitResourcesEv.cpp for why that note's second reason is
 * not the mechanism and the destructor being declared first is.
 *
 * THE `volatile' READS AND THE LADR() WRITES ARE THE ROM'S, not decoration:
 * every one of them is a read-modify-write on a countdown, where naming the
 * member lets mwccarm CSE the field address and costs an instruction. They are
 * per-SITE -- the plain `unk_053' sites next to them convert to
 * members and these do not.
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
    u16 h58, h5a;
    int r4;

    r4 = data_0208ee44;
    h58 = *(u16 *)((char *)data_020a0e58 + data_020a0e40 * 4);
    h5a = *(u16 *)((char *)data_020a0e5a + data_020a0e40 * 4);

    if (data_0209f1e8 == 0) {
        data_0209f1e8 = (u8)func_0201a1bc();
        if (data_0209f1e8 == 0) {
            if ((int)*(volatile u16 *)&unk_050 > (int)(r4 * 2)) {
                *(u16 *)LADR(&unk_050) -= r4;
            }
            return 1;
        }
        LoadMessageBankForLanguage();
    }

    {
    void *obj = *(void **)data_0209f5bc;
    int (**vt)(void *) = *(int (***)(void *))obj;
    if (vt[5](obj) != 0) {
        int f52;

        if (*(volatile u8 *)&unk_054 != 0) {
            *(u8 *)LADR(&unk_054) -= r4;
            if (unk_054 == 0) {
                func_02005348(this);
            }
        }
        if (*(volatile u8 *)&unk_055 != 0) {
            *(u8 *)LADR(&unk_055) -= r4;
            return 1;
        }

        f52 = unk_052;
        switch (f52) {
        case 0:
        case 7:
            if (*(volatile u16 *)&unk_050 != 0) {
                *(u16 *)LADR(&unk_050) -= r4;
                if (unk_050 == 0) {
                    _ZN8dScene_c14StartSceneFadeEjjt(func_0203da3c() != 0 ? 6 : 1, 0, 0);
                } else if (unk_052 == 0 && h58 == 0xf03) {
                    u16 lang;
                    int f;

                    if (GetOwnerLanguage() == 5) {
                        lang = 0xb00d;
                    } else if (GetOwnerLanguage() == 4) {
                        lang = 0xac0d;
                    } else if (GetOwnerLanguage() == 3) {
                        lang = 0xa80d;
                    } else if (GetOwnerLanguage() == 2) {
                        lang = 0xa40d;
                    } else {
                        lang = 0xa00d;
                    }
                    LoadCompressedFileAt(lang, (void *)_ZN3G2S13GetBG1CharPtrEv());
                    f = LoadFile(0x9807);
                    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0, 0x1c0);
                    Deallocate((void *)f);
                    LoadCompressedFileAt(0x22d, _ZN3G2S12GetBG1ScrPtrEv());
                    LoadCompressedFileAt(0x9803, (char *)_ZN3G2S12GetBG1ScrPtrEv() + 0x800);
                    func_0201cd08(0x29a);
                    unk_053 = 1;
                    func_02005348(this);
                    SetSubBg0Offset(0, 0);
                    SetSubBg1Offset(0, 0);
                    data_0209d454 |= 3;
                    data_0209d454 &= ~4;
                    unk_052 = 1;
                }
            }
            break;

        case 1:
        case 3:
        {
            int pi = data_020a0e40;
            u8 e8;

            r4 = 0;
            e8 = data_020a0de8[pi * 4];
            if (e8 != 0 && data_020a0de9[pi * 4] != 0) {
                r4 = 1;
            }
            if (r4 != 0 || (h5a & 0x39)) {
                int ok2;

                if (e8 != 0 && data_020a0de9[pi * 4] != 0) {
                    ok2 = 1;
                } else {
                    ok2 = 0;
                }
                if ((ok2 != 0
                     && (u8)(data_020a0dea[pi * 4] - 0x28) < 0x50
                     && (u8)(data_020a0deb[pi * 4] - 0x98) < 0x20)
                    || (h5a & 0x20)) {
                    if (unk_053 == 0) {
                        unk_054 = 0x10;
                    }
                    unk_053 = 0;
                    func_02005348(this);
                    func_02012790(0);
                    if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x28) < 0x50
                        && (u8)(data_020a0deb[data_020a0e40 * 4] - 0x98) < 0x20) {
                        unk_055 = 0x20;
                        if (unk_052 == 1) {
                            unk_052 = 2;
                        } else {
                            unk_052 = 4;
                        }
                    }
                } else {
                    int ok3;

                    if (e8 != 0 && data_020a0de9[pi * 4] != 0) {
                        ok3 = 1;
                    } else {
                        ok3 = 0;
                    }
                    if ((ok3 != 0
                         && (u8)(data_020a0dea[pi * 4] - 0x88) < 0x50
                         && (u8)(data_020a0deb[pi * 4] - 0x98) < 0x20)
                        || (h5a & 0x10)) {
                        if (unk_053 == 1) {
                            unk_054 = 0x10;
                        }
                        unk_053 = 1;
                        func_02005348(this);
                        func_02012790(0);
                        if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x88) < 0x50) {
                            if ((u8)(data_020a0deb[data_020a0e40 * 4] - 0x98) < 0x20) {
                                unk_055 = 0x20;
                                unk_052 = 6;
                            }
                        }
                    } else if (h5a & 9) {
                        unk_054 = 0x10;
                        unk_055 = 0x20;
                        func_02005348(this);
                        func_02012790(0);
                        if (unk_053 == 0) {
                            if (unk_052 == 1) {
                                unk_052 = 2;
                            } else {
                                unk_052 = 4;
                            }
                        } else {
                            unk_052 = 6;
                        }
                    }
                }
            }
            break;
        }

        case 2:
            func_0201cd08(0x29b);
            unk_053 = 1;
            func_02005348(this);
            unk_052 = 3;
            break;

        case 4:
            *(volatile u16 *)0x0400100a = (u16)((*(volatile u16 *)0x0400100a & 0x43) | 0x218);
            _ZN7Message21DisplaySaveStatusTextEt(0x29c);
            unk_056 = 0x78;
            unk_052 = 5;
            break;

        case 5:
            if (unk_056 == 0x3c) {
                _ZN8SaveData16EraseAllSaveDataEv();
            }
            if (*(volatile u8 *)&unk_056 != 0) {
                u8 t;

                *(u8 *)LADR(&unk_056) -= r4;
                t = unk_056;
                if (t == 0x3c) {
                    _ZN7Message21DisplaySaveStatusTextEt(0x29d);
                } else if (t == 0) {
                    data_0209d454 &= ~3;
                    data_0209d454 |= 4;
                    unk_050 = 0x3c;
                    unk_052 = 7;
                }
            }
            break;

        case 6:
            data_0209d454 &= ~3;
            data_0209d454 |= 4;
            unk_050 = 0x3c;
            unk_052 = 0;
            break;
        }
    }
    }
    return 1;
}
