//cpp
/* unit020b797c -- ov007 unit 0x020b797c..0x020b8f50: 27 functions, .text only.
 *
 * The title menu's message window and its text renderer. ov007 is the title
 * and file-select menu program that dScDSMT_c hosts; outside dScDSMT_c it
 * has no ROM RTTI or vtable, so no class name is claimed and the file keeps
 * the unit<addr> name.
 *
 * What the code does. The text renderer draws a string of the menu's
 * message bank into one of three 4bpp character planes
 * (data_ov007_02103478[0..2]) one glyph at a time: func_ov007_020b8d48 blits
 * one 8x16 glyph of the glyph sheet at the cursor (column
 * data_ov007_02103464 in pixels, line data_ov007_02103468 in tile rows) in
 * the current colour (data_ov007_02102dd8) and advances the cursor by the
 * glyph's width; func_ov007_020b8cfc repeats one glyph, func_ov007_020b8b80
 * draws a 0xff-terminated string (0xfd breaks the line) left-aligned,
 * centred or right-aligned, func_ov007_020b8b00 draws a number of up to
 * three digits and func_ov007_020b8a78 a slot heading. func_ov007_020b8ec0
 * maps a message id to the bank's message index through the arm9 range
 * table at data_0208eeec. func_ov007_020b8548 binds the bank, the glyph
 * sheet and the planes and lays out the three BG screens
 * (data_ov007_02103998 / 02103f98 / 02104598, which func_ov007_020b84e8 /
 * 020b84f4 / 020b8500 hand out); func_ov007_020b850c loads the font
 * palette.
 *
 * The message window is two Ov007Sprites, a tiled frame and a translucent
 * fill, built by func_ov007_020b8714 and freed by func_ov007_020b8690.
 * func_ov007_020b7d94 opens it for a message kind (placing and sizing the
 * frame, func_ov007_020b88c0 laying out the frame's tile map),
 * func_ov007_020b7a78 runs it each frame (grow, hold, show, shrink) and
 * draws the message once the window is nearly open, func_ov007_020b7a44
 * starts closing it and func_ov007_020b7a00 draws it. func_ov007_020b7eec
 * draws the file-select header and func_ov007_020b8188 the record board;
 * func_ov007_020b8070 scrolls the record board through planes 0 and 1.
 *
 * Edges. The bss block 0x02103458..0x02103488 and the screens 0x02103998,
 * 0x02103f98 and 0x02104598, the .data words 0x02102dd8..0x02102ef8 and the
 * .rodata tables 0x020d7a34..0x020d7b04 are referenced by functions in
 * 0x020b797c..0x020b8ec0 and by nothing else in the overlay. The previous
 * function, func_ov007_020b7948, ends unit020b7658 (bss
 * 0x02103434..0x02103458). func_ov007_020b8ec0 touches no overlay data; it
 * sits between this block's last user and unit020b8f50, whose start edge
 * leaves it out, and every caller of it is here.
 *
 * Code generation. The whole unit is compiled with opt_loop_invariants off:
 * func_ov007_020b8d48's glyph loop needs it, and the other functions compile
 * the same either way. Each place where a shape is load-bearing says so.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"
#include "GX.h"
#include "Ov007Sprite.h"
#include "Ov007Settings.h"

#pragma opt_loop_invariants off

extern "C" {

/* The window's state machine, as func_ov007_020b7a78 steps it. */
enum Ov007MessageWindowState {
    OV007_WINDOW_OPENING = 0, /* scale grows to 0x1000 */
    OV007_WINDOW_HOLDING = 1, /* kinds 1, 2, 6..8: wait 60 frames before showing */
    OV007_WINDOW_SHOWN = 2,   /* closes itself after data_ov007_020d7a84[kind] frames, if set */
    OV007_WINDOW_CLOSING = 3, /* scale shrinks to 0 */
    OV007_WINDOW_CLOSED = 4
};

/* The window record at data_ov007_02102ddc. */
typedef struct Ov007MessageWindow {
    s32 kind;      /* 0x00: the message kind func_ov007_020b7d94 opened */
    s32 state;     /* 0x04: Ov007MessageWindowState */
    s32 timer;     /* 0x08: frames in the holding / shown state */
    s32 scale;     /* 0x0c: Q12 open amount, 0..0x1000 */
    s32 textDrawn; /* 0x10: the message is on plane 0 */
    s32 pad_14;    /* 0x14: cleared by func_ov007_020b797c; no reader found */
    s32 textLine;  /* 0x18: the tile row the message starts on */
} Ov007MessageWindow;

typedef char Ov007MessageWindow_size_must_be_0x1c[sizeof(Ov007MessageWindow) == 0x1c ? 1 : -1];

/* The range table at data_0208eeec: message ids from firstId on map to
 * message indices from firstIndex on. A firstId of 0x1f40 or more ends it. */
typedef struct Ov007MessageRange {
    u16 firstId;
    u16 firstIndex;
} Ov007MessageRange;

extern Ov007MessageRange data_0208eeec[];

extern s32 data_ov007_02102dd8;                 /* text colour: data_ov007_020d7ac4 index */
extern Ov007MessageWindow data_ov007_02102ddc;  /* the window */
extern u8 data_ov007_02102df8[];                /* glyph widths in pixels */
extern char data_ov007_02102ef8[];              /* the bank's name, passed to the binder */
extern u16 data_ov007_02103458;                 /* the glyph sheet's first tile */
extern u32 *data_ov007_0210345c;                /* the glyph sheet, 4bpp tiles */
extern Ov007Sprite *data_ov007_02103460;        /* the window frame */
extern s32 data_ov007_02103464;                 /* text cursor: column in pixels */
extern s32 data_ov007_02103468;                 /* text cursor: line in tile rows */
extern Ov007Sprite *data_ov007_0210346c;        /* the window fill */
extern struct Ov007Camera *data_ov007_02103470; /* the camera the window is placed with */
extern s32 data_ov007_02103474;                 /* the language: column of data_ov007_020d7b04 */
extern s32 *data_ov007_02103478[3];             /* the three character planes */
extern u16 data_ov007_02103484[];               /* the frame's tile map */
extern u16 data_ov007_02103998[];               /* BG screen, 32x24 */
extern u16 data_ov007_02103f98[];               /* BG screen copy */
extern u16 data_ov007_02104598[];               /* BG screen copy */

extern u8 data_ov007_020d7a34[];    /* slot-heading glyphs */
extern u8 data_ov007_020d7a38[];    /* digit glyphs */
extern u16 data_ov007_020d7a44[];   /* font palette, 10 colours */
extern s32 data_ov007_020d7a50[];   /* file-select header messages */
extern s32 data_ov007_020d7a5c[3];  /* plane sizes in bytes */
extern u16 data_ov007_020d7a68[];   /* message per window kind */
extern s16 data_ov007_020d7a84[];   /* shown-state frames per window kind; 0 stays open */
extern u32 data_ov007_020d7aa0[];   /* keep-masks per pixel offset in a tile */
extern u32 data_ov007_020d7ac4[];   /* colour masks per text colour */
extern u8 data_ov007_020d7b04[][6]; /* first text line per kind and language */

extern void MultiCopy32Bytes(int *src, int *dst, int len);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void MultiStore32Bytes(unsigned val, int *dst, int len);

extern void func_ov007_020bdeb0(int sound);
extern void func_ov007_020c0308(unsigned short *dst, int b, int c);
extern void func_ov007_020c1d78(int i);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020c3598(int value, int count, s8 *digits, int base);
extern int func_ov007_020c94a0(int unused, unsigned int messageIndex);
extern void func_ov007_020c94e4(void);
/* local extern: the definition (unit020c94a0.cpp) takes two parameters;
 * this call passes the bank's name as a third, which retail never reads. */
extern void func_ov007_020c951c(int unused, void *bank, void *name);

void func_ov007_020b7a44(void);
void func_ov007_020b797c(void);

/* Returns the message index for message id `id`. */
int func_ov007_020b8ec0(int id) {
    int i = 0;
    for (;;) {
        int first = data_0208eeec[i].firstId;
        int next = data_0208eeec[i + 1].firstId;
        if (first <= id && id < next) {
            return data_0208eeec[i].firstIndex + (id - first);
        }
        if (next >= 0x1f40) {
            return data_0208eeec[i + 1].firstIndex + (id - next);
        }
        i++;
    }
}

/* Draws glyph `glyph` into plane `plane` at the cursor and advances the
 * cursor by its width. Glyph 0x4d is a 4-pixel space; a glyph that would run
 * past column 0x100 is dropped. Each glyph is 8 pixels by 16: its two tiles
 * sit one sheet row apart, and a glyph that starts inside a tile spills its
 * right part into the next tile. */
void func_ov007_020b8d48(int plane, int glyph) {
    int w;
    u32 pal;
    int adj;
    int pos;
    int row, col;
    u16 tile;
    u16 t;
    int i;
    u32 *dst;
    u32 palmask;
    u32 *src;
    int rshift;
    u32 mask;
    int q, rem;
    int shift;

    pal = data_ov007_02102dd8;
    if (glyph == 0xf0 || glyph == 0xee)
        pal = 0xd;

    if (glyph == 0x4d) {
        data_ov007_02103464 += 4;
        return;
    }

    if (glyph == 0x4d)
        w = 4;
    else
        w = data_ov007_02102df8[glyph];

    adj = 0;
    if (glyph == 1)
        adj += 2;
    w -= adj;
    pos = data_ov007_02103464 + adj;
    data_ov007_02103464 = pos;
    if (pos >= 0x100 - w)
        return;

    row = glyph >> 5;
    col = glyph - (row << 5);
    tile = (u16)(col + (row << 6));
    t = (u16)(data_ov007_02103458 + tile);

    q = pos >> 3;
    rem = pos - (q << 3);

    dst = (u32 *)data_ov007_02103478[plane] + (q + data_ov007_02103468 * 32) * 8;
    src = data_ov007_0210345c + t * 8;

    shift = rem * 4;
    rshift = 0x20 - shift;
    palmask = data_ov007_020d7ac4[pal];
    mask = data_ov007_020d7aa0[rem];

    for (i = 0; i < 16; i++) {
        u32 s = *src++ & palmask;
        *dst = (*dst & mask) | (s << shift);
        if (rem != 0) {
            u32 *d2 = (u32 *)(int)(dst + 8);
            *d2 = (*d2 & ~mask) | (s >> rshift);
        }
        dst++;
        if (i == 7) {
            dst += 0xf8;
            src += 0xf8;
        }
    }

    data_ov007_02103464 += w;
}

/* Draws glyph `glyph` `n` times. */
void func_ov007_020b8cfc(int plane, int glyph, int n) {
    int i = 0;
    if (n <= 0)
        return;
    do {
        func_ov007_020b8d48(plane, glyph);
        i++;
    } while (i < n);
}

/* Draws string `s` from the cursor. Each line is laid out first, then drawn
 * left-aligned (mode 1), centred (mode 0) or right-aligned (mode 2) on the
 * starting column; 0xfd ends a line (the next starts two tile rows down) and
 * 0xff ends the string. */
void func_ov007_020b8b80(int plane, u8 *s, int mode) {
    u8 buf[0x100];
    int newline;
    int x0;
    int n;
    int width;
    int i;
    int c;
    int cont;
    int flush;

    newline = 1;
    cont = 1;
    flush = 0;
    x0 = data_ov007_02103464;

    do {
        c = *s;
        if (newline) {
            width = 0;
            n = 0;
            newline = 0;
        }
        switch (c) {
        case 0xFD:
            newline = 1;
            flush = 1;
            break;
        case 0xFF:
            cont = 0;
            flush = 1;
            break;
        default:
            buf[n] = (u8)c;
            n++;
            width += (c == 0x4D) ? 4 : data_ov007_02102df8[c];
            break;
        }
        if (flush) {
            switch (mode) {
            case 1:
                data_ov007_02103464 = x0;
                break;
            case 0:
                data_ov007_02103464 = x0 - width / 2;
                break;
            case 2:
                data_ov007_02103464 = x0 - width;
                break;
            }
            for (i = 0; i < n; i++)
                func_ov007_020b8d48(plane, buf[i]);
            flush = 0;
        }
        if (newline) {
            data_ov007_02103464 = x0;
            data_ov007_02103468 += 2;
        }
        s++;
    } while (cont);
}

/* Draws `value` as up to three decimal digits (blank digits are skipped),
 * then moves the cursor 7 pixels per digit drawn. */
void func_ov007_020b8b00(int plane, int value) {
    s8 buf[3];
    int i;
    int saved;
    int r;
    s8 *p;
    saved = data_ov007_02103464;
    r = func_ov007_020c3598(value, 3, buf, 10);
    p = buf + 2;
    for (i = 2; i >= 0; i--) {
        if (*p >= 0)
            func_ov007_020b8d48(plane, data_ov007_020d7a38[*p]);
        p--;
    }
    data_ov007_02103464 = saved + r * 7;
}

/* Draws the heading of save slot `slot`: message 0x1f63 and the slot's
 * glyph, or five dashes for no slot. */
void func_ov007_020b8a78(int plane, int slot) {
    int r;

    if (slot >= 0) {
        r = func_ov007_020b8ec0(0x1f63);
        r = func_ov007_020c94a0(0, (unsigned int)r);
        func_ov007_020b8b80(plane, (u8 *)r, 1);

        data_ov007_02103464 = data_ov007_02103464 + 4;
        func_ov007_020b8d48(plane, data_ov007_020d7a34[slot]);
    } else {
        func_ov007_020b8cfc(plane, 0x47, 5);
    }
}

/* Lays out the frame's tile map for the frame's size (8-pixel tiles: the
 * corners 0, 2, 8 and 0xa, the edges 1, 4, 6 and 9, the inside 0xffff) and
 * fits the fill inside the frame. The fill values go through volatile
 * locals, the corpus idiom for MultiStore16's value argument, and the
 * right-edge index's no-op long long round trip is load-bearing. */
void func_ov007_020b88c0(void) {
    volatile u16 v0;
    volatile u16 v1;
    volatile u16 v2;
    int w;
    int h;
    int botrow;
    int i;
    int row;

    {
        Ov007Sprite *frame = data_ov007_02103460;
        w = frame->width >> 3;
        h = frame->height >> 3;
    }

    v0 = 0xffff;
    MultiStore16(v0, (char *)data_ov007_02103484, (w * h) << 1);
    v1 = 1;
    MultiStore16(v1, (char *)data_ov007_02103484, w << 1);
    v2 = 9;
    botrow = (h - 1) * w;
    MultiStore16(v2, (char *)(data_ov007_02103484 + botrow), w << 1);

    i = 0;
    if (h > 0) {
        row = i;
        do {
            data_ov007_02103484[row] = 4;
            data_ov007_02103484[(int)(((long long)row)) + w - 1] = 6;
            i++;
            row += w;
        } while (i < h);
    }

    data_ov007_02103484[0] = 0;
    data_ov007_02103484[w - 1] = 2;
    data_ov007_02103484[botrow] = 8;
    data_ov007_02103484[(unsigned int)w * h - 1] = 0xa;

    data_ov007_0210346c->screenX = data_ov007_02103460->screenX;
    data_ov007_0210346c->screenY = data_ov007_02103460->screenY;
    data_ov007_0210346c->width = (s16)(data_ov007_02103460->width - 2);
    data_ov007_0210346c->height = (s16)(data_ov007_02103460->height - 2);

    func_ov007_020c43bc(data_ov007_0210346c, data_ov007_02103470);
    func_ov007_020c4388(data_ov007_0210346c, data_ov007_02103470);
    func_ov007_020c43bc(data_ov007_02103460, data_ov007_02103470);
    func_ov007_020c4388(data_ov007_02103460, data_ov007_02103470);
}

/* Starts the window for message kind `kind` in state `state` and clears
 * plane 0. */
void func_ov007_020b8860(int kind, int state) {
    volatile int z = 0;
    data_ov007_02102ddc.state = state;
    data_ov007_02102ddc.kind = kind;
    data_ov007_02102ddc.timer = 0;
    data_ov007_02102ddc.scale = 0;
    data_ov007_02102ddc.textDrawn = 0;
    MultiStore32Bytes((unsigned)z, data_ov007_02103478[0], data_ov007_020d7a5c[0]);
    func_ov007_020c1d8c(0);
}

/* Marks the window closed. */
void func_ov007_020b883c(void) {
    data_ov007_02102ddc.kind = 0;
    data_ov007_02102ddc.state = OV007_WINDOW_CLOSED;
    func_ov007_020c1d78(0);
}

/* Builds the window's sprites once (the fill: plain translucent quad; the
 * frame: tiled from data_ov007_02103484 with texture `texture`) and resets
 * the window to closed. */
void func_ov007_020b8714(int camera, int texture, int language) {
    if (data_ov007_0210346c == 0 && data_ov007_02103460 == 0) {
        data_ov007_0210346c = func_ov007_020c44e4(1);
        data_ov007_0210346c->depth = 0xffffec04;
        data_ov007_0210346c->alpha = 0x18000;
        data_ov007_0210346c->drawFlags |= 1;
        data_ov007_0210346c->polygonId = 0x1d;
        data_ov007_0210346c->polygonAttr = 0x800;
        data_ov007_02103460 = func_ov007_020c44e4(1);
        data_ov007_02103460->depth = 0xffffec04;
        data_ov007_02103460->polygonId = 0x1e;
        data_ov007_02103460->texColor0 = 1;
        data_ov007_02103460->drawFlags |= 8;
        data_ov007_02103460->tilemap = data_ov007_02103484;
        *data_ov007_02103460->frames = (void *)texture;
    }
    data_ov007_02103470 = (struct Ov007Camera *)camera;
    data_ov007_02102ddc.kind = 0;
    data_ov007_02102ddc.state = OV007_WINDOW_CLOSED;
    data_ov007_02102ddc.timer = 0;
    data_ov007_02102ddc.scale = 0;
    data_ov007_02102ddc.textDrawn = 0;
    data_ov007_02103474 = language;
}

/* Unbinds the glyph sheet and the bank and frees the window's sprites. */
void func_ov007_020b8690(void) {
    // The guard folds away, but it keeps sheet and baseTile alive through
    // propagation, so both addresses load before the zero (ROM: zero in r0,
    // baseTile in r1, sheet in r2). notes/mwccarm-codegen.md 6dh.
    u32 **sheet = &data_ov007_0210345c;
    u16 *baseTile = &data_ov007_02103458;
    if (sheet != 0 && baseTile != 0) {
        *sheet = 0;
        *baseTile = 0;
    }
    func_ov007_020c94e4();
    if (data_ov007_0210346c != 0) {
        func_ov007_020c44c4(data_ov007_0210346c);
        data_ov007_0210346c = 0;
    }
    if (data_ov007_02103460 != 0) {
        func_ov007_020c44c4(data_ov007_02103460);
        data_ov007_02103460 = 0;
    }
}

/* Binds message bank `bank`, glyph sheet `sheet` (first tile `baseTile`)
 * and the planes (the third optional), clears them, lays out the BG
 * screen and its two copies, and opens no window. The zero values go
 * through volatile locals, the corpus idiom for MultiStore32Bytes's value
 * argument. */
void func_ov007_020b8548(void *unused, void *sheet, int baseTile, void *plane0, void *plane1,
                         int plane2, void *bank) {
    func_ov007_020c951c(0, bank, data_ov007_02102ef8);

    data_ov007_02103458 = (u16)baseTile;
    data_ov007_0210345c = (u32 *)sheet;
    data_ov007_02103464 = 0;
    data_ov007_02103468 = 0;

    data_ov007_02102dd8 = 0xe;

    data_ov007_02103478[0] = (s32 *)plane0;
    data_ov007_02103478[1] = (s32 *)plane1;
    data_ov007_02103478[2] = (s32 *)plane2;

    {
        volatile int v1 = 0;
        MultiStore32Bytes(v1, data_ov007_02103478[0], data_ov007_020d7a5c[0]);
    }
    {
        volatile int v2 = 0;
        MultiStore32Bytes(v2, data_ov007_02103478[1], data_ov007_020d7a5c[1]);
    }

    if (plane2 != 0) {
        volatile int v3 = 0;
        MultiStore32Bytes(v3, data_ov007_02103478[2], data_ov007_020d7a5c[2]);
    }

    func_ov007_020c0308(data_ov007_02103998, 0x18, 0xf);
    MultiCopy32Bytes((int *)data_ov007_02103998, (int *)data_ov007_02104598, 0x600);
    MultiCopy32Bytes((int *)data_ov007_02103998, (int *)data_ov007_02103f98, 0x600);

    func_ov007_020b8860(0, OV007_WINDOW_CLOSED);
    func_ov007_020b797c();
}

/* Loads the font palette into both engines' BG palettes. */
void func_ov007_020b850c(void) {
    GX::LoadBGPltt(data_ov007_020d7a44, 0x1f6, 0xa);
    GXS::LoadBGPltt(data_ov007_020d7a44, 0x1f6, 0xa);
}

void *func_ov007_020b8500(void) {
    return data_ov007_02104598;
}

void *func_ov007_020b84f4(void) {
    return data_ov007_02103f98;
}

void *func_ov007_020b84e8(void) {
    return data_ov007_02103998;
}

/* Draws the record board of save slot `slot` into plane 2 (ov007 0x020b8188,
 * 0x360 bytes). Eighteen lines: line 0 prints the slot heading
 * (func_ov007_020b8a78) and the entry count at records[slot + 2]; line 1
 * prints one of two headings (0x1f57, then 0x1f62 or 0x1f58 by `alt`); line
 * 0x11 prints message 0x13b0 and the byte at *(int *)records + slot + 0x98;
 * lines 2..0x10 print one record each from the 0x1388 messages plus three
 * bytes of the record at *(int *)records + slot * 0xf + line - 2 (the first
 * clamped to 7 when it reaches 8).
 *
 * Every literal below is a named local because the ROM keeps them all in the
 * frame (0x1c..0x4c, one slot per constant, in declaration order) and reloads
 * them at each use; the register set is the counter, the parameter, five
 * register constants and the column pointer. Shapes that were load-bearing:
 * `zero` is a volatile local assigned right before the call (the corpus idiom
 * for MultiStore32Bytes's value argument); `sb = 0; s14 = 0;` precede the
 * loop so the zero-valued slots store from the counter register; base15 is
 * assigned AFTER the call (a declaration initializer computes it before);
 * the clamp is a ternary, not an if, which raises s48's web priority so its
 * spill store lands after s3c's; row/m use `+=` for the ROM's add operand
 * order; the line-1 heading is an if/else inside case 1 (the 0x1f58 chain is
 * the else arm, not part of case 0x11). */
void func_ov007_020b8188(int slot, char *records, int alt) {
    volatile int zero;
    int t = 2;
    int one = 1;
    int f0 = 0xf0;
    int ef = 0xef;
    int c0 = 0xc0;
    int sb;
    int b8, bc, b10;
    int s14;
    int base15;
    int s1c = 0x5d, s20 = 0x4d, s24 = 0x30, s28 = 0, s2c = 0, s30 = 0;
    int s34 = 0xc, s38 = 0, s3c = 0x9c, s40 = 6, s44 = 0, s48 = 7, s4c = 0xee;

    zero = 0;
    MultiStore32Bytes(zero, data_ov007_02103478[2], data_ov007_020d7a5c[2]);
    base15 = slot * 0xf;
    data_ov007_02102dd8 = 0xe;

    sb = 0;
    s14 = 0;
    for (; sb < 0x12; sb++) {
        data_ov007_02103468 = s14;
        switch (sb) {
        case 0:
            data_ov007_02103464 = s1c;
            func_ov007_020b8a78(t, slot);
            func_ov007_020b8d48(t, s20);
            func_ov007_020b8d48(t, s20);
            func_ov007_020b8d48(t, f0);
            func_ov007_020b8d48(t, ef);
            func_ov007_020b8b00(t, ((s16 *)(records + 4))[slot]);
            break;
        case 1:
            data_ov007_02103464 = s24;
            func_ov007_020b8b80(
                t, (u8 *)func_ov007_020c94a0(s28, (unsigned int)func_ov007_020b8ec0(0x1f57)), one);
            if (alt != 0) {
                data_ov007_02103464 = c0;
                func_ov007_020b8b80(
                    t, (u8 *)func_ov007_020c94a0(s2c, (unsigned int)func_ov007_020b8ec0(0x1f62)),
                    one);
            } else {
                data_ov007_02103464 = c0;
                func_ov007_020b8b80(
                    t, (u8 *)func_ov007_020c94a0(s30, (unsigned int)func_ov007_020b8ec0(0x1f58)),
                    one);
            }
            break;
        case 0x11:
            data_ov007_02103464 = s34;
            func_ov007_020b8b80(
                t, (u8 *)func_ov007_020c94a0(s38, (unsigned int)func_ov007_020b8ec0(0x13b0)), one);
            data_ov007_02103464 = s3c;
            func_ov007_020b8d48(t, f0);
            func_ov007_020b8d48(t, ef);
            func_ov007_020b8b00(t, *(u8 *)(slot + *(int *)records + 0x98));
            break;
        default: {
            int row = func_ov007_020b8ec0(0x1388);
            row += sb - 2;
            u8 *m = (u8 *)(base15 + *(int *)records);
            m += sb - 2;
            b8 = m[0x11];
            bc = m[0x3e];
            b10 = m[0x6b];
            data_ov007_02103464 = s40;
            func_ov007_020b8b80(t, (u8 *)func_ov007_020c94a0(s44, (unsigned int)row), one);
            data_ov007_02103464 = s3c;
            func_ov007_020b8d48(t, f0);
            func_ov007_020b8d48(t, ef);
            b8 = (b8 >= 8) ? s48 : b8;
            func_ov007_020b8b00(t, b8);
            data_ov007_02103464 = c0;
            func_ov007_020b8d48(t, s4c);
            func_ov007_020b8d48(t, ef);
            func_ov007_020b8b00(t, bc);
            if (b10 != 0)
                func_ov007_020b8d48(t, f0);
            break;
        }
        }
        s14 += 2;
    }
}

/* Shows the record board (plane 2) scrolled by `n` tile rows across
 * planes 1 and 0, 0x17 rows each, clearing what it does not cover. */
void func_ov007_020b8070(int n) {
    volatile int z1, z2, z3, z4;
    if (n < 0x17) {
        int b, a;
        a = 0x17 - n;
        b = 0x24 - a;
        if (b >= 0x17)
            b = 0x16;
        MultiCopy32Bytes(data_ov007_02103478[2], data_ov007_02103478[1] + n * 0x100, a * 0x400);
        z1 = 0;
        MultiStore32Bytes((unsigned)z1, data_ov007_02103478[1], n * 0x400);
        MultiCopy32Bytes(data_ov007_02103478[2] + a * 0x100, data_ov007_02103478[0] + 0x100,
                         b * 0x400);
        z2 = 0;
        MultiStore32Bytes((unsigned)z2, data_ov007_02103478[0] + (b + 1) * 0x100,
                          (0x17 - b) * 0x400);
    } else {
        int a = n - 0x16;
        z4 = 0;
        MultiStore32Bytes((unsigned)z4, data_ov007_02103478[1], data_ov007_020d7a5c[1]);
        z3 = 0;
        MultiStore32Bytes((unsigned)z3, data_ov007_02103478[0], a * 0x400);
        MultiCopy32Bytes(data_ov007_02103478[2], data_ov007_02103478[0] + a * 0x100,
                         (0x17 - a) * 0x400);
    }
}

/* Draws the file-select header into plane 0 from the boot arguments: the
 * message for soundModeItem, then messages 0x1f51 and 0x1f52 in colours 0xb
 * and 0xc, swapped when backlightOn is clear. */
void func_ov007_020b7eec(Ov007BootArgs *bootArgs) {
    int r;
    func_ov007_020c0308(data_ov007_02103998, 0x18, 0xf);
    func_ov007_020c0308(data_ov007_02104598, 0x18, 0xf);
    {
        volatile int z = 0;
        MultiStore32Bytes((unsigned)z, data_ov007_02103478[0], data_ov007_020d7a5c[0]);
    }
    data_ov007_02102dd8 = 0xb;
    data_ov007_02103464 = 0xac;
    data_ov007_02103468 = 4;
    r = func_ov007_020b8ec0(data_ov007_020d7a50[bootArgs->soundModeItem]);
    r = func_ov007_020c94a0(0, (unsigned int)r);
    func_ov007_020b8b80(0, (u8 *)r, 0);
    data_ov007_02103464 = 0x8c;
    data_ov007_02103468 = 0xa;
    if (bootArgs->backlightOn != 0) {
        data_ov007_02102dd8 = 0xb;
    } else {
        data_ov007_02102dd8 = 0xc;
    }
    r = func_ov007_020b8ec0(0x1f51);
    r = func_ov007_020c94a0(0, (unsigned int)r);
    func_ov007_020b8b80(0, (u8 *)r, 0);
    data_ov007_02103464 = 0xcc;
    data_ov007_02103468 = 0xa;
    r = data_ov007_02102dd8;
    if (r == 0xb) {
        data_ov007_02102dd8 = 0xc;
    } else {
        data_ov007_02102dd8 = 0xb;
    }
    r = func_ov007_020b8ec0(0x1f52);
    r = func_ov007_020c94a0(0, (unsigned int)r);
    func_ov007_020b8b80(0, (u8 *)r, 0);
}

/* Opens the window for message kind `kind` (0 opens nothing): places and
 * sizes the frame for the kind, picks the first text line for the kind and
 * language, and lays the frame out. */
void func_ov007_020b7d94(int kind) {
    switch (kind) {
    case 0:
        return;
    case 1:
    case 2:
    case 6:
    case 7:
    case 8:
        data_ov007_02103460->screenX = 0x80000;
        data_ov007_02103460->screenY = 0x62000;
        data_ov007_02103460->width = 0xc0;
        data_ov007_02103460->height = 0x58;
        break;
    case 3:
    case 4:
    case 5:
    case 10:
    case 11:
        data_ov007_02103460->screenX = 0x80000;
        data_ov007_02103460->screenY = 0x5a000;
        data_ov007_02103460->width = 0xc0;
        data_ov007_02103460->height = 0x40;
        data_ov007_02102ddc.textLine = 0xa;
        break;
    case 9:
    case 12:
        data_ov007_02103460->screenX = 0x80000;
        data_ov007_02103460->screenY = 0x6e000;
        data_ov007_02103460->width = 0xc8;
        data_ov007_02103460->height = 0x68;
        data_ov007_02102ddc.textLine = 0xa;
        break;
    }
    data_ov007_02102ddc.textLine = data_ov007_020d7b04[kind][data_ov007_02103474];
    func_ov007_020b8860(kind, OV007_WINDOW_OPENING);
    func_ov007_020b88c0();
}

/* Q12 product, truncated. */
static inline int MulFx(int a, int b) {
    return (int)(((s64)a * b) >> 12);
}

/* Runs the window for one frame. Opening, it grows by 0x100 a frame and
 * draws the kind's message once it is 0xaf0 open (kinds 5 and 0xb also play
 * a sound); closing, it wipes the message below 0xaf0 and shrinks to closed.
 * Both sprites are scaled by scale * (2 - scale), an ease-out. */
void func_ov007_020b7a78(void) {
    switch (data_ov007_02102ddc.state) {
    case OV007_WINDOW_OPENING:
        if (data_ov007_02102ddc.textDrawn == 0) {
            if (data_ov007_02102ddc.scale == 0) {
                volatile int zero;
                zero = 0;
                MultiStore32Bytes(zero, data_ov007_02103478[0], data_ov007_020d7a5c[0]);
            } else if (data_ov007_02102ddc.scale >= 0xAF0) {
                data_ov007_02102dd8 = 0xF;
                data_ov007_02103464 = 0x80;
                data_ov007_02103468 = data_ov007_02102ddc.textLine;
                func_ov007_020b8b80(
                    0,
                    (u8 *)func_ov007_020c94a0(
                        0, func_ov007_020b8ec0(data_ov007_020d7a68[data_ov007_02102ddc.kind])),
                    0);
                data_ov007_02102ddc.textDrawn = 1;
                if (data_ov007_02102ddc.kind == 5) {
                    func_ov007_020bdeb0(0x10);
                } else if (data_ov007_02102ddc.kind == 0xB) {
                    func_ov007_020bdeb0(0x11);
                }
            }
        }
        data_ov007_02102ddc.scale += 0x100;
        if (data_ov007_02102ddc.scale > 0x1000) {
            data_ov007_02102ddc.scale = 0x1000;
            if (data_ov007_02102ddc.kind == 1 || data_ov007_02102ddc.kind == 2 ||
                (unsigned int)(data_ov007_02102ddc.kind - 6) <= 2U) {
                data_ov007_02102ddc.state = OV007_WINDOW_HOLDING;
            } else {
                data_ov007_02102ddc.state = OV007_WINDOW_SHOWN;
            }
        }
        break;
    case OV007_WINDOW_HOLDING:
        data_ov007_02102ddc.timer += 1;
        if (data_ov007_02102ddc.timer >= 0x3C) {
            data_ov007_02102ddc.timer = 0;
            data_ov007_02102ddc.state = OV007_WINDOW_SHOWN;
        }
        break;
    case OV007_WINDOW_SHOWN:
        if (data_ov007_02102ddc.timer == 0 &&
            (data_ov007_02102ddc.kind == 9 || data_ov007_02102ddc.kind == 0xC)) {
            func_ov007_020c1d78(0);
        }
        {
            short v = data_ov007_020d7a84[data_ov007_02102ddc.kind];
            if (v > 0) {
                data_ov007_02102ddc.timer += 1;
                if (data_ov007_02102ddc.timer >= v) {
                    func_ov007_020b7a44();
                }
            }
        }
        break;
    case OV007_WINDOW_CLOSING:
        if (data_ov007_02102ddc.kind == 9 || data_ov007_02102ddc.kind == 0xC) {
            func_ov007_020c1d8c(0);
        }
        if (data_ov007_02102ddc.textDrawn != 0 && data_ov007_02102ddc.scale <= 0xAF0) {
            volatile int zero;
            zero = 0;
            data_ov007_02102ddc.textDrawn = 0;
            MultiStore32Bytes(zero, data_ov007_02103478[0], data_ov007_020d7a5c[0]);
        }
        data_ov007_02102ddc.scale -= 0x100;
        if (data_ov007_02102ddc.scale < 0) {
            data_ov007_02102ddc.scale = 0;
            func_ov007_020b883c();
        }
        break;
    case OV007_WINDOW_CLOSED:
    default:
        break;
    }

    if (data_ov007_02102ddc.scale > 0) {
        int t = data_ov007_02102ddc.scale;
        int ease = MulFx(t, 0x2000 - t);
        data_ov007_0210346c->scaleY = ease;
        data_ov007_0210346c->scaleX = data_ov007_0210346c->scaleY;
        data_ov007_02103460->scaleY = ease;
        data_ov007_02103460->scaleX = data_ov007_02103460->scaleY;
    } else {
        data_ov007_0210346c->scaleY = 0;
        data_ov007_0210346c->scaleX = data_ov007_0210346c->scaleY;
        data_ov007_02103460->scaleY = 0;
        data_ov007_02103460->scaleX = data_ov007_02103460->scaleY;
    }
}

/* Starts closing the window. */
void func_ov007_020b7a44(void) {
    int kind = data_ov007_02102ddc.kind;
    data_ov007_02102ddc.state = OV007_WINDOW_CLOSING;
    data_ov007_02102ddc.timer = data_ov007_020d7a84[kind];
    data_ov007_02102ddc.scale = 0x1000;
}

int func_ov007_020b7a34(void) {
    return data_ov007_02102ddc.kind;
}

/* Draws the window: the fill, then the frame. */
void func_ov007_020b7a00(void) {
    func_ov007_020c421c(data_ov007_0210346c);
    func_ov007_020c421c(data_ov007_02103460);
}

int func_ov007_020b79e4(void) {
    return data_ov007_02102ddc.state != OV007_WINDOW_CLOSED;
}

int func_ov007_020b79c8(void) {
    return data_ov007_02102ddc.state == OV007_WINDOW_SHOWN;
}

/* Clears plane 0. */
void func_ov007_020b797c(void) {
    volatile int x = 0;
    MultiStore32Bytes(x, data_ov007_02103478[0], data_ov007_020d7a5c[0]);
    data_ov007_02102ddc.pad_14 = 0;
}
}
