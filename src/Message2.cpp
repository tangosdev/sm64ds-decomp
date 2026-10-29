//cpp
/* Message talk window: EndTalk and PrepareTalk, the three control-code
 * handlers in MSG_GEN_TEXT_FUNCS, the glyph blitters and the line layout
 * pass, then UpdateWindow (the open/close wipe) and Update (the per-frame
 * state machine). The run is 0x0201ab04..0x0201cb2c.
 *
 * leftovers:
 * - defer_codegen is off so this file emits in source order. The definitions
 *   run ROM-ascending, and func_0201b388's opt_common_subs off is a bracket;
 *   under deferred codegen it would be last-wins for the whole file.
 * - Sound::ChangeMusicVolume and OAM::Render stay extern "C" free functions.
 *   Each takes a Fix12<int> by value; a method with that parameter homes
 *   r0-r3 to the stack.
 * - The G2/G2S getters are declared locally in their namespaces. No
 *   header declares them yet.
 * - data_0209d6f4 is u8 * here. Message.cpp spells the same word volatile int
 *   because one store there must not be coalesced. This TU only reads bytes.
 * - MsgTextChar is local. The bytes at data_0209d6f4 are the text cursor,
 *   and no header describes them.
 * - Address-named helpers stay address-named. The ROM has no English name
 *   for them.
 */

#pragma defer_codegen off
#include "types.h"
#include "Message.h"
#include "PlayerInput.h"
#include "SaveData.h"
#include "Sound.h"

extern "C" {
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u8 data_0209d64c;
extern u8 data_0209d650;
extern u8 data_0209d658;
extern u8 data_0209d65c;
extern u8 data_0209d660;
extern u8 data_0209d664; /* nonzero while a dialogue box is up */
extern u8 data_0209d668;
extern u8 data_0209d66c;
extern u8 data_0209d670;
extern u8 data_0209d674;
extern u8 data_0209d678;
extern u8 data_0209d680;
extern u8 data_0209d684;
extern u8 data_0209d688;
extern u8 data_0209d68c;
extern u8 data_0209d690;
extern u8 data_0209d694;
extern u8 data_0209d698;
extern u8 data_0209d69c;
extern u8 data_0209d6a0;
extern u8 data_0209d6a4;
extern u8 data_0209d6a8;
extern u8 data_0209d6ac;
extern u8 data_0209d6b0;
extern u8 data_0209d6b4;
extern u8 data_0209d6b8;
extern u8 data_0209d6bc;
extern u8 data_0209d6c0;
extern u8 data_0209d6c4;
extern u8 data_0209d6c8;
extern u8 data_0209d6cc;
extern u8 data_0209d6d0;
extern s16 data_0209d6d8;
extern s16 data_0209d6dc;
extern u16 data_0209d6e0;
extern u8 data_0209d6e4[];
extern u8 *data_0209d6f0;
extern u8 *data_0209d6f4;
extern int data_0209d6f8;
extern int data_0209d700;
extern int data_0209d704;
extern u8 data_0209d710[];
extern int data_0209d74c[];
extern s32 data_0208ee44;
extern u16 data_0208ee64[];
extern u8 data_0208ee6c[];
extern u8 data_0208ee74[];
extern int data_0208ee8c[];
extern int data_0208eeac[];
extern int data_0208eecc[];
extern u8 data_0208f074[];
extern u8 data_0208f174[];
extern int data_0209caa0[];
extern u32 data_0209b454;
extern u8 data_0209f2d8;
extern s16 data_0209f4a2[];
extern s16 data_0209f4a4[];
extern u16 data_020a0e5a[];
extern s32 data_020a0db0;
extern u8 data_ov002_0210c390;
extern u8 data_ov002_0210c398;
extern u8 data_ov002_0210c3a0;
extern u8 data_ov002_0210c3a8;
extern void (*MSG_GEN_TEXT_FUNCS[])(void);

extern int func_02054d88(void);
extern void *func_02054ea8(void);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern void MultiStore_Int(int val, int *dst, int len);
extern int IsButtonInputValid(void);
extern int func_0201fb4c(void);
extern u8 NumStars(void);
extern int func_020138dc(void);
extern int __aeabi_idiv(int, int);
}
extern "C" {
void OamRender10_Message2(int a, void *b, int c, int d, int e, int f, int g, int h, int i, int j);
}

struct OamAttr;
struct OAM {
    static void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
};

namespace G2 {
void *GetBG2CharPtr();
}

namespace G2S {
unsigned int GetBG0CharPtr();
unsigned int GetBG1CharPtr();
}

/* data_0209f2d8 (CURRENT_GAMEMODE) is 2 while the fall-in-front-of-castle
 * cutscene runs: ContinueKuppaScriptIfNecessary stores 2 only when the
 * script it resumes is FALL_IN_FRONT_OF_CASTLE_CUTSCENE (data_02089608),
 * and EndKuppaScript clears it once no script is left to resume. The
 * credits script does not set it. This int-returning predicate is what
 * makes the ROM materialize 0/1 before testing it. */
static inline int IsCastleFallCutsceneRunning(void) {
    return data_0209f2d8 == 2;
}

// @symbol _ZN7Message7EndTalkEv
void Message::EndTalk() {
    /* Restore music to 0x7f. Slower ramp than PrepareTalk. */
    Sound::ChangeMusicVolume(0x7f, 0x000064cc);
    data_0209d664 = 0;
}

// @symbol _ZN7Message11PrepareTalkEv
void Message::PrepareTalk() {
    /* Duck the music to 0x40 before dialogue. EndTalk is the inverse. */
    Sound::ChangeMusicVolume(0x40, 0x0000c999);
    data_0209d664 = 1;
}

extern "C" {

// @symbol func_0201ab6c
void func_0201ab6c(void) {

    struct S {
        u8 b[8];
    };
    struct S *p = (struct S *)data_0209d6f4;
    u16 v = p->b[4] | (p->b[3] << 8);
    u8 idx;
    switch (v) {
    case 0:
        data_0209d688 = data_0209d6b4;
        break;
    case 1:
        idx = data_0209d68c;
        data_0209d68c = idx + 1;
        data_0209d6e4[idx] = data_0209d6b0;
        Message::AddChar(0x4d);
        data_0209d694 = 1;
        break;
    case 2:
        idx = data_0209d68c;
        data_0209d68c = idx + 1;
        data_0209d6e4[idx] = data_0209d6b4;
        Message::AddChar(0x4d);
        data_0209d694 = 2;
        break;
    case 3:
        idx = data_0209d68c;
        data_0209d68c = idx + 1;
        data_0209d6e4[idx] = data_0209d6b4;
        Message::AddChar(0x4d);
        data_0209d694 = 3;
        break;
    }
}

// @symbol func_0201aca4
void func_0201aca4(void) {

    struct MsgTextChar {
        u8 unk0;
        u8 unk1;
        u8 unk2;
        u8 indexHi; /* 3 */
        u8 indexLo; /* 4 */
        u8 mode;    /* 5 */
        u8 base;    /* 6 */
    };

    struct MsgTextChar *state = (struct MsgTextChar *)data_0209d6f4;
    u16 idx = state->indexLo | (state->indexHi << 8);
    switch (idx) {
    case 0: {
        u8 mode = state->mode;
        u8 num = 0;
        int leading = 0;
        u8 base = state->base;
        int i;
        switch (mode) {
        case 0:
            num = base - NumStars();
            break;
        case 1:
            num = func_020138dc();
            break;
        case 2:
            num = base - SaveData::NumGlowingRabbitsFound();
            break;
        }
        for (i = 0; i < 3; i++) {
            int q = __aeabi_idiv(num, data_0208ee64[i]);
            if (q != 0 || leading != 0 || i == 2) {
                Message::AddChar(data_0208ee74[q]);
                leading = 1;
            }
            num = num % data_0208ee64[i];
        }
        break;
    }
    case 1:
        break;
    }
}

// @symbol func_0201adac
void func_0201adac(void) {
    /* Calls Message::AddChar twice with the font byte and the next byte.
       Index is data_0209d6f4 bytes [3] and [4], looked up in data_0208ee6c. */

    struct MsgTextChar {
        u8 unk0;
        u8 unk1;
        u8 unk2;
        u8 indexHi; /* offset 3 */
        u8 indexLo; /* offset 4 */
    };
    struct MsgTextChar *state = (struct MsgTextChar *)data_0209d6f4;
    u16 idx = state->indexLo | (state->indexHi << 8);
    Message::AddChar(data_0208ee6c[idx]);
    Message::AddChar((data_0208ee6c[idx] + 1) & 0xff);
}

// @symbol func_0201adfc
void func_0201adfc(void) {
    int n;
    int i;
    int j;
    u32 *p;

    if (data_0209d698 != 0) {
        p = (u32 *)(G2S::GetBG1CharPtr() + 0x4000);
    } else {
        p = (u32 *)(func_02054d88() + 0x4000);
    }

    for (i = 0; i < data_0209d6f0[6] * 2; i++) {
        for (j = 0; j < data_0209d6a8; j++) {
            MultiCopy_Int((int *)(p + 4), (int *)p, 0x10);
            if (i == data_0209d6f0[6] * 2 - 1) {
                p[4] = 0;
                p[5] = 0;
                p[6] = 0;
                p[7] = 0;
            } else {
                MultiCopy_Int((int *)(p + data_0209d6a8 * 8), (int *)(p + 4), 0x10);
            }
            p += 8;
        }
    }

    n = data_0209d6f0[6] * 2;
    if (data_0209d698 != 0) {
        p = (u32 *)(G2S::GetBG1CharPtr() + ((data_0209d6a8 * (n - 1) + 0x200) << 5) + 0x10);
    } else {
        p = (u32 *)(func_02054d88() + ((data_0209d6a8 * (n - 1) + 0x200) << 5) + 0x10);
    }

    {
        int cnt = data_0209d6a4;
        int k;
        u8 pos = data_0209d65c;

        k = 0;
        if (cnt > 0) {
            int base = data_0209d6a0;
            u8 *sp2 = data_0209d710;

            do {
                u8 bit = pos & 7;
                int word = (u32)pos >> 3;
                u32 *q = &p[word << 3];
                volatile u32 *mask = (volatile u32 *)&data_0208eeac[bit];

                p[word << 3] |= *mask & ((volatile u32 *)data_0209d74c)[base];
                q[1] |= *mask & ((volatile u32 *)data_0209d74c)[base + 1];
                q[2] |= *mask & ((volatile u32 *)data_0209d74c)[base + 2];
                q[3] |= *mask & ((volatile u32 *)data_0209d74c)[base + 3];

                if (bit != 0) {
                    volatile u32 *omask = (volatile u32 *)&data_0208ee8c[bit];

                    q[8] = ((volatile u32 *)data_0209d74c)[base] & *omask;
                    q[9] = ((volatile u32 *)data_0209d74c)[base + 1] & *omask;
                    q[10] = ((volatile u32 *)data_0209d74c)[base + 2] & *omask;
                    q[11] = ((volatile u32 *)data_0209d74c)[base + 3] & *omask;
                }

                pos = (u8)(pos + *sp2);
                base += 0x10;
                sp2++;
                k++;
            } while (k < cnt);
        }
    }

    data_0209d6a0 += 3;
}

// @symbol func_0201b100
void func_0201b100(int mode) {
    int base;
    int opcode;
    unsigned int acc;
    int size;
    int startRow;
    int r4addr;
    unsigned char *cursor;
    acc = data_0209d65c;
    base = func_02054d88() + 0x8000;
    startRow = 0;
    size = 0x40;
    for (;;) {
        cursor = data_0209d6f4;
        opcode = *cursor;
        switch (opcode) {
        case 0xfd:
            return;
        case 0xfe:
            data_0209d6f4 = cursor + cursor[1];
            continue;
        case 0xff:
            return;
        }
        if (mode != 0) {
            r4addr = (int)G2::GetBG2CharPtr() + (((data_0209d674 + 0x280) + (acc >> 3)) << 5);
        } else {
            r4addr = G2S::GetBG0CharPtr() + (((data_0209d6a8 + 0x200) + (acc >> 3)) << 5);
        }
        if ((acc & 0xf) == 0) {
            int lo2 = opcode & 0xf;
            int hi2 = opcode & 0xf0;
            int w;
            MultiCopy_Int((int *)(base + (((lo2 << 1) + (hi2 << 2)) << 5)), (int *)r4addr, size);
            w = ((int)data_0209d6b0 + 7) / 8;
            MultiCopy_Int((int *)(base + ((lo2 + ((hi2 << 1) + 0x10)) << 6)),
                          (int *)(r4addr + (w << 5)), size);
        } else {
            int lo;
            int hi;
            int r8addr;
            int sh;
            int r7addr;
            int i;
            int row;
            short rem;
            short val;
            int w8;
            int inv;
            int lshift;
            int rshift;
            lo = opcode & 0xf;
            hi = opcode & 0xf0;
            r8addr = base + (((lo << 1) + (hi << 2)) << 5);
            sh = acc & 7;
            val = (short)(int)data_0208f174[opcode];
            row = startRow;
            w8 = (((int)data_0209d6b0 + 7) / 8) << 3;
            r7addr = r8addr + 0x20;
            lshift = sh << 2;
            inv = 8 - sh;
            rshift = inv << 2;
            for (; row < 0x10; row++) {
                i = (row >> 3) * w8 + (row & 7);
                ((int *)r4addr)[i] |= *(int *)r8addr << lshift;
                rem = val;
                rem -= inv;
                if (rem > 0) {
                    (((int *)r4addr) + i)[8] =
                        (*(unsigned int *)r8addr >> rshift) | (*(int *)r7addr << lshift);
                    rem = rem - 8;
                }
                if (rem > 0)
                    *(int *)((char *)r4addr + (i << 2) + 0x40) = *(unsigned int *)r7addr >> rshift;
                r8addr += 4;
                r7addr += 4;
                if (row == 7) {
                    r8addr += 0x3e0;
                    r7addr += 0x3e0;
                }
            }
        }
        acc = (acc + data_0208f174[opcode]) & 0xff;
        data_0209d6f4 = data_0209d6f4 + 1;
    }
}

#pragma push
#pragma opt_common_subs off
// @symbol func_0201b388
void func_0201b388(int a0) {
    int start;
    int *sl;
    int j;
    u32 sb;
    int i;
    int off;
    u8 n;
    int step;

    if (data_0209d668 != 0) {
        sl = (int *)(G2S::GetBG0CharPtr() + ((data_0209d6c0 * data_0209d6a8 * 2 + 0x200) << 5));
    } else if (data_0209d690 != 0) {
        sl = (int *)(G2S::GetBG0CharPtr() + (data_0209d6e0 << 5));
    } else if (data_0209d698 == 2) {
        sl =
            (int *)(((char *)func_02054ea8()) + ((data_0209d6c0 * data_0209d6a8 * 2 + 0x200) << 5));
    } else if (data_0209d698 == 1) {
        sl = (int *)(((char *)G2S::GetBG1CharPtr()) +
                     ((data_0209d6c0 * data_0209d6a8 * 2 + 0x200) << 5));
    } else {
        sl =
            (int *)(((char *)func_02054d88()) + ((data_0209d6c0 * data_0209d6a8 * 2 + 0x200) << 5));
    }

    start = data_0209d65c;
    n = data_0209d6a4;
    step = (a0 - 1) << 5;

    sb = (u32)(((long long)(int)start));
    for (j = 0; j < 0x10; j++) {
        for (i = 0; i < n; i++) {
            int blk = sb >> 3;
            u8 col = sb & 7;
            int row = blk * 8;
            sl[row] = sl[row] | (data_0208eeac[col] & data_0209d74c[j + i * 16]);
            if (col != 0) {
                (sl + row)[8] = data_0208ee8c[col] & data_0209d74c[j + i * 16];
            }
            sb = (sb + data_0209d710[i]) & 0xff;
        }
        sl++;
        if (j == 7) {
            sl = (int *)(((char *)sl) + step);
        }
        sb = start;
    }

    data_0209d6c0 = data_0209d6c0 + 1;
}
#pragma pop
}

// @symbol _ZN7Message7AddCharEh
void Message::AddChar(u8 c) {
    int i;
    int *ip;
    unsigned int word;
    unsigned int masked;
    int mask;
    int sh;

    if (data_0209d698 == 2)
        ip = (int *)((char *)data_0209d6f8 + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    else if (data_0209d698 == 1)
        ip = (int *)((char *)G2S::GetBG1CharPtr() + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    else
        ip = (int *)((char *)func_02054d88() + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));

    mask = data_0208eecc[data_0209d6c4];
    sh = data_0209d678;
    for (i = 0; i < 0x10; i++) {
        word = *ip++;
        masked = word & mask;
        if (sh != 0) {
            masked = (masked >> ((8 - sh) << 2)) | (masked << (sh << 2));
            data_0209d74c[data_0209d704] = masked;
        } else {
            data_0209d74c[data_0209d704] = masked;
        }
        if (i == 7)
            ip += 0xf8;
        data_0209d704 = data_0209d704 + 1;
    }

    {
        unsigned char width = data_0208f074[c];
        int b = data_0209d6b0;
        int d = data_0209d678;
        data_0209d678 = (d + width) & 7;
        data_0209d710[data_0209d6a4] = width;
        data_0209d6b0 = b + width;
        data_0209d6a4 = data_0209d6a4 + 1;
    }
}

extern "C" {

// @symbol func_0201b6f8
void func_0201b6f8(int mode) {
    int i;
    u8 *p = data_0209d6f4;
    data_0209d6a4 = 0;
    data_0209d6b0 = 0;
    i = 0;
    for (;;) {
        u8 v = p[i];
        i++;
        switch (v) {
        case 0xff:
            data_0209d6bc = 8;
            return;
        case 0xfd:
            data_0209d6bc = 2;
            return;
        case 0xfe:
            break;
        default:
            if (mode == 0)
                data_0209d6b0 = data_0209d6b0 + data_0208f074[v];
            else
                data_0209d6b0 = data_0209d6b0 + data_0208f174[v];
            data_0209d6a4 = data_0209d6a4 + 1;
            break;
        }
    }
}

// @symbol func_0201b7cc
void func_0201b7cc(void) {
    int added = 0;

    data_0209d678 = data_0209d65c & 7;
    if (data_0209d6b8 == 0) {
        volatile int fill = 0;
        data_0209d704 = 0;
        data_0209d6a4 = 0;
        data_0209d6b0 = 0;
        MultiStore_Int(fill, data_0209d74c, 0xf00);
    }

    while (1) {
        u8 *cur;
        u8 c;
        cur = data_0209d6f4;
        c = *cur;
        switch (c) {
        case 0xff:
            if (added != 0) {
                if (data_0209d69c == 0) {
                    data_0209d6bc = 2;
                } else {
                    data_0209d6bc = 3;
                    data_0209d6a0 = 0;
                }
                data_0209d6b4 = data_0209d6b4 + 1;
            } else {
                if (data_0209d69c != 0) {
                    u8 *p = data_0209d6f0;
                    u8 v = data_0209d6b4;
                    if (v < p[6]) {
                        data_0209d6b4 = v + 1;
                        data_0209d6a0 = 0;
                        data_0209d6bc = 3;
                    } else {
                        u8 m = data_0209d694;
                        if (m == 1) {
                            data_0209d6bc = 4;
                            data_0209d68c = 0;
                        } else if (m == 2) {
                            data_0209d6bc = 6;
                            data_0209d68c = 0;
                        } else if (m == 3) {
                            data_0209d6bc = 5;
                            data_0209d68c = 0;
                        } else {
                            data_0209d6bc = 8;
                        }
                    }
                } else {
                    u8 m = data_0209d694;
                    if (m == 1) {
                        data_0209d6bc = 4;
                        data_0209d68c = 0;
                    } else if (m == 2) {
                        data_0209d6bc = 6;
                        data_0209d68c = 0;
                    } else if (m == 3) {
                        data_0209d6bc = 5;
                        data_0209d68c = 0;
                    } else {
                        data_0209d6bc = 8;
                    }
                }
            }
            return;

        case 0xfe: {
            u8 idx = cur[2];
            data_0209d700 = idx;
            (MSG_GEN_TEXT_FUNCS + idx)[-1]();
            data_0209d6f4 = data_0209d6f4 + data_0209d6f4[1];
            continue;
        }

        case 0xfd:
            if (data_0209d69c != 0) {
                u8 *p = data_0209d6f0;
                u8 v = data_0209d6b4;
                if (v != p[6]) {
                    data_0209d6b4 = v + 1;
                    data_0209d6bc = 3;
                    data_0209d6a0 = 0;
                    if (data_0209d6b4 != p[6]) {
                        data_0209d6f4 = cur + 1;
                    }
                    return;
                }
                data_0209d6bc = 7;
                return;
            } else {
                u8 *p = data_0209d6f0;
                u8 v = data_0209d6b4;
                if (v != p[6]) {
                    data_0209d6b4 = v + 1;
                    if (data_0209d6b4 != p[6]) {
                        data_0209d6f4 = cur + 1;
                    }
                    data_0209d6bc = 2;
                    return;
                }
                data_0209d6bc = 7;
                return;
            }

        default:
            Message::AddChar(c);
            added = 1;
            data_0209d6f4 = data_0209d6f4 + 1;
        }
    }
}
}

// @symbol _ZN7Message12UpdateWindowEv
int Message::UpdateWindow() {
    if (data_0209d670 == 0) {
        if (data_0209d658 > data_0209d650 || data_0209d64c > data_0209d6cc) {
            int t;
            s16 v;
            int mask;

            data_0209d680++;
            t = data_0209d680 * (data_0209d6d0 - data_0209d650 + 1);
            t = data_0209d6d0 - t / 8;
            v = t;
            if (v < 0)
                data_0209d658 = 0;
            else
                data_0209d658 = t;
            t = data_0209d680 * (data_0209d6c8 - data_0209d6cc + 1);
            data_0209d64c = data_0209d6c8 - t / 8;
            mask = 0;
            if (data_0209d658 <= data_0209d650) {
                data_0209d658 = data_0209d650;
                mask = 1;
            }
            if (data_0209d64c <= data_0209d6cc) {
                mask |= 2;
                data_0209d64c = data_0209d6cc;
            }
            if (mask == 3)
                data_0209d6bc = data_0209d6ac;

            if (data_0209d698 != 0) {
                *(volatile u16 *)0x4001040 = ((data_0209d658 << 8) & 0xff00) |
                                             ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff);
                *(volatile u16 *)0x4001044 = ((data_0209d64c << 8) & 0xff00) |
                                             ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff);
            } else {
                *(volatile u16 *)0x4000040 = ((data_0209d658 << 8) & 0xff00) |
                                             ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff);
                *(volatile u16 *)0x4000044 = ((data_0209d64c << 8) & 0xff00) |
                                             ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff);
            }
            return 0;
        }
        return 1;
    }

    if (data_0209d658 < data_0209d6d0 && data_0209d64c < data_0209d6c8) {
        int t;
        int t2;

        data_0209d680--;
        t = data_0209d680 * (data_0209d6d0 - data_0209d650 + 1);
        t2 = data_0209d680 * (data_0209d6c8 - data_0209d6cc + 1);
        data_0209d658 = data_0209d6d0 - t / 8;
        data_0209d64c = data_0209d6c8 - t2 / 8;
        data_0209d658 += 7;
        data_0209d64c += 5;
        if (data_0209d658 >= data_0209d6d0)
            data_0209d658 = data_0209d6d0;
        if (data_0209d64c >= data_0209d6c8)
            data_0209d64c = data_0209d6c8;

        if (data_0209d698 != 0) {
            *(volatile u16 *)0x4001040 =
                ((data_0209d658 << 8) & 0xff00) | ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff);
            *(volatile u16 *)0x4001044 =
                ((data_0209d64c << 8) & 0xff00) | ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff);
        } else {
            *(volatile u16 *)0x4000040 =
                ((data_0209d658 << 8) & 0xff00) | ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff);
            *(volatile u16 *)0x4000044 =
                ((data_0209d64c << 8) & 0xff00) | ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff);
        }
        return 0;
    }

    data_0209b454 &= ~0x80000000;
    if (func_0201fb4c() != 0) {
        Sound::ChangeMusicVolume(0x7f, 0x000064cc);
    }
    data_0209d660 = 0;
    data_0209d670 = 0;
    if (data_0209d698 != 0) {
        data_0209d454 &= ~2;
        *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
        *(volatile u16 *)0x4001050 = 0;
        *(volatile u16 *)0x4001048 &= ~0x3f;
        *(volatile u16 *)0x400104a &= ~0x3f;
        *(volatile u32 *)0x4001000 &= ~0xe000;
    } else {
        data_0209d45c &= ~8;
        *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | (data_0209d45c << 8);
        *(volatile u16 *)0x4000050 = 0;
        *(volatile u16 *)0x4000048 &= ~0x3f;
        *(volatile u16 *)0x400004a &= ~0x3f;
        *(volatile u32 *)0x4000000 &= ~0xe000;
    }
    return 0;
}

// @symbol _ZN7Message6UpdateEv
void Message::Update() {
    switch (data_0209d6bc) {
    case 1: {
        if (!IsCastleFallCutsceneRunning()) {
            data_0209d65c = 0;
        } else {
            func_0201b6f8(0);
            data_0209d65c = (u8)((*(u16 *)(data_0209d6f0 + 4) - data_0209d6b0) >> 1);
        }
        func_0201b7cc();
        return;
    }
    case 2: {
        func_0201b388(data_0209d6a8);
        data_0209d6bc = 1;
        return;
    }
    case 3: {
        data_0209d65c = 0;
        func_0201adfc();
        data_0209d6a0 += 1;
        if (data_0209d6a0 != 0x10) {
            return;
        }
        data_0209d6bc = 1;
        data_0209d65c = 0;
        func_0201b7cc();
        return;
    }
    case 4: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209d684 = data_0209d68c + 1;
            data_0209d670 = 1;
            return;
        }
        t = *(s16 *)((u8 *)data_0209f4a2 + gActivePlayerSlot * 0x18);
        if (t > 0x80) {
            if (data_0209d68c == 0) {
                func_02012790(0x96);
            }
            data_0209d68c = 1;
        } else if (t < -0x80) {
            if (data_0209d68c != 0) {
                func_02012790(0x96);
            }
            data_0209d68c = 0;
        } else if (*((u8 *)data_0209caa0 + 0x42) == 0 &&
                   (*(u16 *)((u8 *)data_020a0e5a + gActivePlayerSlot * 4) & 0x30)) {
            data_0209d68c ^= 1;
            func_02012790(0x96);
        }
        {
            int r5 = data_0209d6e4[data_0209d68c] + data_0209d650 + 0xC;
            int r4 = data_0209d6cc + (data_0209d688 * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0,
                            -1);
            } else {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0,
                            -1);
            }
            return;
        }
    }
    case 5: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209d684 = data_0209d68c + 1;
            data_0209d670 = 1;
            return;
        }
        t = *(s16 *)((u8 *)data_0209f4a4 + gActivePlayerSlot * 0x18);
        if (t > 0x80) {
            if (data_0209d68c == 0) {
                func_02012790(0x96);
            }
            data_0209d68c = 1;
        } else if (t < -0x80) {
            if (data_0209d68c != 0) {
                func_02012790(0x96);
            }
            data_0209d68c = 0;
        } else if (*((u8 *)data_0209caa0 + 0x42) == 0 &&
                   (*(u16 *)((u8 *)data_020a0e5a + gActivePlayerSlot * 4) & 0xC0)) {
            data_0209d68c ^= 1;
            func_02012790(0x96);
        }
        {
            int r5 = data_0209d650 + 0xC;
            int r4 = data_0209d6cc + (data_0209d6e4[data_0209d68c] * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0,
                            -1);
            } else {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0,
                            -1);
            }
            return;
        }
    }
    case 6: {
        int touchBegan;
        int idx;
        u8 f0;
        if (IsButtonInputValid() != 0) {
            data_0209d684 = data_0209d68c + 1;
            data_0209d670 = 1;
            return;
        }
        idx = gActivePlayerSlot;
        /* Stylus down with the edge bit set: the touch began this frame.
           The ROM keeps this 0/1 in a register; an inline predicate folds
           it away. */
        touchBegan = 0;
        f0 = gTouchHeld[idx * 4];
        if (f0 != 0 && gTouchEdge[idx * 4] != 0)
            touchBegan = 1;
        if (touchBegan != 0) {
            data_0209d6d8 = ((u8(*)[4])gTouchY)[idx][0];
            data_0209d6d8 -= (s16)(data_0209d68c * 8);
        } else if (f0 != 0) {
            s16 debval = ((u8(*)[4])gTouchY)[idx][0];
            data_0209d6dc = debval - data_0209d6d8;
            if (data_0209d6dc > 0x10) {
                data_0209d6dc = 0x10;
                data_0209d6d8 = debval - 0x10;
            } else if (data_0209d6dc < 0) {
                data_0209d6dc = 0;
                data_0209d6d8 = debval;
            }
            switch (data_0209d68c) {
            case 0:
                if (data_0209d6dc > 0 && data_0209d6dc >= 8) {
                    data_0209d68c += 1;
                    func_02012790(0x96);
                }
                break;
            case 1:
                if (data_0209d6dc > 0) {
                    if (data_0209d6dc >= 0x10) {
                        data_0209d68c += 1;
                        func_02012790(0x96);
                    } else if (data_0209d6dc <= 6) {
                        data_0209d68c -= 1;
                        func_02012790(0x96);
                    }
                }
                break;
            case 2:
                if (data_0209d6dc > 0 && data_0209d6dc <= 0xE) {
                    data_0209d68c -= 1;
                    func_02012790(0x96);
                }
                break;
            }
        } else if (*((u8 *)data_0209caa0 + 0x42) == 0) {
            u16 mask = data_020a0e5a[idx * 2];
            if (mask & 0xC0) {
                u8 target = (mask & 0x40) ? 0 : 2;
                if (data_0209d68c != target) {
                    if (target != 0) {
                        data_0209d68c += 1;
                    } else {
                        data_0209d68c -= 1;
                    }
                    func_02012790(0x96);
                }
            }
        }
        {
            int r5 = data_0209d650 + 0xC;
            int r4 = data_0209d6cc + (data_0209d6e4[data_0209d68c] * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0,
                            -1);
            } else {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0,
                            -1);
            }
            return;
        }
    }
    case 7: {
        if (((data_0209caa0[2] & 0x80) && !IsCastleFallCutsceneRunning() &&
             IsButtonInputValid() != 0) ||
            data_0209d66c != 0) {
            data_0209d6f4 += 1;
            data_0209d69c = 1;
            data_0209d6b4 = 0;
            data_0209d66c = 0;
            data_0209d6bc = 1;
            func_02012790(0x14);
        }
        if (!(data_0209caa0[2] & 0x80)) {
            return;
        }
        {
            int r5 = ((data_0209d6d0 * 2) - 6) - data_0209d650;
            int r4 = ((data_0209d6c8 * 2) + 2) - data_0209d6cc;
            if (!(data_020a0db0 & (0x10 / data_0208ee44))) {
                return;
            }
            if (data_0209d6c4 != 0) {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c390, r5, r4, -1, -1, 0x1000, 0x1000, 0,
                            -1);
            } else {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c398, r5, r4, -1, -1, 0x1000, 0x1000, 0,
                            -1);
            }
            return;
        }
    }
    case 8: {
        if (((data_0209caa0[2] & 0x80) && !IsCastleFallCutsceneRunning() &&
             IsButtonInputValid() != 0) ||
            data_0209d66c != 0) {
            data_0209d670 = 1;
            data_0209d66c = 0;
            data_0209d6bc = 9;
            if (IsCastleFallCutsceneRunning()) {
                return;
            }
            if (data_0209d6c4 != 0) {
                func_02012790(0x21);
                return;
            }
            func_02012790(5);
        }
        return;
    }
    }
}
