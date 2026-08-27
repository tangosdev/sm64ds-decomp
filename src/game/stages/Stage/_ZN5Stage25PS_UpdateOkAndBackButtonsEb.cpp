//cpp
/* Stage::PS_UpdateOkAndBackButtons @ 0x02025d20 (arm9, size 0x154)
 * [mwccarm 1.2/base, 1.2/sp2, 1.2/sp2p3 and 2004/b56 all byte-identical]
 * Lever (notes 6au): TWO coupled changes that only work together.
 *   1. the array object is declared `extern volatile int data_02075610[];`
 *   2. there is NO named pointer local -- the array is indexed INLINE in the loop bound.
 * A named pointer local puts the base in mwcc's address-constant class, which is colored
 * after every normal web and lands it in r7; that alone was the whole 9-word miss, and it
 * dominates even if the pointer is itself volatile. A plain inline access rematerializes
 * the base in normal birth order but into a scratch register and caches the bound, which
 * drops the tail reload and comes out three words SHORT. The volatile forces the bound to
 * be re-read every iteration (the ROM reloads at +0xc0 and +0x114), which keeps the base
 * live across the inner loop so it must take a callee-saved register -- but via the
 * normal-birth path, not the address-constant class. Result ptr r5 / 0x1000 r6 / idx r7,
 * exactly the ROM. Do NOT reintroduce a named pointer and do NOT drop the volatile;
 * either one alone regresses. Link-verified: VERIFIED, 0 diffs, 0 blind.
 */
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" int GetOwnerLanguage(void);

namespace G2S { u16* GetBG1ScrPtr(); }

extern volatile int data_02075610[];
extern u16 data_020755cc[];
extern u8 data_0209f2b4;
extern u8 data_0209f2e0;
extern u8 data_0209f29c;
extern u8 data_0209f244;

struct Stage {
    static void PS_UpdateOkAndBackButtons(bool b);
};

void Stage::PS_UpdateOkAndBackButtons(bool b)
{
    int sl;
    int i;
    int base;
    int data_0x1000;
    int idx;
    int data_0x2000;
    int j;

    if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 3)
        base = 2;
    else
        base = 0;

    for (i = 0; i < 2; i++) {
        data_0x2000 = 0x2000;
        data_0x1000 = 0x1000;

        if (b) {
            sl = data_0x1000;
        } else {
            int v;
            if (data_0209f2b4 == data_0209f2e0 && data_0209f29c == i && data_0209f244 == 0)
                v = data_0x2000;
            else
                v = data_0x1000;
            sl = (u16)v;
        }

        idx = i + base;
        u16* scr = G2S::GetBG1ScrPtr() + data_020755cc[idx];
        for (j = 0; j < data_02075610[idx]; j++) {
            scr[0] = (scr[0] & 0x3ff) | sl;
            scr[0x20] = (scr[0x20] & 0x3ff) | sl;
            scr[0x40] = (scr[0x40] & 0x3ff) | sl;
            scr[0x60] = (scr[0x60] & 0x3ff) | sl;
            scr++;
        }
    }
}
