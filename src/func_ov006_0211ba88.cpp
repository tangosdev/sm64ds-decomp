//cpp
// @symbol func_ov006_0211ba88
/* recovered: dScMgSound_c helper, ov006 0x0211ba88 (344 bytes). One entry of
 * the 0x14-byte table at +0x50e8 scrolls left by 0x10 units per call; when
 * its +0x11 flag is clear the entry index is reduced modulo the per-mode
 * column count (data_ov006_0212ef5c / 0212ef6c, chosen by Virtual8C, indexed
 * by the mode byte at +0x5627) with a repeated-subtraction loop, a zero
 * remainder marks the entry, and otherwise the entries before it in the same
 * column are checked for a crowding stop (+0x0c := 4). Once the position
 * falls below -0x17 the entry's +0x0d/+0x0e bytes are cleared.
 *
 * The three shapes the cartridge insists on: the question is asked of the
 * scene ITSELF through the vtable (C++ `Virtual8C()`, which loads the table
 * through the r0 `this` copy; a C `p->vt->f(p)` spelling loads it through the
 * callee-saved copy), the crowding-stop store at +0x0c is a BYTE, and the
 * final two clears index the table as an array (`ent[idx]`) while every
 * earlier access is explicit `idx * 0x14` pointer math -- mwccarm keeps the
 * subscript scale and the explicit product in different common-subexpression
 * classes, so the epilogue recomputes idx*0x14 with a fresh mla and r6 is
 * free for the loop's hoisted #0x14. */
#include "types.h"
#include "decl_common.h"
#include "dScMgSound_c.h"

struct SndEntry {
    s32 x;
    u8 unk04[0x0c - 0x04];
    u8 b0c;
    u8 b0d;
    u8 b0e;
    u8 b0f;
    u8 b10;
    u8 b11;
    u8 b12;
    u8 b13;
};

struct SndCtx {
    u8 unk00[0x50e8];
    SndEntry ent[67];
    u8 unk5624[3];
    u8 mode;
};

extern "C" {
extern int data_ov006_0212ef5c[];
extern int data_ov006_0212ef6c[];
void func_ov006_0211ba88(char *base, int idx);
}

void func_ov006_0211ba88(char *base, int idx)
{
    SndCtx *ctx = (SndCtx *)base;
    int n;
    int i;
    int limit;
    s32 *cur;
    u8 *flag;

    cur = (s32 *)(base + 0x50e8 + idx * 0x14);
    *cur = *cur - 0x10000;

    if (((dScMgSound_c *)base)->Virtual8C() != 0) {
        limit = data_ov006_0212ef5c[*(u8 *)(base + 0x5000 + 0x627)];
    } else {
        limit = data_ov006_0212ef6c[*(u8 *)(base + 0x5000 + 0x627)];
    }

    flag = (u8 *)(base + 0x50f9 + idx * 0x14);
    if (*flag == 0) {
        n = idx;
        if (idx >= limit) {
            do {
                n = n - limit;
            } while (n >= limit);
        }
        if (n == 0) {
            *flag = 1;
            return;
        }
        for (i = 0; i < n; i++) {
            char *e = base + (idx - i - 1) * 0x14;
            if (*(u8 *)(e + 0x5000 + 0xf5) != 0) {
                if ((*(int *)(base + idx * 0x14 + 0x50e8) - *(int *)(e + 0x5000 + 0xe8)) >> 0xc <= 4) {
                    *(u8 *)(base + idx * 0x14 + 0x50f9) = 1;
                    *(u8 *)(e + 0x5000 + 0xf4) = 4;
                }
            }
        }
    }

    if (*cur >> 0xc > -0x18) return;

    ctx->ent[idx].b0d = 0;
    ctx->ent[idx].b0e = 0;
}
