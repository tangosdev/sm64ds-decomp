//cpp
// @symbol _ZN12dScMgSound_c8BehaviorEv
#include "types.h"
#include "decl_common.h"
#include "dScMgSound_c.h"
/* dScMgSound_c::Behavior -- vtable slot 6, ov006 0x0211c720.
 *
 * Attributed by the vtable: dScMgSound_c's own table is ov006 0x0213f844 and
 * its slot 6 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 6, `virtual s32 Behavior()`.
 *
 * The minigame's state machine, on the word at 0x5608: 0 = init, 1 = intro
 * countdown at 0x5618, 2 = result countdown at 0x5616 with the win/lose
 * handling and the 9999-capped win counter, 3 = retry countdown. Every one of
 * those offsets is INSIDE mTable, the single 0x6f4-byte member
 * include/dScMgSound_c.h describes, so they stay raw casts off the scene
 * rather than becoming fields of this class -- func_ov006_020c33dc and
 * func_ov006_020c3288 own that object's shape, and nothing in this tree has
 * recovered it.
 *
 * mPromptBlinkTimer, mPromptEnabled and mPromptBlinkCount are dScMgBase_c's, and read as inherited
 * members. The pre-migration file wrote all three as `*(u8 *)(c + 0xc3)` and
 * could not have said whose they were.
 *
 * THE THREE COUNTDOWN DECREMENTS KEEP THEIR `(int)` LAUNDER. Writing
 * `(*(u16 *)(c + 0x5618))--` lets mwccarm re-fold the address into the
 * post-decrement and the function comes out short; the cast through int is
 * what forces the address into a register first, which is what the cartridge
 * does. Same lever notes/mwccarm-codegen.md records for compound assignment. */

extern "C" {
void FreeGfxSlotsById(int id);
void func_ov006_020c2594(char *p);
}

s32 dScMgSound_c::Behavior()
{
    char *c = (char *)this;

    switch (*(int *)(c + 0x5608)) {
    case 0:
        *(int *)(c + 0x5608) = 1;
        break;
    case 1:
        if (*(u16 *)(c + 0x5618) != 0) {
            (*(u16 *)(int)(c + 0x5618))--;
            if (*(u16 *)(c + 0x5618) == 0) {
                FreeGfxSlotsById(0x1d);
                if (mPromptBlinkCount == 0) {
                    mPromptEnabled = 1;
                    mPromptBlinkCount = 1;
                    mPromptBlinkTimer = 0;
                }
            }
        }
        func_ov006_0211b954(c);
        func_ov006_0211b80c(c);
        func_ov006_0211b5e0(c);
        func_ov006_0211b790(c);
        break;
    case 2:
        func_ov006_0211b954(c);
        func_ov006_0211b5e0(c);
        if (*(u16 *)(c + 0x5616) != 0) {
            (*(u16 *)(int)(c + 0x5616))--;
            if (*(u16 *)(c + 0x5616) == 0) {
                if (*(u8 *)(c + 0x5626) != 0) {
                    *(int *)(c + 0x50e0) = 0;
                    func_ov006_020c2594((char *)mTable);
                    if (*(u8 *)(c + 0x5626) == 3)
                        func_ov004_020b67f8();
                    func_ov004_020b0a54(0);
                    {
                        char *g = (char *)data_ov004_020beb68;
                        if (g != 0) {
                            if (*(int *)(g + 0xb4) < 9999)
                                (*(int *)(int)(g + 0xb4))++;
                            if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                                *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
                        }
                    }
                    func_ov004_020adb1c(data_ov004_020beb68 != 0
                                            ? *(int *)((char *)data_ov004_020beb68 + 0xb4) : 0);
                    func_ov006_02119ba4(c);
                    func_ov006_02119a88(c);
                    mPromptEnabled = 0;
                } else {
                    *(int *)(c + 0x5608) = 3;
                    *(u16 *)(c + 0x5616) = 0x20;
                    func_ov006_0211b9c8(c);
                }
            }
        } else {
            func_ov006_02119b00(c);
            func_ov006_02119a18(c);
        }
        break;
    case 3:
        func_ov006_0211b954(c);
        func_ov006_0211b5e0(c);
        if (*(u16 *)(c + 0x5616) != 0) {
            (*(u16 *)(int)(c + 0x5616))--;
            if (*(s16 *)(c + 0x5616) <= 0) {
                *(int *)(c + 0x50e0) = 0;
                func_ov006_020c2440((char *)mTable);
                func_ov004_020b0a54(0x12);
                mPromptEnabled = 0;
                *(u16 *)(c + 0x5616) = 0;
            }
        }
        break;
    }

    func_ov006_020c2b8c((char *)mTable);
    return 1;
}
