//cpp
/* VS-mode text. LoadFont3D brings in the font, LoadTextVS picks the language
 * table, and the display entry points draw into the main and sub backgrounds.
 *
 * This is the single-player Message text code built a second time for the VS
 * mode, against its own globals. With relocations masked, nine functions here
 * are word-identical to their single-player twins, in the same order:
 * func_02030fa8 (func_0200f0bc), func_02031154 (func_0201ab6c), func_020313d8
 * (func_0201adac), AddCharVS (Message::AddChar), func_02031e00
 * (func_0201b7cc), SetTextGlobalsVS (Message::SetTextGlobals), func_02033a80
 * (func_0201cfb8), func_02034098 (func_0201d590) and Message_DrawCenteredLineVS
 * (Message_DrawCenteredLine). The other functions are VS variants of other
 * sizes. Twins take the single-player name plus `VS` where that name exists.
 *
 * leftovers:
 * - defer_codegen is off so this file emits in source order. The per-function
 *   optimization pragmas are brackets.
 * - OAM::Render stays a local extern. Its scale arguments are Fix12<int> by
 *   value.
 * - data_0209fc78 is a volatile macro in func_02033464 only. decl_common.h
 *   spells that byte plain, and the other functions in this file read it plain.
 * - data_0209f4a4 is s16[] in decl_common.h. func_020326ac indexes it with a
 *   byte stride, so the cast stays at that use.
 * - SetTextGlobalsVS keeps the r2/r3 temps. One expression reorders the load
 *   of *(r2 + 4) against the store of r3.
 * - Address-named helpers stay address-named.
 */

#pragma defer_codegen off
#include "types.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "Message.h"
#include "G2x.h"
#include "G2.h"

struct OamAttr;

extern "C" {
extern u8 data_0209fc6c;
extern u8 data_0209fc70;
extern u8 data_0209fc74;
extern u8 data_0209fc7c;
extern u8 data_0209fc80;
extern u8 data_0209fc84;
extern u8 data_0209fc88;
extern u8 data_0209fc8c;
extern u8 data_0209fc90;
extern u8 data_0209fc98;
extern u8 data_0209fca0;
extern u8 data_0209fca4;
extern u8 data_0209fca8;
extern u8 data_0209fcac;
extern u8 data_0209fcb0;
extern u8 data_0209fcb4;
extern u8 data_0209fcb8;
extern u8 data_0209fcbc;
extern u8 data_0209fcc0;
extern u8 data_0209fcc4;
extern u8 data_0209fcc8;
extern u8 data_0209fccc;
extern u8 data_0209fcd0;
extern u8 data_0209fcd4;
extern u8 data_0209fcd8;
extern u8 data_0209fce0;
extern u8 data_0209fce4;
extern u8 data_0209fcf4[];
extern s16 data_0209fce8;
extern s16 data_0209fcec;
extern s16 data_0209fcf0;
extern int *data_0209fcf8;
extern int data_0209fcfc;
extern int data_0209fd00;
extern int data_0209fd04;
extern int data_0209fd08;
extern u8 *data_0209fd0c;
extern void *data_0209fd10;
extern int data_0209fd14;
extern int data_0209fd18;
extern int data_0209fd1c;
extern u8 data_0209fd20[];
extern u32 data_0209fd5c[];
extern u8 data_02092818[];
extern int data_0209285c[];
extern void (*data_02092830[])(void);
extern u8 data_0208f074[];
extern u8 data_0208f174[];
extern s32 data_0208ee44;
extern int data_0209caa0[3];
extern u32 data_0209b454;
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern s16 data_0209f4a2[];
extern u16 data_020a0e5a[];
extern s32 data_020a0db0;
extern u8 data_ov002_0210c390;
extern u8 data_ov002_0210c398;
extern u8 data_ov002_0210c3a0;
extern u8 data_ov002_0210c3a8;
extern int data_0209289c;
extern int data_02092d3c;
extern int data_0209325c;
extern int data_020937bc;
extern int data_02093d7c;

extern u16 data_02075b14[];
extern u16 data_02075d14[];
extern u16 data_02092808[];
extern u8 data_02092810[];
extern u8 data_02092824[];
extern u32 data_0209283c[];
extern u32 data_0209287c[];

extern void *func_0203d8a0(int idx);
extern u8 NumStars(void);
extern int func_020138dc(void);
extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void DecompressLZ16(void *src, void *dst);
extern int LoadFile(int handle);
/* local extern: Fix12 wall -- the by-value Fix12<int> form cannot be spelled
   from a header; the literal name keeps the def's own scalar params. */
extern void OamRender10_Message3(int sub, struct OamAttr *attr_, int xOff, int yOff, int palette,
                                 int priority, int scaleX, int scaleY, int rotation, int mode);
struct OAM {
    static void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
};

void func_02031cd4(void *arg);
void func_02031e00(void);
int func_020321fc(void);
void func_020326ac(void);
void func_02032f54(void);
void func_02032f9c(int a);
void func_020331fc(int a0, int a1, int a2);
void func_02033390(int sub);
void func_02033464(int arg0, int arg1, int arg2);
void func_020337ec(void);
void func_020338b0(int a, int b, short c, int d);
void func_02033a80(int arg0);
void func_02033bb8(int param0);
void func_02033e50(int p0, int p1);
void func_02034098(void);
void func_020345b0(int arg0);
void LoadFont3D(void);
}

/* The current message's 8-byte table entry (data_0209fd00 points into the
 * table at data_0209fd08, idx * 8). +4 is the box width in pixels
 * ((width + 7) / 8 tile columns into data_0209fc78) and +6 the number of text
 * lines; each line is two tile rows. Only the fields the readers touch are
 * spelled out. */
struct MessageEntry {
    s32 textOffset; /* 0x0 */
    u16 width;      /* 0x4 */
    u8 lineCount;   /* 0x6 */
};

/* The text cursor's current control record (data_0209fd0c). Bytes 3 and 4 are
 * a big-endian argument; 5 and 6 are the number-format mode and base. */
struct MsgTextChar {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 indexHi; /* 3 */
    u8 indexLo; /* 4 */
    u8 mode;    /* 5 */
    u8 base;    /* 6 */
};

inline bool CharDiffers(u16 a, u16 b) {
    return a != b;
}

// @symbol func_02030fa8
extern "C" s32 func_02030fa8(void) {
    s32 lang = GetOwnerLanguage();
    switch (lang) {
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
        return 4;
    default:
        return 0;
    }
}

// @symbol func_02031028
extern "C" void func_02031028(int arg) {
    u16 *p;
    int i;
    int j;
    u8 *out;
    int n;
    u16 val;
    p = (u16 *)func_0203d8a0(data_0209fc84);
    i = 0;
    data_0209fca0 = ((u8 *)p)[1];
    n = data_0209fca0;
    if (n <= 0) {
        return;
    }
    out = data_02092818;
    do {
        val = (p + i)[1];
        j = 0;
        while (CharDiffers(data_02075b14[j], val) && CharDiffers(data_02075d14[j], val) &&
               j < 0x100) {
            j++;
        }

        if (j == 0x100) {
            *out = 0x47;
        } else {
            *out = j;
        }
        if (arg >= 0) {
            if (arg == 0) {
                data_0209fc88 += data_0208f074[*out];
            } else {
                data_0209fc88 += data_0208f174[*out];
            }
            data_0209fcd8 += 1;
        }
        i++;
        out++;
    } while (i < n);
}

// @symbol func_02031154
extern "C" void func_02031154(void) {
    struct S {
        u8 b[8];
    };
    struct S *p = (struct S *)data_0209fd0c;
    u16 v = p->b[4] | (p->b[3] << 8);
    u8 idx;
    switch (v) {
    case 0:
        data_0209fc80 = data_0209fcbc;
        break;
    case 1:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcd8;
        Message::AddCharVS(0x4d);
        data_0209fca4 = 1;
        break;
    case 2:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcbc;
        Message::AddCharVS(0x4d);
        data_0209fca4 = 2;
        break;
    case 3:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcbc;
        Message::AddCharVS(0x4d);
        data_0209fca4 = 3;
        break;
    }
}

// @symbol func_0203128c
extern "C" void func_0203128c(void) {
    u8 *g;
    int mode;
    int sub;
    int num;
    int seen;
    int i;
    int q;
    int cnt;
    int n2;
    g = data_0209fd0c;
    mode = (u16)(g[4] | (g[3] << 8));
    switch (mode) {
    case 0:
        sub = g[5];
        num = 0;
        seen = num;
        cnt = g[6];
        switch (sub) {
        case 0:
            num = (cnt - NumStars()) & 0xff;
            break;
        case 1:
            num = func_020138dc() & 0xff;
            break;
        }
        for (i = 0; i < 3; i++) {
            q = num / data_02092808[i];
            if (q != 0 || seen != 0 || i == 2) {
                Message::AddCharVS(data_02092824[q]);
                seen = 1;
            }
            num = (num % data_02092808[i]) & 0xff;
        }
        break;
    case 1:
        func_02031028(-1);
        n2 = 0;
        if ((int)data_0209fca0 > 0) {
            u8 *pp = data_02092818;
            do {
                Message::AddCharVS(*pp);
                n2++;
                pp++;
            } while (n2 < (int)data_0209fca0);
        }
        break;
    }
}

// @symbol func_020313d8
extern "C" void func_020313d8(void) {
    struct MsgTextChar *state = (struct MsgTextChar *)data_0209fd0c;
    u16 idx = state->indexLo | (state->indexHi << 8);
    Message::AddCharVS(data_02092810[idx]);
    Message::AddCharVS((data_02092810[idx] + 1) & 0xff);
}

// @symbol func_02031428
#pragma push
#pragma opt_common_subs off
extern "C" void func_02031428(void) {
    int cnt;
    int i;
    int rows;
    int j;
    int base;

    base = func_02054d88() + 0x4000;

    cnt = (int)((MessageEntry *)data_0209fd00)->lineCount << 1;
    i = 0;
    if (cnt > 0) {
        do {
            rows = data_0209fc78;
            j = 0;
            if (rows > 0) {
                do {
                    /* scroll the tile up four pixel rows (4 bytes per 4bpp row) */
                    MultiCopy_Int(&((int *)base)[4], (int *)base, 0x10);
                    if (i == (((int)((MessageEntry *)data_0209fd00)->lineCount << 1) - 1)) {
                        ((int *)base)[4] = 0;
                        ((int *)base)[5] = 0;
                        ((int *)base)[6] = 0;
                        ((int *)base)[7] = 0;
                    } else {
                        int rc2 = data_0209fc78;
                        MultiCopy_Int((int *)(base + rc2 * 0x20), &((int *)base)[4], 0x10);
                    }
                    rows = data_0209fc78;
                    j++;
                    base += 0x20;
                } while (j < rows);
            }
            cnt = (int)((MessageEntry *)data_0209fd00)->lineCount << 1;
            i++;
        } while (i < cnt);
    }

    {
        u32 col;
        int rowIdx;
        u8 rem;
        int count;
        int j;
        int blk;
        int wordIdx;
        u8 *inc;
        u32 *dst;
        int i2;
        int i3;

        dst = (u32 *)(func_02054d88() + (((data_0209fc78 * (cnt - 1)) + 0x200) << 5) + 0x10);
        col = data_0209fc7c;
        count = data_0209fcd8;

        j = 0;
        if (count > 0) {
            rowIdx = data_0209fc70;
            inc = data_0209fd20;
            do {
                rem = col & 7;
                blk = col >> 3;
                wordIdx = blk << 3;
                dst[wordIdx] = (data_0209287c[rem] & data_0209fd5c[rowIdx + 0]) | dst[wordIdx];
                *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 4) & ~0ULL)) =
                    (data_0209287c[rem] & data_0209fd5c[rowIdx + 1]) |
                    *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 4) & ~0ULL));
                i2 = rowIdx + 2;
                *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 8) & ~0ULL)) =
                    (data_0209287c[rem] & data_0209fd5c[i2]) |
                    *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 8) & ~0ULL));
                i3 = rowIdx + 3;
                *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 12) & ~0ULL)) =
                    (data_0209287c[rem] & data_0209fd5c[i3]) |
                    *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 12) & ~0ULL));
                if (rem != 0) {
                    /* the next tile's first four rows (8 words on) */
                    (&dst[wordIdx])[8] = data_0209283c[rem] & data_0209fd5c[rowIdx + 0];
                    (&dst[wordIdx])[9] = data_0209283c[rem] & data_0209fd5c[rowIdx + 1];
                    (&dst[wordIdx])[10] = data_0209283c[rem] & data_0209fd5c[i2];
                    (&dst[wordIdx])[11] = data_0209283c[rem] & data_0209fd5c[i3];
                }
                col = (col + *inc) & 0xff;
                rowIdx += 0x10;
                inc++;
                j++;
            } while (j < count);
        }
    }

    data_0209fc70 = data_0209fc70 + 3;
}
#pragma pop

// @symbol func_020316d8
extern "C" void func_020316d8(int screen, int c, u32 x, void *buf) {
    u32 *chr;
    int pitch;
    if (screen == 1)
        chr = (u32 *)((char *)G2::GetBG2CharPtr() + ((data_0209fcdc + 0x280 + (x >> 3)) << 5));
    else if (screen == 0)
        chr = (u32 *)((char *)G2S::GetBG0CharPtr() + ((data_0209fc78 + 0x200 + (x >> 3)) << 5));
    else
        chr = (u32 *)((char *)func_02054d88() + ((data_0209fc78 + 0x200 + (x >> 3)) << 5));

    if ((x & 0xf) == 0) {
        MultiCopy_Int((int *)((char *)buf + ((((c & 0xf) << 1) + ((c & 0xf0) << 2)) << 5)),
                      (int *)chr, 0x40);
        pitch = (data_0209fc88 + 7) / 8;
        MultiCopy_Int((int *)((char *)buf + (((c & 0xf) + (((c & 0xf0) << 1) + 0x10)) << 6)),
                      (int *)(chr + pitch * 8), 0x40);
    } else {
        int lo;
        int hi;
        u32 *src;
        int m;
        u32 *src2;
        int idx;
        int i;
        short rem;
        short n;
        int stride;
        int k;
        int sh;
        int rsh;
        lo = c & 0xf;
        hi = c & 0xf0;
        src = (u32 *)((char *)buf + (((lo << 1) + (hi << 2)) << 5));
        src2 = src + 8;
        n = (short)(int)data_0208f174[c];
        i = 0;
        stride = (((int)data_0209fc88 + 7) / 8) << 3;
        m = x & 7;
        sh = m << 2;
        k = 8 - m;
        rsh = k << 2;
        for (; i < 0x10; i++) {
            idx = (i >> 3) * stride + (i & 7);
            chr[idx] |= *src << sh;
            rem = n;
            rem -= k;
            if (rem > 0) {
                (chr + idx)[8] = (*src >> rsh) | (*src2 << sh);
                rem = rem - 8;
            }
            if (rem > 0)
                *(u32 *)((char *)chr + (idx << 2) + 0x40) = *src2 >> rsh;
            src += 1;
            src2 += 1;
            if (i == 7) {
                src += 0xf8;
                src2 += 0xf8;
            }
        }
    }
}

// @symbol func_020318a4
extern "C" void func_020318a4(int sl) {
    int base;
    int acc;
    int m;
    int z;
    int i;
    u8 *cmd;
    u8 op;

    acc = data_0209fc7c;
    base = func_02054d88() + 0x8000;
    m = -1;
    z = 0;

    for (;;) {
        cmd = data_0209fd0c;
        op = cmd[0];
        switch (op) {
        case 0xfd:
            return;
        case 0xfe: {
            int param = cmd[2];
            int val;
            data_0209fd18 = param;
            val = (u16)(cmd[4] | (cmd[3] << 8));
            if (param == 1 && val == 1) {
                func_02031028(m);
                i = z;
                if (i < data_0209fca0) {
                    u8 *p = data_02092818;
                    do {
                        func_020316d8(sl, *p, acc, (void *)base);
                        acc = (acc + data_0208f174[*p]) & 0xff;
                        p++;
                        i++;
                    } while (i < data_0209fca0);
                }
            }
            data_0209fd0c = data_0209fd0c + data_0209fd0c[1];
            break;
        }
        case 0xff:
            return;
        default:
            func_020316d8(sl, op, acc, (void *)base);
            acc = (acc + data_0208f174[op]) & 0xff;
            data_0209fd0c = data_0209fd0c + 1;
            break;
        }
    }
}

// @symbol func_020319fc
extern "C" void func_020319fc(int n) {
    u32 *dst;
    int i;
    u32 col;
    int j;
    int col0;
    int skip;
    int count;
    int blk;
    int wordIdx;
    u8 rem;

    if (data_0209fc94 != 0)
        dst = (u32 *)((char *)G2S::GetBG0CharPtr() +
                      (((data_0209fcb0 * data_0209fc78 << 1) + 0x200) << 5));
    else
        dst = (u32 *)((char *)func_02054d88() +
                      (((data_0209fcb0 * data_0209fc78 << 1) + 0x200) << 5));

    skip = (n - 1) << 5;
    col0 = data_0209fc7c;
    count = data_0209fcd8;
    col = col0;

    for (i = 0; i < 0x10; i++) {
        for (j = 0; j < count; j++) {
            blk = col >> 3;
            wordIdx = blk << 3;
            rem = col & 7;
            dst[wordIdx] = (data_0209287c[rem] & data_0209fd5c[i + j * 0x10]) | dst[wordIdx];
            if (rem != 0)
                *(u32 *)((char *)&dst[wordIdx] + 0x20) =
                    *(u32 *)((char *)data_0209283c + (rem << 2)) &
                    *(u32 *)((char *)data_0209fd5c + ((i + j * 0x10) << 2));
            col = (col + data_0209fd20[j]) & 0xff;
        }
        dst++;
        if (i == 7)
            dst = (u32 *)((char *)dst + skip);
        col = col0;
    }
    data_0209fcb0++;
}

// @symbol _ZN7Message9AddCharVSEh
void Message::AddCharVS(u8 c) {
    int i;
    int *ip;
    unsigned int word;
    unsigned int masked;
    int mask;
    int sh;
    sh = data_0209fca8;
    if (sh == 2) {
        ip = (int *)(((char *)data_0209fd10) + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    } else if (sh == 1) {
        ip = (int *)(((char *)G2S::GetBG1CharPtr()) + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    } else {
        ip = (int *)(((char *)func_02054d88()) + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    }
    mask = data_0209285c[data_0209fcd4];
    sh = data_0209fccc;
    for (i = 0; i < 0x10; i++) {
        word = *(ip++);
        masked = word;
        masked = masked & mask;
        if (sh != 0) {
            masked = (masked >> ((8 - sh) << 2)) | (masked << (sh << 2));
            data_0209fd5c[data_0209fd1c] = masked;
        } else {
            data_0209fd5c[data_0209fd1c] = masked;
        }
        if (i == 7) {
            ip += 0xf8;
        }
        data_0209fd1c = data_0209fd1c + 1;
    }

    {
        unsigned char width = data_0208f074[c];
        data_0209fccc = (sh + width) & 7;
        data_0209fd20[data_0209fcd8] = width;
        data_0209fc88 = data_0209fc88 + width;
        data_0209fcd8 = data_0209fcd8 + 1;
    }
}

// @symbol func_02031cd4
extern "C" void func_02031cd4(void *arg) {
    int i;
    u8 *base;
    u8 cmd;
    u8 *entry;
    u32 type;
    u16 v;

    data_0209fcd8 = 0;
    data_0209fc88 = 0;
    i = 0;

    for (;;) {
        base = data_0209fd0c;
        cmd = base[i];
        switch (cmd) {
        case 0xff:
            data_0209fcc8 = 8;
            return;
        case 0xfe:
            entry = base + i;
            type = entry[2];
            data_0209fd18 = type;
            v = entry[4] | (entry[3] << 8);
            if (type == 1 && v == 1)
                func_02031028((int)arg);
            entry = data_0209fd0c + i;
            i += entry[1];
            break;
        case 0xfd:
            data_0209fcc8 = 2;
            return;
        default:
            if (arg == 0)
                data_0209fc88 += data_0208f074[cmd];
            else
                data_0209fc88 += data_0208f174[cmd];
            i += 1;
            data_0209fcd8 += 1;
            break;
        }
    }
}

// @symbol func_02031e00
extern "C" void func_02031e00(void) {
    int flag;
    u8 *p;
    u8 op;
    u8 m;

    data_0209fccc = data_0209fc7c & 7;
    flag = 0;
    if (data_0209fc90 == 0) {
        volatile int zero = 0;
        data_0209fd1c = 0;
        data_0209fcd8 = 0;
        data_0209fc88 = 0;
        MultiStore_Int(zero, (int *)data_0209fd5c, 0xf00);
    }

    for (;;) {
        p = data_0209fd0c;
        op = *p;
        switch (op) {
        case 0xff:
            if (flag != 0) {
                if (data_0209fcc4 == 0) {
                    data_0209fcc8 = 2;
                } else {
                    data_0209fcc8 = 3;
                    data_0209fc70 = 0;
                }
                data_0209fcbc++;
                return;
            } else {
                if (data_0209fcc4 != 0) {
                    u8 *s = (u8 *)data_0209fd00;
                    u8 cc = data_0209fcbc;
                    if (cc < s[6]) {
                        data_0209fcbc = cc + 1;
                        data_0209fc70 = 0;
                        data_0209fcc8 = 3;
                        flag = 0;
                        return;
                    }
                    m = data_0209fca4;
                    if (m == 1) {
                        data_0209fcc8 = 4;
                        data_0209fcd0 = 0;
                    } else if (m == 2) {
                        data_0209fcc8 = 6;
                        data_0209fcd0 = 0;
                    } else if (m == 3) {
                        data_0209fcc8 = 5;
                        data_0209fcd0 = 0;
                    } else {
                        data_0209fcc8 = 8;
                    }
                    return;
                } else {
                    m = data_0209fca4;
                    if (m == 1) {
                        data_0209fcc8 = 4;
                        data_0209fcd0 = 0;
                    } else if (m == 2) {
                        data_0209fcc8 = 6;
                        data_0209fcd0 = 0;
                    } else if (m == 3) {
                        data_0209fcc8 = 5;
                        data_0209fcd0 = 0;
                    } else {
                        data_0209fcc8 = 8;
                    }
                    return;
                }
            }
        case 0xfe: {
            u8 idx = p[2];
            data_0209fd18 = idx;
            (*(void (**)(void))((char *)data_02092830 + (idx << 2) - 4))();
            p = data_0209fd0c;
            data_0209fd0c = p + p[1];
            break;
        }
        case 0xfd:
            if (data_0209fcc4 != 0) {
                u8 *s = (u8 *)data_0209fd00;
                u8 cc = data_0209fcbc;
                if (cc != s[6]) {
                    data_0209fcbc = cc + 1;
                    data_0209fcc8 = 3;
                    data_0209fc70 = 0;
                    if (data_0209fcbc != s[6])
                        data_0209fd0c = p + 1;
                } else {
                    data_0209fcc8 = 7;
                }
                return;
            } else {
                u8 *s = (u8 *)data_0209fd00;
                u8 cc = data_0209fcbc;
                if (cc != s[6]) {
                    data_0209fcbc = cc + 1;
                    if (data_0209fcbc != s[6])
                        data_0209fd0c = p + 1;
                    data_0209fcc8 = 2;
                } else {
                    data_0209fcc8 = 7;
                }
                return;
            }
        default:
            Message::AddCharVS(op);
            data_0209fd0c++;
            flag = 1;
            break;
        }
    }
}

// @symbol func_020321fc
extern "C" int func_020321fc(void) {
    if (data_0209fcb8 == 0) {
        if (data_0209fc8c > data_0209fc6c && data_0209fc74 > data_0209fce0) {
            int t;
            s16 v;
            int mask;

            data_0209fcac++;
            t = data_0209fcac * (data_0209fce4 - data_0209fc6c + 1);
            t = data_0209fce4 - t / 8;
            v = t;
            if (v < 0)
                data_0209fc8c = 0;
            else
                data_0209fc8c = t;
            t = data_0209fcac * (data_0209fcc0 - data_0209fce0 + 1);
            data_0209fc74 = data_0209fcc0 - t / 8;
            mask = 0;
            if (data_0209fc8c <= data_0209fc6c) {
                data_0209fc8c = data_0209fc6c;
                mask = 1;
            }
            if (data_0209fc74 <= data_0209fce0) {
                mask |= 2;
                data_0209fc74 = data_0209fce0;
            }
            if (mask == 3)
                data_0209fcc8 = data_0209fcb4;

            if (data_0209fca8 != 0) {
                *(volatile u16 *)0x4001040 = ((data_0209fc8c << 8) & 0xff00) |
                                             ((data_0209fce4 * 2 - data_0209fc8c + 1) & 0xff);
                *(volatile u16 *)0x4001044 = ((data_0209fc74 << 8) & 0xff00) |
                                             ((data_0209fcc0 * 2 - data_0209fc74 + 1) & 0xff);
            } else {
                *(volatile u16 *)0x4000040 = ((data_0209fc8c << 8) & 0xff00) |
                                             ((data_0209fce4 * 2 - data_0209fc8c + 1) & 0xff);
                *(volatile u16 *)0x4000044 = ((data_0209fc74 << 8) & 0xff00) |
                                             ((data_0209fcc0 * 2 - data_0209fc74 + 1) & 0xff);
            }
            return 0;
        }
        return 1;
    }

    if (data_0209fc8c < data_0209fce4 && data_0209fc74 < data_0209fcc0) {
        int t;
        int t2;

        data_0209fcac--;
        t = data_0209fcac * (data_0209fce4 - data_0209fc6c + 1);
        t2 = data_0209fcac * (data_0209fcc0 - data_0209fce0 + 1);
        data_0209fc8c = data_0209fce4 - t / 8;
        data_0209fc74 = data_0209fcc0 - t2 / 8;
        data_0209fc8c += 7;
        data_0209fc74 += 5;
        if (data_0209fc8c >= data_0209fce4)
            data_0209fc8c = data_0209fce4;
        if (data_0209fc74 >= data_0209fcc0)
            data_0209fc74 = data_0209fcc0;

        if (data_0209fca8 != 0) {
            *(volatile u16 *)0x4001040 =
                ((data_0209fc8c << 8) & 0xff00) | ((data_0209fce4 * 2 - data_0209fc8c + 1) & 0xff);
            *(volatile u16 *)0x4001044 =
                ((data_0209fc74 << 8) & 0xff00) | ((data_0209fcc0 * 2 - data_0209fc74 + 1) & 0xff);
        } else {
            *(volatile u16 *)0x4000040 =
                ((data_0209fc8c << 8) & 0xff00) | ((data_0209fce4 * 2 - data_0209fc8c + 1) & 0xff);
            *(volatile u16 *)0x4000044 =
                ((data_0209fc74 << 8) & 0xff00) | ((data_0209fcc0 * 2 - data_0209fc74 + 1) & 0xff);
        }
        return 0;
    }

    data_0209fc9c = 0;
    data_0209fcb8 = 0;
    if (data_0209fca8 != 0) {
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

// @symbol func_020326ac
#pragma push
#pragma opt_common_subs off
extern "C" void func_020326ac(void) {
    switch (data_0209fcc8) {
    case 0:
    case 8:
        break;
    case 1: {
        data_0209fc7c = 0;
        func_02031e00();
        return;
    }
    case 2: {
        func_020319fc(data_0209fc78);
        data_0209fcc8 = 1;
        return;
    }
    case 3: {
        data_0209fc7c = 0;
        func_02031428();
        data_0209fc70 += 1;
        if (data_0209fc70 != 0x10) {
            return;
        }
        data_0209fcc8 = 1;
        data_0209fc7c = 0;
        func_02031e00();
        return;
    }
    case 4: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        t = *(s16 *)((u8 *)data_0209f4a2 + gActivePlayerSlot * 0x18);
        if (t > 0x80) {
            data_0209fcd0 = 1;
        } else if (t < -0x80) {
            data_0209fcd0 = 0;
        } else if (*((u8 *)data_0209caa0 + 0x42) == 0 &&
                   (*(u16 *)((u8 *)data_020a0e5a + gActivePlayerSlot * 4) & 0x30)) {
            data_0209fcd0 ^= 1;
        }
        {
            int r5 = data_0209fcf4[data_0209fcd0] * 9 + data_0209fc6c + 0xC;
            int r4 = data_0209fce0 + (data_0209fc80 * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, (struct OamAttr *)&data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000,
                            0x1000, 0, -1);
            } else {
                OAM::Render(0, (struct OamAttr *)&data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000,
                            0x1000, 0, -1);
            }
            return;
        }
    }
    case 5: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        t = *(s16 *)((u8 *)data_0209f4a4 + gActivePlayerSlot * 0x18);
        if (t > 0x80) {
            data_0209fcd0 = 1;
        } else if (t < -0x80) {
            data_0209fcd0 = 0;
        } else if (*((u8 *)data_0209caa0 + 0x42) == 0 &&
                   (*(u16 *)((u8 *)data_020a0e5a + gActivePlayerSlot * 4) & 0xC0)) {
            data_0209fcd0 ^= 1;
        }
        {
            int r5 = data_0209fc6c + 0xC;
            int r4 = data_0209fce0 + (data_0209fcf4[data_0209fcd0] * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, (struct OamAttr *)&data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000,
                            0x1000, 0, -1);
            } else {
                OAM::Render(0, (struct OamAttr *)&data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000,
                            0x1000, 0, -1);
            }
            return;
        }
    }
    case 6: {
        int hasf1;
        u8 idx;
        u8 f0;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        idx = gActivePlayerSlot;
        hasf1 = 0;
        f0 = gTouchHeld[idx * 4];
        if (f0 != 0) {
            if (gTouchEdge[idx * 4] != 0) {
                hasf1 = 1;
            }
        }
        if (hasf1 != 0) {
            data_0209fcec = gTouchY[idx * 4];
            data_0209fcec -= (s16)(data_0209fcd0 * 8);
        } else if (f0 != 0) {
            s16 debval = gTouchY[idx * 4];
            data_0209fcf0 = debval - data_0209fcec;
            if (data_0209fcf0 > 0x10) {
                data_0209fcf0 = 0x10;
                data_0209fcec = debval - 0x10;
            } else if (data_0209fcf0 < 0) {
                data_0209fcf0 = 0;
                data_0209fcec = debval;
            }
            switch (data_0209fcd0) {
            case 0:
                if (data_0209fcf0 > 0 && data_0209fcf0 >= 8) {
                    data_0209fcd0 += 1;
                }
                break;
            case 1:
                if (data_0209fcf0 > 0) {
                    if (data_0209fcf0 >= 0x10) {
                        data_0209fcd0 += 1;
                    } else if (data_0209fcf0 <= 6) {
                        data_0209fcd0 -= 1;
                    }
                }
                break;
            case 2:
                if (data_0209fcf0 > 0 && data_0209fcf0 <= 0xE) {
                    data_0209fcd0 -= 1;
                }
                break;
            }
        } else if (*((u8 *)data_0209caa0 + 0x42) == 0) {
            u16 mask = *(u16 *)((u8 *)data_020a0e5a + idx * 4);
            if (mask & 0xC0) {
                u8 target = (mask & 0x40) ? 0 : 2;
                if (data_0209fcd0 != target) {
                    if (target != 0) {
                        data_0209fcd0 += 1;
                    } else {
                        data_0209fcd0 -= 1;
                    }
                }
            }
        }
        {
            int r5 = data_0209fc6c + 0xC;
            int r4 = data_0209fce0 + (data_0209fcf4[data_0209fcd0] * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, (struct OamAttr *)&data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000,
                            0x1000, 0, -1);
            } else {
                OAM::Render(0, (struct OamAttr *)&data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000,
                            0x1000, 0, -1);
            }
            return;
        }
    }
    case 7: {
        if ((data_0209caa0[2] & 0x80) && IsButtonInputValid() != 0) {
            data_0209fd0c += 1;
            data_0209fcc4 = 1;
            data_0209fcbc = 0;
            data_0209fcc8 = 1;
        }
        if (!(data_0209caa0[2] & 0x80)) {
            return;
        }
        {
            int r5 = ((data_0209fce4 * 2) - 6) - data_0209fc6c;
            int r4 = ((data_0209fcc0 * 2) + 2) - data_0209fce0;
            if (!(data_020a0db0 & (0x10 / data_0208ee44))) {
                return;
            }
            if (data_0209fcd4 != 0) {
                OAM::Render(0, (struct OamAttr *)&data_ov002_0210c390, r5, r4, -1, -1, 0x1000,
                            0x1000, 0, -1);
            } else {
                OAM::Render(0, (struct OamAttr *)&data_ov002_0210c398, r5, r4, -1, -1, 0x1000,
                            0x1000, 0, -1);
            }
            return;
        }
    }
    }
}
#pragma pop

// @symbol _ZN7Message16SetTextGlobalsVSEv
/* Splits the VS-mode text blob at 0x0209fd14 into the four pointers the renderer
 * reads. The blob's own header carries the offset from its second section to the
 * third, so only three of the four are fixed strides:
 *
 *     +0x00  header      -> data_0209fd04
 *     +0x20  section A   -> data_0209fcf8, and its word at +4 is the distance
 *                           from A to section B
 *     A + *(A+4)         -> data_0209fcfc
 *     +0x30              -> data_0209fd08
 *
 * The intermediate `r2`/`r3` locals are kept because the load of `*(r2 + 4)` has
 * to happen after r2 is formed and before r3 is stored; collapsing them into one
 * expression reorders the loads.
 */
void Message::SetTextGlobalsVS() {
    int ip = data_0209fd14;
    int r2 = ip + 0x20;
    data_0209fd04 = ip;
    data_0209fcf8 = (int *)r2;
    int r3 = r2 + *(int *)(r2 + 4);
    data_0209fcfc = r3;
    data_0209fd08 = ip + 0x30;
}

// @symbol func_02032f54
extern "C" void func_02032f54(void) {
    if (data_0209fc9c == 0)
        return;
    if (func_020321fc() == 0)
        return;
    func_020326ac();
}

// @symbol func_02032f9c
extern "C" void func_02032f9c(int a) {
    volatile int li;
    volatile unsigned short ls;
    int idx;
    int p, s;
    int v;
    int i;
    u16 *q;
    int v2;
    int w, n;
    u8 c;
    u8 old94;

    data_0209fce8 = 0x20;
    data_0209fc9c = 0;
    func_02034504();

    idx = data_0209fce8;
    v = data_0209fd14;
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    data_0209fd00 = (int)((char *)data_0209fd08 + idx * 8);
    data_0209fd0c = (u8 *)(v + 0x28 + data_0209fcf8[1] + *(int *)((char *)data_0209fd08 + idx * 8));

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int *)p, 0x2000);

    s = (int)G2::GetBG3ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char *)s, 0x800);

    if (a < 0) {
        return;
    }

    data_0209fc84 = (u8)a;
    func_02031cd4(0);

    w = 0x100 - data_0209fc88;
    data_0209fc7c = (u8)((w >> 1) & 7);
    q = (u16 *)((char *)G2::GetBG3ScrPtr() + 0x40) + (u16)(w / 8 >> 1);
    c = data_0209fc7c;
    n = (data_0209fc88 + c + 7) >> 3;
    i = 0;
    if (n > 0) {
        v2 = n + 0x200;
        do {
            q[i] = (u16)(i + 0x200);
            (q + i)[0x20] = (u16)v2;
            v2++;
            i++;
        } while (i < n);
    }

    data_0209fc90 = 1;
    data_0209fcd4 = 4;
    data_0209fccc = c;
    data_0209fcd8 = 0;
    data_0209fc88 = 0;
    old94 = data_0209fc94;
    data_0209fc94 = 0;
    func_02031e00();

    data_0209fc88 = data_0209fc88 + data_0209fc7c;
    func_020319fc((u8)((data_0209fc88 + 7) >> 3));

    data_0209fc94 = old94;
    data_0209fcd4 = 0;
    data_0209fcdc = (u8)(((data_0209fc88 + 7) >> 3) << 1);
}

// @symbol func_020331fc
#pragma push
#pragma opt_strength_reduction off
extern "C" void func_020331fc(int a0, int a1, int a2) {
    volatile int li;
    short *scr;
    int base;
    int cnt;
    int acc;
    int i;
    int old;
    unsigned char *p;

    base = func_02054d88() + 0x8000;
    data_0209fc78 = 0;
    data_0209fc88 = 0;
    data_0209fc84 = a2;

    {
        int cp = (u32)G2S::GetBG0CharPtr() + 0x4000;
        li = 0;
        MultiStore_Int(li, (int *)(cp + (data_0209fc78 << 5)), 0x800);
    }
    scr = (short *)((u32)G2S::GetBG0ScrPtr() + a0 * 2);
    func_02031028(1);

    old = data_0209fc88;
    data_0209fc7c = (((a1 << 3) - old) >> 1) & 7;
    acc = data_0209fc7c;
    cnt = (unsigned char)((a1 - (old + 7) / 8) >> 1);
    data_0209fc88 = old + acc;
    i = 0;
    if (i < data_0209fca0) {
        p = data_02092818;
        do {
            func_020316d8(0, *p, acc, (void *)base);
            acc = (acc + data_0208f174[*p]) & 0xff;
            p++;
            i++;
        } while (i < data_0209fca0);
    }

    scr += cnt;
    {
        int j = 0;
        int v2;
        int n = (data_0209fc88 + 7) / 8;
        if (n > 0) {
            v2 = n + 0x200;
            do {
                scr[j] = j + 0x200;
                *(short *)((char *)&scr[j] + 0x40) = v2;
                v2++;
                j++;
            } while (j < n);
        }
    }
}
#pragma pop

// @symbol func_02033390
extern "C" void func_02033390(int sub) {
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;

    if (sub != 0) {
        volatile u16 fill;
        volatile s32 v;
        int *dst = (int *)(func_02054d88() + 0x4000);
        v = 0;
        MultiStore_Int(v, dst, 0x2000);
        MultiStore16(fill = 0x2ff, (char *)G2::GetBG3ScrPtr(), 0x800);
    } else {
        volatile u16 fill;
        volatile s32 v;
        int *dst = (int *)((int)G2S::GetBG0CharPtr() + 0x4000);
        v = 0;
        MultiStore_Int(v, dst, 0x2000);
        MultiStore16(fill = 0x2ff, (char *)G2S::GetBG0ScrPtr(), 0x800);
    }
}

/* byte-forced: this function accesses data_0209fc78 volatile (reload per use);
   decl_common.h spells it plain. The macro expands each use to a volatile deref
   of the global -- the inner name paints blue and resolves to the extern. */
#define data_0209fc78 (*(volatile u8 *)&data_0209fc78)
// @symbol func_02033464
extern "C" void func_02033464(int arg0, int arg1, int arg2) {
    volatile unsigned short sv0;
    volatile unsigned short sv2;
    volatile int li4;
    volatile int li8;
    volatile int liC;
    u32 t;
    char *d;
    volatile u16 *base;
    u32 x;
    int i;
    int n;
    u8 *p;
    int v1;

    li4 = 0;
    data_0209fd1c = 0;
    data_0209fcb0 = 1;
    data_0209fc9c = 0;
    data_0209fc88 = 0;
    data_0209fcd8 = 0;
    data_0209fc84 = arg1;

    MultiStore_Int(li4, (int *)data_0209fd5c, 0xf00);

    if (arg2 == 0) {
        x = (arg0 * 2 + 8) << 0x15;
        data_0209fc94 = 1;
        t = x >> 16;
        d = (char *)((u32)G2S::GetBG0ScrPtr() + (((x >> 16) & 0xffe0) << 1));
        sv0 = 0x2ff;
        MultiStore16(sv0, d, 0x80);
        if (arg1 < 0)
            return;
        func_02031028(0);
        data_0209fc7c = (u8)((0x100 - data_0209fc88) >> 1);
        n = data_0209fca0;
        data_0209fccc = data_0209fc7c & 7;
        data_0209fc88 = 0;
        data_0209fcd8 = 0;
        i = 0;
        if (n > 0) {
            p = data_02092818;
            do {
                Message::AddCharVS(*p);
                i++;
                p++;
            } while (i < data_0209fca0);
        }
        data_0209fc78 = (u8)(arg0 * 0x18 + data_0209fcdc);
        {
            int cp = (u32)G2S::GetBG0CharPtr() + 0x4000;
            int *dst = (int *)(cp + (data_0209fc78 << 5));
            li8 = 0;
            MultiStore_Int(li8, dst, 0x300);
        }
        base = (volatile u16 *)((u32)G2S::GetBG0ScrPtr() + ((t + (data_0209fc7c >> 3)) << 1));
        {
            u32 ch = data_0209fc78;
            x = ch + 0x200;
            v1 = ch + 0x20c;
        }
        i = 0;
        do {
            base[i] = (u16)x;
            (base + i)[0x20] = (u16)v1;
            i++;
            x++;
            v1++;
        } while (i < 0xc);
        data_0209fc7c &= 7;
        data_0209fc78 = (u8)(data_0209fc78 >> 1);
        data_0209fcb0 = 1;
        func_020319fc(0xc);
    } else {
        data_0209fc94 = 0;
        d = (char *)G2::GetBG3ScrPtr() + (arg0 << 1);
        sv2 = 0x2ff;
        MultiStore16(sv2, d, 0x80);
        func_02031028(0);
        n = data_0209fca0;
        data_0209fc7c = 4;
        data_0209fccc = 4;
        data_0209fc88 = 0;
        data_0209fcd8 = 0;
        i = 0;
        if (n > 0) {
            p = data_02092818;
            do {
                Message::AddCharVS(*p);
                i++;
                p++;
            } while (i < data_0209fca0);
        }
        data_0209fc78 = (u8)(arg1 * 0x18 + data_0209fcdc);
        {
            int cp = func_02054d88() + 0x4000;
            int *dst = (int *)(cp + (data_0209fc78 << 5));
            liC = 0;
            MultiStore_Int(liC, dst, 0x300);
        }
        base = (volatile u16 *)((char *)G2::GetBG3ScrPtr() + ((arg0 + (data_0209fc7c >> 3)) << 1));
        {
            u32 ch = data_0209fc78;
            x = ch + 0x200;
            v1 = ch + 0x20c;
        }
        i = 0;
        do {
            base[i] = (u16)x;
            (base + i)[0x20] = (u16)v1;
            i++;
            x++;
            v1++;
        } while (i < 0xc);
        data_0209fc7c &= 7;
        data_0209fc78 = (u8)(data_0209fc78 >> 1);
        data_0209fcb0 = 1;
        func_020319fc(0xc);
    }
}
#undef data_0209fc78

// @symbol func_020337ec
extern "C" void func_020337ec(void) {
    volatile unsigned short v;
    volatile int z;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    {
        int *dst = (int *)((u32)G2S::GetBG0CharPtr() + 0x4000);
        z = 0;
        MultiStore_Int(z, dst, 0x2000);
    }
    {
        char *dst = (char *)G2S::GetBG0ScrPtr();
        v = 0x2ff;
        MultiStore16(v, dst, 0x800);
    }
    data_0209fce8 = 0x18;
    Message_DrawCenteredLineVS(0x280, 0x20);
    data_0209fcdc = data_0209fc78;
    func_02033464(0, 0, 0);
}

// @symbol func_020338b0
extern "C" void func_020338b0(int a, int b, short c, int d) {
    int p, s;
    volatile int li0, li1;
    volatile unsigned short ls0, ls1;
    int zero = 0;
    data_0209fce8 = c;
    data_0209fc9c = zero;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = zero;
    if (d >= 0)
        data_0209fc84 = d;
    switch (data_0209fce8) {
    case 0x13:
    case 0x15:
        p = (u32)G2S::GetBG0CharPtr() + 0x4000;
        li0 = 0;
        MultiStore_Int(li0, (int *)p, 0x3000);
        s = (u32)G2S::GetBG0ScrPtr();
        ls0 = 0x37f;
        MultiStore16(ls0, (char *)s, 0x800);
        break;
    default:
        p = (u32)G2S::GetBG0CharPtr() + 0x4000;
        li1 = 0;
        MultiStore_Int(li1, (int *)p, 0x2000);
        s = (u32)G2S::GetBG0ScrPtr();
        ls1 = 0x2ff;
        MultiStore16(ls1, (char *)s, 0x800);
        break;
    }
    func_02033e50(a, b);
}

// @symbol _ZN7Message17DisplayVsExitTextEt
void Message::DisplayVsExitText(unsigned short n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209fce8 = n;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    data_0209fcdc = 0;
    p = (u32)G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int *)p, 0x2000);
    s = (u32)G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char *)s, 0x800);
    Message_DrawCenteredLineVS(0x100, 0x20);
    data_0209fce8 = 0x22;
    Message_DrawCenteredLineVS(0x1c0, 0x20);
}

// @symbol func_02033a80
extern "C" void func_02033a80(int arg0) {
    data_0209fce8 = (u16)arg0;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    func_02033e50(0xcf, 0x68);
}

// @symbol _ZN7Message24DisplayOptionsMenuTextVSEs
/* The VS twin of Message::DisplayOptionsMenuText: the same two option lines and
   centred footer, drawn from the VS text tables. */
void Message::DisplayOptionsMenuTextVS(s16 arg) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    p = (u32)G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int *)p, 0x2000);
    s = (u32)G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char *)s, 0x800);
    func_02033a80(arg);
    data_0209fce8 = 0xc;
    data_0209fcb0 = 1;
    func_02033e50(0x18f, 0x28);
    data_0209fce8 = data_0209fce8 + 1;
    func_02033e50(0x197, 0x28);
    {
        u8 old = *(volatile u8 *)&data_0209fc78;
        data_0209fce8 = 8;
        data_0209fc78 = old << 1;
    }
    Message_DrawCenteredLineVS(0x280, 0x20);
}

// @symbol func_02033bb8
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" void func_02033bb8(int param0) {
    volatile int li;
    volatile unsigned short sv;
    int n, m, i;
    short *scr;

    data_0209fce8 = (short)param0;
    data_0209fc9c = 0;
    func_02034504();

    data_0209fc94 = 1;
    data_0209fc78 = 0;
    data_0209fcdc = 0;

    {
        int cp = (u32)G2S::GetBG0CharPtr() + 0x4000;
        li = 0;
        MultiStore_Int(li, (int *)cp, 0x2000);
    }

    {
        int sp2 = (u32)G2S::GetBG0ScrPtr();
        sv = 0x2ff;
        MultiStore16(sv, (char *)sp2, 0x800);
    }

    func_02034098();
    n = data_0209fc88;
    m = (n + 7) / 8;
    scr = (short *)((char *)G2::GetBG2ScrPtr() + 0x40 + (16 - m / 2) * 2);
    i = 0;
    if (m > 0) {
        int v2 = m + 0x280;
        do {
            scr[i] = (short)(i + 0x280);
            *(short *)((char *)&scr[i] + 0x40) = (short)v2;
            v2++;
            i++;
        } while (i < m);
    }
    func_02033e50(0xa9, 0x70);

    data_0209fce8++;
    func_02034098();
    n = data_0209fc88;
    m = (n + 7) / 8;
    {
        int base = (int)G2::GetBG2ScrPtr();
        scr = (short *)(base + 0x840 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209fcdc;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = (short)v1;
            *(short *)((char *)&scr[i] + 0x40) = (short)v2;
            v1++;
            v2++;
            i++;
        } while (i < mm);
    }
    func_02033e50(0x149, 0x70);

    data_0209fce8++;
    func_02034098();
    n = data_0209fc88;
    m = (n + 7) / 8;
    {
        int base = (int)G2::GetBG2ScrPtr();
        scr = (short *)(base + 0x1040 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209fcdc;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = (short)v1;
            *(short *)((char *)&scr[i] + 0x40) = (short)v2;
            v1++;
            v2++;
            i++;
        } while (i < mm);
    }
    func_02033e50(0x1e9, 0x70);

    data_0209fc78 = (unsigned char)(data_0209fc78 << 1);
    data_0209fce8++;
    Message_DrawCenteredLineVS(0x280, 0x20);
}
#pragma pop

// @symbol func_02033e50
#pragma push
#pragma opt_strength_reduction off
extern "C" void func_02033e50(int p0, int p1) {
    int i;
    int idx = data_0209fce8;
    char *base = (char *)data_0209fd08;
    int n = (p1 >> 3) & 0xff;

    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c = (u8 *)(data_0209fd14 + 0x28 + data_0209fcf8[1] + *(int *)(base + idx * 8));
    data_0209fcc8 = 0;

    do {
        func_02031cd4(0);
        idx = data_0209fce8;
        if (idx == 0x13 || idx == 0x15) {
            volatile int li;
            data_0209fc7c = 0xc;
            n = ((data_0209fc88 + 0x13) >> 3) & 0xff;
            {
                int cp = (u32)G2S::GetBG0CharPtr() + 0x4000;
                li = 0;
                MultiStore_Int(li, (int *)(cp + (data_0209fc78 << 6)), n << 6);
            }
            {
                short *scr = (short *)((u32)G2S::GetBG0ScrPtr() + p0 * 2);
                i = 0;
                if (n > 0) {
                    int v1 = (data_0209fc78 << 1) + 0x200;
                    int v2 = n + v1;
                    do {
                        scr[i] = v1;
                        *(short *)((char *)&scr[i] + 0x40) = v2;
                        v1++;
                        v2++;
                        i++;
                    } while (i < n);
                }
            }
            func_02031e00();
            func_020319fc(n);
            data_0209fcb0 = 1;
            data_0209fc78 = data_0209fc78 + n;
            p0 = (unsigned short)(p0 + 0x40);
        } else {
            volatile int li;
            data_0209fc7c = (unsigned char)((p1 - data_0209fc88) >> 1);
            {
                int cp = (u32)G2S::GetBG0CharPtr() + 0x4000;
                li = 0;
                MultiStore_Int(li, (int *)(cp + (data_0209fc78 << 6)), n << 6);
            }
            {
                int v1, v2;
                short *scr = (short *)((u32)G2S::GetBG0ScrPtr() + p0 * 2);
                i = 0;
                if (n > 0) {
                    v1 = (data_0209fc78 << 1) + 0x200;
                    v2 = n + v1;
                    do {
                        scr[i] = v1;
                        *(short *)((char *)&scr[i] + 0x40) = v2;
                        v1++;
                        v2++;
                        i++;
                    } while (i < n);
                }
            }
            func_02031e00();
            func_020319fc(n);
            data_0209fcb0 = 1;
            data_0209fc78 = data_0209fc78 + n;
            p0 = (unsigned short)(p0 + 0x40);
        }
    } while (data_0209fcc8 != 8);
    data_0209fcc8 = 0;
}
#pragma pop

// @symbol func_02034098
extern "C" void func_02034098(void) {
    volatile int li;
    int n;
    int idx = data_0209fce8;
    char *base = (char *)data_0209fd08;
    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c = (u8 *)(data_0209fd14 + 0x28 + data_0209fcf8[1] + *(int *)(base + idx * 8));
    func_02031cd4((void *)1);
    data_0209fc7c = 0;
    n = ((data_0209fc88 + 7) / 8) << 6;
    {
        int p = (int)G2::GetBG2CharPtr() + ((data_0209fcdc + 0x280) << 5);
        li = 0;
        MultiStore_Int(li, (int *)p, n);
    }
    func_020318a4(1);
    data_0209fcdc = data_0209fcdc + (u8)(((data_0209fc88 + 7) / 8) << 1);
}

// @symbol Message_DrawCenteredLineVS
extern "C" void Message_DrawCenteredLineVS(int a0, int a1) {
    volatile int li;
    int n, i, oldc;
    short *scr;
    int idx = data_0209fce8;
    char *base = (char *)data_0209fd08;
    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c = (u8 *)(data_0209fd14 + 0x28 + data_0209fcf8[1] + *(int *)(base + idx * 8));
    func_02031cd4((void *)1);
    oldc = data_0209fc88;
    data_0209fc7c = (((a1 << 3) - oldc) & 0xf) >> 1;
    data_0209fc88 = oldc + data_0209fc7c;
    n = ((data_0209fc88 + 7) / 8) << 6;
    {
        int p = (u32)G2S::GetBG0CharPtr() + 0x4000 + (data_0209fc78 << 5);
        li = 0;
        MultiStore_Int(li, (int *)p, n);
    }
    {
        int sp0 = (u32)G2S::GetBG0ScrPtr() + a0 * 2;
        scr = (short *)(sp0 + ((unsigned short)((a1 - ((oldc + 7) / 8)) >> 1)) * 2);
    }
    for (i = 0; i < (data_0209fc88 + 7) / 8; i++) {
        scr[i] = data_0209fc78 + 0x200 + i;
        (&scr[i])[32] = data_0209fc78 + 0x200 + (data_0209fc88 + 7) / 8 + i;
    }
    func_020318a4(0);
    data_0209fc78 = data_0209fc78 + (u8)(((data_0209fc88 + 7) / 8) << 1);
}

// @symbol _ZN7Message18DisplayPauseTextVSEt
void Message::DisplayPauseTextVS(unsigned short n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209fce8 = n;
    data_0209fc78 = 0;
    data_0209fc94 = 1;
    p = (u32)G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int *)p, 0x2000);
    s = (u32)G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char *)s, 0x800);
    Message_DrawCenteredLineVS(0xc0, 0x20);
    data_0209fce8 = data_0209fce8 + 1;
    Message_DrawCenteredLineVS(0x160, 0x20);
    data_0209fce8 = 0x22;
    Message_DrawCenteredLineVS(0x200, 0x20);
}

// @symbol _ZN7Message21DisplayDontSaveTextVSEt
/* The VS twin of Message::DisplayDontSaveText: message `n` in the box, then the
   two centred answer lines. */
void Message::DisplayDontSaveTextVS(unsigned short n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    p = (u32)G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int *)p, 0x2000);
    s = (u32)G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char *)s, 0x800);
    data_0209fce8 = n;
    func_02033e50(0x105, 0xb0);
    data_0209fce8 = 0xf;
    data_0209fc78 = data_0209fc78 << 1;
    Message_DrawCenteredLineVS(0x285, 0xa);
    data_0209fce8 = data_0209fce8 + 1;
    Message_DrawCenteredLineVS(0x291, 0xa);
}

// @symbol func_02034504
extern "C" void func_02034504(void) {
    data_0209fcb0 = 0;
    data_0209fcbc = 0;
    data_0209fcc4 = 0 * 0;
    data_0209fcc8 = 1;
    data_0209fca4 = 0;
    data_0209fcd0 = 0;
    data_0209fc98 = 0;
    data_0209fc90 = 0;
    data_0209fcd4 = 0;
    data_0209fc94 = 0;
    data_0209fc78 = 0;
    data_0209fc7c = 0;
}

// @symbol func_020345b0
extern "C" void func_020345b0(int arg0) {
    void *scr;
    s32 i;
    s32 cols;
    s32 last;
    u8 t;
    volatile s32 li;
    volatile u16 ls;

    if (*(u16 *)((char *)data_0209fcf8 + 8) <= (u16)arg0) {
        return;
    }

    data_0209fce8 = (s16)arg0;
    func_02034504();

    data_0209fc9c = 1;
    data_0209fc8c = 0x7f;
    data_0209fc74 = 0x87;
    data_0209fcd4 = 0;
    data_0209fd00 = (int)((char *)data_0209fd08 + data_0209fce8 * 8);
    data_0209fd0c = (u8 *)(data_0209fd14 + 0x28 + *(s32 *)((char *)data_0209fcf8 + 4) +
                           *(s32 *)((char *)data_0209fd08 + data_0209fce8 * 8));
    data_0209fc78 = (u8)((*(u16 *)((char *)data_0209fd00 + 4) + 7) / 8);
    data_0209fce4 = 0x7f;
    data_0209fc6c = (u8)(((0x20 - (data_0209fc78 + 2)) / 2) * 8);
    data_0209fcc0 = 0x63;
    data_0209fce0 = (u8)(0x64 - (((*(u8 *)((char *)data_0209fd00 + 6) * 2 + 1) * 8) / 2));
    data_0209fc8c = 0x7f;
    data_0209fc74 = 0x63;
    data_0209fcac = 0;
    data_0209fc7c = 0;
    data_0209b454 |= 0x800000;

    {
        void *p = (char *)func_02054d88() + 0x4000;
        li = 0;
        MultiStore_Int(li, (int *)p, 0x3000);
    }

    {
        void *s = G2::GetBG3ScrPtr();
        ls = 0x37f;
        MultiStore16(ls, (char *)s, 0x800);
    }

    scr = (char *)G2::GetBG3ScrPtr() + 0x42;
    SetBg3Offset(-(int)data_0209fc6c, 4 - (int)data_0209fce0);

    i = 0;
    cols = data_0209fc78;
    if ((s32)(cols * (*(u8 *)((char *)data_0209fd00 + 6)) * 2) > 0) {
        last = cols - 1;
        do {
            s32 rem = i % cols;
            *(u16 *)((char *)scr + rem * 2) = (u16)(i + 0x200);
            if (last == rem) {
                scr = (char *)scr + 0x40;
            }
            i++;
        } while (i < (s32)(cols * (*(u8 *)((char *)data_0209fd00 + 6)) * 2));
    }

    t = data_0209fcc8;
    if (t < 4) {
        do {
            func_020326ac();
            t = data_0209fcc8;
        } while (t < 4);
    }
    data_0209fcb4 = t;

    G2x::SetBlendBrightness((volatile u16 *)0x4000050, (data_0209d45c ^ 0x10) | 0x20, -8);

    *(volatile u16 *)0x4000048 = (*(volatile u16 *)0x4000048 & ~0x3f) | (data_0209d45c | 8) | 0x20;
    *(volatile u16 *)0x400004a = (*(volatile u16 *)0x400004a & ~0x3f) | 0x17;
    *(volatile s16 *)0x4000040 =
        ((data_0209fc8c << 8) & 0xff00) | (u8)(((data_0209fce4 * 2) - data_0209fc8c) + 1);
    *(volatile s16 *)0x4000044 =
        ((data_0209fc74 << 8) & 0xff00) | (u8)(((data_0209fcc0 * 2) - data_0209fc74) + 1);

    data_0209fcc8 = 0;
    *(volatile s32 *)0x4000000 = (*(volatile s32 *)0x4000000 & ~0xe000) | 0x2000;
    data_0209d45c |= 8;
    *(volatile s32 *)0x4000000 = (*(volatile s32 *)0x4000000 & ~0x1f00) | (data_0209d45c << 8);
}

// @symbol LoadFont3D
extern "C" void LoadFont3D(void) {
    void *r5;
    void *r4;

    r5 = (void *)LoadFile(0x980e);
    MultiCopy_Int((int *)r5, (int *)func_02054d88(), 0x4000);

    r4 = (void *)LoadFile(0x980d);
    DecompressLZ16(r4, (u8 *)func_02054d88() + 0x8000);

    Deallocate(r4);
    Deallocate(r5);

    Message::LoadTextVS();

    data_0209fc9c = 0;
    data_0209fce8 = -1;
}

// @symbol _ZN7Message10LoadTextVSEv
/* Points the VS-mode text blob at the table for the console's language, then hands
 * to SetTextGlobalsVS to split it into the four pointers the renderer reads.
 * The order is a descending chain rather than a switch, and the final `else` is the
 * default rather than a sixth language -- languages 5..2 have their own tables and
 * everything else falls back to 0x02092d3c. */
void Message::LoadTextVS() {
    if (GetOwnerLanguage() == 5) {
        data_0209fd14 = (int)&data_020937bc;
    } else if (GetOwnerLanguage() == 4) {
        data_0209fd14 = (int)&data_0209325c;
    } else if (GetOwnerLanguage() == 3) {
        data_0209fd14 = (int)&data_02093d7c;
    } else if (GetOwnerLanguage() == 2) {
        data_0209fd14 = (int)&data_0209289c;
    } else {
        data_0209fd14 = (int)&data_02092d3c;
    }
    Message::SetTextGlobalsVS();
}
