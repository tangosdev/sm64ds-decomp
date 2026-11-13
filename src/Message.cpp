//cpp
/* Message window. The display entry points and the helpers that draw one
 * line into a background.
 *
 * leftovers:
 * - defer_codegen is off so this file emits in source order. The per-function
 *   optimization pragmas are brackets; under deferred codegen they would be
 *   last-wins for the whole file.
 * - Display is the only Message method here that takes this. The others are
 *   static and drive the globals around data_0209d660.
 * - data_0209d708 is the bank's MessageEntry array (MessageBank.h). Where
 *   indexing it by a runtime index swaps the base and index registers, the
 *   entry is addressed as (char *)data_0209d708 + idx * 8 and read through
 *   a MessageEntry cast.
 *   data_0209d70c stays int * (decl_common.h spells it so); [1] is
 *   MessageInfoSection::sectionSize. data_0209d6f0 stays u8 * for the same
 *   reason (Message2.cpp) and is read back through a MessageEntry cast.
 * - data_0209d6f4 is volatile. DisplayCourseNameForStarSelect stores it twice
 *   in a row and both stores have to emit.
 * - IsStarCollected and Message::DisplayText stay local externs. The header
 *   spellings take s8 / u16 and those parameters emit a narrow the ROM does
 *   not have. DisplayText must see this untruncated.
 * - Sound::ChangeMusicVolume stays mangled. The second parameter is
 *   Fix12<int> by value.
 * - BG pointer getters are the SDK's G2_ / G2S_ functions
 *   (nitro/gx/gx_bgcnt.h). They return void *, so each call site casts
 *   to the type it computes with.
 * - Address-named helpers stay address-named. The ROM has no English name
 *   for them.
 * - dScStage_c::UpdateMessage, the stage's per-frame message driver, opens
 *   the file: it is first by address (0x0201cb7c).
 */

#pragma defer_codegen off
#include "types.h"
#include "nitro/hw/registers.h"
#include "Message.h"
#include "nitro/gx/g2.h"
#include "SaveData.h"
#include "dScStage_c.h"
#include "Sound.h"
#include "MessageBank.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/mi/memory.h"
#include "nitro/gx/gx.h"

extern "C" {
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
/* The message system's .bss, 0x0209d64c..0x0209e64c, defined here. mwcc
   sorts a translation unit's .bss objects by ascending size, and this run
   climbs without a drop from the u8 state through the 16-bit words, the
   choice table, the words and pointers, to the two glyph buffers. Its
   readers are this file, Message2.cpp, Message::SetTextGlobals, LoadFont and
   Message::LoadText: the five tree files that tile .text 0x0201ab04..
   0x0201fec8, one original translation unit. Every object below is reached
   from more than one of them or from outside the run, so all stay global.
   The address-named objects keep their address names. */
/* The message box rectangle, set when a box opens (Message.cpp's layout code):
   gMsgBoxLeft/gMsgBoxTop are its final left and top edges and
   gMsgBoxCenterX/gMsgBoxCenterY its centre. The window registers (WIN0H/WIN0V)
   are written as (edge << 8) | (2 * centre - edge + 1), so the right and bottom
   edges mirror the left and top. gMsgBoxWinLeft/gMsgBoxWinTop are the edges
   the window is drawn with right now: they start at the centre and the
   opening animation walks them out to gMsgBoxLeft/gMsgBoxTop. At runtime the
   next-page arrow sits at (2 * centre x - 6 - left, 2 * centre y + 2 - top),
   by the box's bottom-right corner. */
u8 gMsgBoxWinTop;
u8 gMsgBoxLeft;
u8 data_0209d654;
u8 gMsgBoxWinLeft;
u8 data_0209d65c;
u8 data_0209d660;
u8 data_0209d664;
u8 data_0209d668;
u8 data_0209d66c;
u8 data_0209d670;
u8 data_0209d674;
u8 data_0209d678;
u8 data_0209d67c;
u8 data_0209d680;
u8 data_0209d684;
/* The line a horizontal (mode 1) choice sits on (Message2.cpp). */
u8 gMsgChoiceLine;
u8 gMsgChoiceIndex;
u8 data_0209d690;
u8 data_0209d694;
u8 data_0209d698;
u8 data_0209d69c;
u8 data_0209d6a0;
u8 data_0209d6a4;
u8 data_0209d6a8;
u8 data_0209d6ac;
u8 data_0209d6b0;
u8 data_0209d6b4;
u8 data_0209d6b8;
u8 data_0209d6bc;
u8 data_0209d6c0;
/* The message box's style: the text colour mask (data_0208eecc[it]), which
   next-page arrow sprite is drawn, and the closing sound (0x21 or 5). The
   Display* entry points set it: 1 for a sign (black arrow), 0 for a character
   talking (white arrow). Under melonDS, flipping it on an open sign box turns
   the arrow white, and on a Toad's box turns it black. */
u8 gMsgBoxStyle;
u8 gMsgBoxCenterY;
u8 gMsgBoxTop;
u8 gMsgBoxCenterX;
s16 data_0209d6d4;
s16 data_0209d6d8;
s16 data_0209d6dc;
u16 data_0209d6e0;
/* Where each choice's cursor goes, recorded by AddChoice (Message2.cpp). The
   bound is the 4-byte slot the object occupies. */
u8 gMsgChoicePositions[4];
/* The message bank pointers Message::SetTextGlobals splits the loaded bank
   into; MessageBank.h gives data_0209d6e8, data_0209d6ec, data_0209d6fc,
   data_0209d708 and data_0209d70c readable aliases. */
int data_0209d6e8;
int data_0209d6ec;
u8 *data_0209d6f0;
/* byte-forced spelling: the cell is written twice back-to-back in
   DisplayCourseNameForStarSelect and both stores must emit, so it is
   volatile here; plurality decl elsewhere is u8 *. */
volatile int data_0209d6f4;
int data_0209d6f8;
int data_0209d6fc;
int data_0209d700;
s32 data_0209d704;
MessageEntry *data_0209d708;
int *data_0209d70c;
u8 data_0209d710[0x3c];
s32 data_0209d74c[0x3c0];
extern u8 data_0208ee74[];
extern u8 data_0208f074[];
extern u8 data_0209f20c;
extern u8 gCurrentStarId;
extern u8 gCollectedStarId;
extern u8 gGameMode;
extern s32 gRunningKuppaScript;
extern s8 gNextSublevel;
extern u32 gNextActorUpdateMask;
extern s32 data_0208ee44;

extern void SetBg1Offset(int a, int b);
extern void SetBg3Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void func_0201b6f8(int a);
extern void func_0201b388(int a);
extern void func_0201b100(int a);
extern int func_0201fb4c(void);
/* local extern: byte-forced spelling -- the definition is (s8, int); a s8
   parameter would emit a sign-extend of arg0 the ROM lacks. */
extern int IsStarCollected(s32 a, u8 b);
extern int SublevelToLevel(int a);
}
namespace Sound {
int ChangeMusicVolume(u32 a, s32 b);
}
extern "C" {

void func_0201cd08(int arg);
void func_0201cebc(short a0);
void func_0201cfb8(int arg0);
void LoadControllerModeText(int param0);
void func_0201d418(int p0, int p1);
void Message_DrawLineBG2(void);
void Message_DrawCenteredLine(int mapPos, int width);
void func_0201d850(u32 course);
void func_0201e220(int arg);
void func_0201eaac(void);
void func_0201ef38(void);
void func_0201ef50(int arg);
void func_0201f108(void);
void func_0201f138(void);
void func_0201f32c(int arg0);
}

void dScStage_c::UpdateMessage() {
    if (data_0209d660 == 0)
        return;

    if (Message::UpdateWindow()) {
        if (data_0209d654 == 0) {
            Message::Update();
            return;
        }

        if (data_0209d6d4 == 0x295) {
            if (data_0209d67c == 0x3c) {
                SaveData::SaveCurrentFile();
            }
            if (data_0209d67c != 0) {
                data_0209d67c -= data_0208ee44;
                if (data_0209d67c == 0) {
                    data_0209d670 = 1;
                }
            }
            return;
        }

        if (data_0209d67c != 0) {
            data_0209d67c -= data_0208ee44;
            if (data_0209d67c == 0) {
                data_0209d670 = 1;
            }
        }
        return;
    }

    if (data_0209d654 != 0) {
        if (data_0209d660 != 0)
            return;
        if (data_0209d6d4 != 0x295) {
            data_0209d654 = 0;
            return;
        }
        Message::DisplaySaving(0x296);
    }
}

extern "C" void func_0201cd08(int arg) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    data_0209d6d4 = arg;
    func_0201d418(data_0209d6d4 == 0x29a ? 0x105 : 0xc5, 0xb0);
    data_0209d6d4 = 0x293;
    data_0209d6a8 = data_0209d6a8 << 1;
    Message_DrawCenteredLine(0x285, 0xa);
    data_0209d6d4 = data_0209d6d4 + 1;
    Message_DrawCenteredLine(0x291, 0xa);
}

void Message::DisplaySaveStatusText(unsigned short n)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209d6d4 = n;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    data_0209d674 = 0;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    func_0201d418(0x109, 0x70);
}

extern "C" void func_0201cebc(short a0)
{
    volatile unsigned short v;
    volatile int z;
    data_0209d6d4 = a0;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    data_0209d674 = 0;
    {
        int *dst = (int *)((u32)G2S_GetBG0CharPtr() + 0x4000);
        z = 0;
        MI_CpuFill32(dst, z, 0x2000);
    }
    {
        char *dst = (char *)G2S_GetBG0ScrPtr();
        v = 0x2ff;
        MI_CpuFill16(dst, v, 0x800);
    }
    Message_DrawCenteredLine(0x100, 0x20);
    data_0209d6d4 = 0x27e;
    Message_DrawCenteredLine(0x1c0, 0x20);
}

void Message::DisplayLevelClearText(u16 msgID, u8 course) {
    func_0201d850(course);
    data_0209d6d4 = msgID;
    data_0209d668 = 1;
    func_0201e220(1);
}

extern "C" void func_0201cfb8(int arg0)
{
    data_0209d6d4 = arg0;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    func_0201d418(0xcf, 0x68);
}

void Message::DisplayOptionsMenuText(s16 n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    func_0201cfb8(n);
    data_0209d6d4 = 0x287;
    data_0209d6c0 = 1;
    func_0201d418(0x18f, 0x28);
    data_0209d6d4 = data_0209d6d4 + 1;
    func_0201d418(0x197, 0x28);
    data_0209d6a8 = data_0209d6a8 << 1;
    data_0209d6d4 = 0x283;
    Message_DrawCenteredLine(0x280, 0x20);
}

/* opt_common_subs stays off: each of the three rows recomputes (n + 7) / 8 for its
 * loop bound the way the cartridge does, and CSE shares the divide instead
 * (0x1e0 against 0x20c). Reading the count back from data_0209d6b0 for the second
 * copy forces the reload but not the same schedule (0x210). */
#pragma push
#pragma opt_common_subs off
extern "C" void LoadControllerModeText(int param0)
{
    int n, m, i;
    short *scr;

    data_0209d6d4 = param0;
    data_0209d660 = 0;
    func_0201eaac();

    data_0209d668 = 1;
    data_0209d6a8 = 0;
    data_0209d674 = 0;
    Message_DrawLineBG2();

    n = data_0209d6b0;
    m = (n + 7) / 8;
    {
        int base = (int)G2_GetBG2ScrPtr();
        scr = (short*)(base + 0x40 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int v2 = m + 0x280;
        do {
            scr[i] = (i + 0x280);
            (scr + i)[0x20] = v2;
            v2++;
            i++;
        } while (i < m);
    }

    data_0209d6d4++;
    Message_DrawLineBG2();
    n = data_0209d6b0;
    m = (n + 7) / 8;
    {
        int base = (int)G2_GetBG2ScrPtr();
        scr = (short*)(base + 0x840 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209d674;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = v1;
            (scr + i)[0x20] = v2;
            v1++;
            v2++;
            i++;
        } while (i < mm);
    }

    data_0209d6d4++;
    Message_DrawLineBG2();
    n = data_0209d6b0;
    m = (n + 7) / 8;
    {
        int base = (int)G2_GetBG2ScrPtr();
        scr = (short*)(base + 0x1040 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209d674;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = v1;
            (scr + i)[0x20] = v2;
            v1++;
            v2++;
            i++;
        } while (i < mm);
    }
}
#pragma pop

void Message::DisplayControllerModeText(unsigned short n)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209d6d4 = n;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    data_0209d674 = 0;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    func_0201d418(0xa9, 0x70);
    data_0209d6d4 = data_0209d6d4 + 1;
    func_0201d418(0x149, 0x70);
    data_0209d6d4 = data_0209d6d4 + 1;
    func_0201d418(0x1e9, 0x70);
    data_0209d6a8 = data_0209d6a8 << 1;
    data_0209d6d4 = data_0209d6d4 + 1;
    Message_DrawCenteredLine(0x280, 0x20);
}

extern "C" void func_0201d418(int p0, int p1) {
    volatile int li;
    int i;
    int idx = data_0209d6d4;
    char *base = (char*)data_0209d708;
    int n = (p1 >> 3) & 0xff;
    data_0209d6f0 = (u8*)(base + idx * 8);
    data_0209d6f4 =
        data_0209d6fc + 0x28 + data_0209d70c[1] + ((MessageEntry *)(base + idx * 8))->textOffset;
    data_0209d6bc = 0;
    do {
        func_0201b6f8(0);
        data_0209d65c = (p1 - data_0209d6b0) >> 1;
        {
            int cp = (u32)G2S_GetBG0CharPtr() + 0x4000;
            li = 0;
            MI_CpuFill32((void *)(cp + (data_0209d6a8 << 6)), li, n << 6);
        }
        {
            short *scr = (short *)((u32)G2S_GetBG0ScrPtr() + p0 * 2);
            for (i = 0; i < n; i++) {
                scr[i] = (data_0209d6a8 << 1) + 0x200 + i;
                (scr + i)[0x20] = n + ((data_0209d6a8 << 1) + 0x200) + i;
            }
        }
        Message::LayoutLine();
        func_0201b388(n);
        data_0209d6c0 = 1;
        data_0209d6a8 = data_0209d6a8 + n;
        p0 = (unsigned short)(p0 + 0x40);
    } while (data_0209d6bc != 8);
    data_0209d6bc = 0;
}

/* Lays out the current message line into the main BG2 tiles at
   data_0209d674 and advances it. The caller writes the screen map. */
extern "C" void Message_DrawLineBG2(void) {
    volatile int li;
    int n;
    int idx = data_0209d6d4;
    char *base = (char*)data_0209d708;
    data_0209d6f0 = (u8*)(base + idx * 8);
    data_0209d6f4 =
        data_0209d6fc + 0x28 + data_0209d70c[1] + ((MessageEntry *)(base + idx * 8))->textOffset;
    func_0201b6f8(1);
    data_0209d65c = 0;
    n = ((data_0209d6b0 + 7) / 8) << 6;
    {
        int p = (int)G2_GetBG2CharPtr() + ((data_0209d674 + 0x280) << 5);
        li = 0;
        MI_CpuFill32((void *)p, li, n);
    }
    func_0201b100(1);
    data_0209d674 = data_0209d674 + (u8)(((data_0209d6b0 + 7) / 8) << 1);
}

/* Lays out the current message line on the sub screen's BG0, centred in a
   field width tiles wide that starts at map entry mapPos. Whole tiles of
   the centring go into the map position; the remaining pixels go into
   data_0209d65c as a shift. The line takes the next character tiles after
   data_0209d6a8, two map rows per line. */
extern "C" void Message_DrawCenteredLine(int mapPos, int width) {
    volatile int zero;
    int bytes;
    u8 *p;
    int i;
    int idx = data_0209d6d4;
    char *base = (char *)data_0209d708;
    u16 *map;
    int col;
    int old;

    data_0209d6f0 = (u8 *)(base + idx * 8);
    data_0209d6f4 =
        data_0209d6fc + 0x28 + data_0209d70c[1] + ((MessageEntry *)(base + idx * 8))->textOffset;
    func_0201b6f8(1);
    old = data_0209d6b0;
    data_0209d65c = ((width * 8 - old) & 0xf) >> 1;
    data_0209d6b0 = old + data_0209d65c;
    bytes = (data_0209d6b0 + 7) / 8 * 0x40;
    p = (u8 *)G2S_GetBG0CharPtr() + 0x4000 + (data_0209d6a8 << 5);
    col = width - (old + 7) / 8;
    zero = 0;
    MI_CpuFill32(p, zero, bytes);
    map = (u16 *)G2S_GetBG0ScrPtr() + mapPos + (u16)(col >> 1);
    for (i = 0; i < (data_0209d6b0 + 7) / 8; i++) {
        map[i] = data_0209d6a8 + 0x200 + i;
        (&map[i])[0x20] = data_0209d6a8 + 0x200 + (data_0209d6b0 + 7) / 8 + i;
    }

    func_0201b100(0);
    data_0209d6a8 += (u8)((data_0209d6b0 + 7) / 8 * 2);
}

/* Lays out the pause and level-clear titles on BG3. Each line is rendered
   into the next run of character tiles from 0x200 (the 0x100 tiles cleared
   below; 0x2ff, the last, is the blank the map is filled with) and centred
   in the 32-column map, two tile rows per line, from row 4. Which lines
   appear depends on the course and on data_0209f20c. A main course
   (course < 15) gets its number line, the course name and the star line.
   The star line leads with glyph 0xf0 when the star is collected or
   data_0209f20c is set, and with 0xf1 otherwise. */
extern "C" void func_0201d850(u32 course) {
    u16 tile = 0x200;
    u16 mapPos = 0x80;
    /* The fill values go through the stack, as the cartridge does. */
    volatile u16 blank;
    volatile s32 zero;
    volatile s32 zeroGlyphs;

    data_0209d660 = 0;
    func_0201eaac();
    data_0209d6a8 = 0;
    data_0209d65c = 0;

    {
        void *pa = (char *)G2_GetBG3CharPtr() + 0x4000;
        zero = 0;
        MI_CpuFill32(pa, zero, 0x2000);
    }
    {
        void *pb = G2_GetBG3ScrPtr();
        blank = 0x2ff;
        MI_CpuFill16(pb, blank, 0x800);
    }
    SetBg3Offset(0, 0);

    if (course < 0xf) {
        MessageEntry *ep = &data_0209d708[0x28d];
        s32 base;
        s32 num;
        s32 n;
        s32 i;
        u16 *p;

        data_0209d6f0 = (u8 *)ep;
        base = data_0209d6fc + 0x28 + data_0209d70c[1];
        data_0209d6f4 = base + ep->textOffset;
        Message::LayoutLine();
        Message::AddChar(0x4d);

        num = (course + 1) & 0xff;
        if (num / 10 != 0)
            Message::AddChar(data_0208ee74[num / 10]);
        Message::AddChar(data_0208ee74[num % 10]);

        data_0209d6a8 = (data_0209d6b0 + 7) / 8;
        func_0201b388(data_0209d6a8);
        data_0209d6c0 = 1;
        n = data_0209d6a8;
        p = (u16 *)G2_GetBG3ScrPtr() + (mapPos + ((0x20 - n) >> 1));
        for (i = 0; i < n; i++) {
            p[i] = (tile + i);
            (p + i)[0x20] = (n + (tile + i));
        }
        mapPos += 0x40;
        tile += (u16)(n * 2);
    }

    if (data_0209f20c != 0 && course < 0x15 || data_0209f20c == 0) {
        s32 n;
        s32 t;
        u16 *p;
        s32 i;

        if (data_0209f20c != 0 && course >= 0xf)
            mapPos = 0x100;
        else if (course >= 0xf)
            mapPos = 0x140;

        data_0209d65c = 0;
        data_0209d6a4 = 0;
        data_0209d6f0 = (u8 *)((char *)data_0209d708 + (course + 0x196) * 8);
        {
            s32 sum = data_0209d6fc + 0x28 + data_0209d70c[1] +
                      ((MessageEntry *)((char *)data_0209d708 + (course + 0x196) * 8))->textOffset;
            data_0209d6f4 = sum;
            data_0209d6f4 = sum + 3;
        }
        Message::LayoutLine();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)G2_GetBG3ScrPtr() + (mapPos + ((0x20 - n) >> 1));
        /* The counter is set up behind the loop's entry test, as written
           here; a for loop starts it ahead of the test (10 words differ). */
        i = 0;
        if (i < n) {
            t = tile;
            do {
                p[i] = t;
                (p + i)[0x20] = (n + t);
                t++;
                i++;
            } while (i < n);
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        tile += (u16)(n * 2);
        mapPos += 0x40;
    }

    if (course < 0xf || data_0209f20c != 0 && (course < 0xf || course >= 0x15)) {
        s32 n;
        s32 t;
        u16 *p;
        s32 i;

        zeroGlyphs = 0;
        data_0209d65c = 0;
        data_0209d678 = 0;
        data_0209d704 = 0;
        data_0209d6a4 = 0;
        data_0209d6b0 = 0;
        MI_CpuFill32(data_0209d74c, zeroGlyphs, 0xf00);
        data_0209d6b8 = 1;
        if (IsStarCollected(course, gCurrentStarId) || data_0209f20c != 0)
            Message::AddChar(0xf0);
        else
            Message::AddChar(0xf1);
        data_0209d65c += data_0208f074[0xf0];
        Message::AddChar(0x4d);
        data_0209d65c += data_0208f074[0x4d];
        if (course < 0xf) {
            if (data_0209f20c != 0) {
                s32 off = course * 7 + 0x1b3;
                data_0209d6f0 = (u8 *)&data_0209d708[off + gCollectedStarId];
            } else {
                s32 off = course * 7 + 0x1b3;
                data_0209d6f0 = (u8 *)&data_0209d708[off + gCurrentStarId];
            }
            mapPos = 0x140;
        } else {
            data_0209d6f0 = (u8 *)&data_0209d708[0x21d];
            mapPos = 0x100;
        }
        {
            s32 base = data_0209d6fc + 0x28 + data_0209d70c[1];
            data_0209d6f4 = base + ((MessageEntry *)data_0209d6f0)->textOffset;
        }
        Message::LayoutLine();
        data_0209d65c = 0;

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)G2_GetBG3ScrPtr() + (mapPos + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            t = tile;
            do {
                p[i] = t;
                (p + i)[0x20] = (n + t);
                t++;
                i++;
            } while (i < n);
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        tile += (u16)(n * 2);
        mapPos += 0x40;
    }

    if (data_0209f20c != 0) {
        s32 n;
        u16 *p;
        s32 i;

        if (course < 0xf || course >= 0x15)
            data_0209d6f0 = (u8 *)&data_0209d708[0x28c];
        else
            data_0209d6f0 = (u8 *)&data_0209d708[0x2b3];
        data_0209d6b8 = 0;
        {
            s32 base = data_0209d6fc + 0x28 + data_0209d70c[1];
            data_0209d6f4 = base + ((MessageEntry *)data_0209d6f0)->textOffset;
        }
        Message::LayoutLine();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)G2_GetBG3ScrPtr() + (mapPos + ((0x20 - n) >> 1));
        for (i = 0; i < n; i++) {
            p[i] = (tile + i);
            (p + i)[0x20] = (n + (tile + i));
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        return;
    }

    if (course >= 0xf)
        return;

    data_0209d6b8 = 0;
    {
        MessageEntry *ep = &data_0209d708[0x28e];
        s32 base;
        s32 n;
        u16 *p;
        s32 i;

        data_0209d6f0 = (u8 *)ep;
        base = data_0209d6fc + 0x28 + data_0209d70c[1];
        data_0209d6f4 = base + ep->textOffset;
        Message::LayoutLine();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)G2_GetBG3ScrPtr() + ((u16)(mapPos + 0x40) + ((0x20 - n) >> 1));
        for (i = 0; i < n; i++) {
            p[i] = (tile + i);
            (p + i)[0x20] = (n + (tile + i));
        }
    }
}

void Message::DisplayPauseText(unsigned short n, unsigned char b)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    func_0201d850(b);
    data_0209d6d4 = n;
    data_0209d6a8 = 0;
    data_0209d668 = 1;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    Message_DrawCenteredLine(0x80, 0x20);
    data_0209d6d4 = data_0209d6d4 + 1;
    Message_DrawCenteredLine(0x260, 0x20);
    data_0209d6d4 = data_0209d6d4 + 1;
    Message_DrawCenteredLine(0x120, 0x20);
    data_0209d6d4 = 0x28b;
    Message_DrawCenteredLine(0x1c0, 0x20);
}

void Message::DisplayDontSaveText(unsigned short n)
{
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    data_0209d6d4 = n;
    func_0201d418(0x105, 0xb0);
    data_0209d6d4 = 0x293;
    data_0209d6a8 = data_0209d6a8 << 1;
    Message_DrawCenteredLine(0x285, 0xa);
    data_0209d6d4 = data_0209d6d4 + 1;
    Message_DrawCenteredLine(0x291, 0xa);
}

extern "C" void func_0201e220(int arg) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    if (arg == 0) {
        Message_DrawCenteredLine(0x160, 0x20);
        data_0209d6d4 = data_0209d6d4 + 1;
        Message_DrawCenteredLine(0x200, 0x20);
        data_0209d6d4 = data_0209d6d4 + 1;
        Message_DrawCenteredLine(0xc0, 0x20);
    } else {
        Message_DrawCenteredLine(0xc0, 0x20);
        data_0209d6d4 = data_0209d6d4 + 1;
        Message_DrawCenteredLine(0x160, 0x20);
        data_0209d6d4 = data_0209d6d4 + 1;
        Message_DrawCenteredLine(0x200, 0x20);
    }
}

void Message::DisplaySaveMenuText(u16 msgID)
{
    data_0209d6d4 = msgID;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d6a8 = 0;
    data_0209d668 = 1;
    func_0201e220(0);
}

void Message::DisplayText(int t) {
    volatile int li;
    volatile unsigned short ls;
    int idx;
    int p, s;
    int i;
    u16 *map;
    int cnt, mapPos;
    int base;

    data_0209d6d4 = t;
    data_0209d660 = 0;
    func_0201eaac();

    base = data_0209d6fc;
    idx = data_0209d6d4;
    gMsgBoxStyle = 1;
    data_0209d6c0 = 0;
    data_0209d668 = 0;
    data_0209d6a8 = 0;
    data_0209d6f0 = (u8*)((char*)data_0209d708 + idx * 8);
    data_0209d6f4 = base + 0x28 + data_0209d70c[1] +
                    ((MessageEntry *)((char *)data_0209d708 + idx * 8))->textOffset;

    p = (int)G2_GetBG3CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);

    s = (int)G2_GetBG3ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);

    if (t != 0x1b3) {
        func_0201b6f8(0);
        data_0209d6a8 = ((data_0209d6b0 + 7) / 8);
        data_0209d65c = 0;
        mapPos = 0x107;
    } else {
        data_0209d6f4 += 3;
        func_0201b6f8(0);
        data_0209d65c = ((0xb0 - data_0209d6b0) >> 1);
        data_0209d6a8 = 0x16;
        data_0209d678 = data_0209d65c & 7;
        mapPos = 0x105;
    }

    map = (u16 *)((char *)G2_GetBG3ScrPtr() + mapPos * 2);
    cnt = data_0209d6a8;
    for (i = 0; i < cnt; i++) {
        u16 *lower = map + 0x20;
        map[i] = (i + 0x200);
        lower[i] = (i + 0x200 + cnt);
    }

    LayoutLine();
    func_0201b388(data_0209d6a8);
}

void Message::Display(unsigned int msg)
{
    volatile int li;
    volatile unsigned short ls;
    volatile int li2;
    volatile unsigned short ls2;
    int p, s;
    data_0209d660 = 0;
    func_0201eaac();
    p = (int)G2_GetBG3CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (int)G2_GetBG3ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    DisplayText(msg);
    data_0209d6d4 = 0x276;
    gMsgBoxStyle = 0;
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li2 = 0;
    MI_CpuFill32((void *)p, li2, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls2 = 0x2ff;
    MI_CpuFill16((void *)s, ls2, 0x800);
    Message_DrawCenteredLine(0xa0, 0x20);
    data_0209d6d4 = 0x279;
    Message_DrawCenteredLine(0x140, 0x20);
    data_0209d6d4 = 0x28b;
    Message_DrawCenteredLine(0x1e0, 0x20);
    data_0209d6d4 = 0x28f;
    Message_DrawCenteredLine(0x280, 0x20);
}

void Message::DisplayStarNameForStarSelect(unsigned int msg)
{
    volatile int li;
    int p;
    int idx;
    u16* q;
    int i;

    data_0209d6d4 = (msg + (SublevelToLevel(gNextSublevel) * 7 + 0x1b4));
    data_0209d660 = 0;
    func_0201eaac();

    idx = data_0209d6d4;
    data_0209d6c0 = 2;
    gMsgBoxStyle = 1;
    data_0209d6f0 = (u8*)((char*)data_0209d708 + idx * 8);
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] +
                    ((MessageEntry *)((char *)data_0209d708 + idx * 8))->textOffset;
    func_0201b6f8(0);

    data_0209d65c = (0x100 - (int)data_0209d6b0) / 2;
    data_0209d6a8 = 0x20;

    p = (int)G2_GetBG3CharPtr() + 0x5000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x800);

    q = (u16 *)((char *)G2_GetBG3ScrPtr() + 0x180);
    for (i = 0; i < 0x40; i++) {
        q[i] = (i + 0x280);
    }

    LayoutLine();
    func_0201b388(data_0209d6a8);
}

/* Shows the course name on the star-select screen (message base + 0x196)
   on BG3, centred across the screen in a two-row strip of map entries.
   Main courses (message < 0x1a5) also get a second line, message 0x28e,
   centred in 0xd2 pixels. */
void Message::DisplayCourseNameForStarSelect(u32 base)
{
    volatile unsigned short blank;
    volatile int zero;
    volatile int zero2;
    int idx;
    int sum;
    u16 *map;
    int i;
    int j;

    data_0209d6d4 = (base + 0x196);
    data_0209d660 = 0;
    func_0201eaac();

    idx = data_0209d6d4;
    data_0209d6f0 = (u8*)((char*)data_0209d708 + idx * 8);
    sum = data_0209d6fc + 0x28 + data_0209d70c[1] +
          ((MessageEntry *)((char *)data_0209d708 + idx * 8))->textOffset;
    data_0209d6f4 = sum;
    data_0209d6f4 = sum + 3;
    func_0201b6f8(0);

    data_0209d65c = ((0x100 - data_0209d6b0) / 2);
    data_0209d6a8 = 0x20;

    {
        void *chars = (char *)G2_GetBG3CharPtr() + 0x4000;
        zero = 0;
        MI_CpuFill32(chars, zero, 0x2000);
    }
    {
        void *scr = G2_GetBG3ScrPtr();
        blank = 0x2ff;
        MI_CpuFill16(scr, blank, 0x800);
    }

    if (data_0209d6d4 >= 0x1a5) {
        gMsgBoxStyle = 0;
        map = (u16 *)((char *)G2_GetBG3ScrPtr() + 0x400);
    } else {
        gMsgBoxStyle = 1;
        map = (u16 *)((char *)G2_GetBG3ScrPtr() + 0x4c0);
    }
    for (i = 0; i < 0x40; i++)
        map[i] = i + 0x200;

    SetSubBg1Offset(0, 0);
    LayoutLine();
    func_0201b388(data_0209d6a8);

    if (data_0209d6d4 >= 0x1a5)
        return;

    data_0209d6c0 = 1;
    {
        MessageEntry *e = &data_0209d708[0x28e];
        data_0209d6f0 = (u8*)e;
        sum = data_0209d6fc + 0x28;
        sum += data_0209d70c[1];
        sum += e->textOffset;
        data_0209d6f4 = sum;
    }
    func_0201b6f8(0);

    data_0209d65c = ((0xd2 - data_0209d6b0) / 2);
    data_0209d6a8 = 0x20;

    {
        void *chars = (u8 *)G2_GetBG3CharPtr() + 0x4800;
        zero2 = 0;
        MI_CpuFill32(chars, zero2, 0x800);
    }

    map = (u16 *)((char *)G2_GetBG3ScrPtr() + 0x240);
    for (j = 0; j < 0x40; j++)
        map[j] = j + 0x240;

    LayoutLine();
    func_0201b388(data_0209d6a8);
}

extern "C" void func_0201eaac(void)
{
    data_0209d6c0 = 0;
    data_0209d6b4 = 0;
    data_0209d69c = 0;
    data_0209d694 = 0;
    data_0209d6bc = 1;
    gMsgChoiceIndex = 0;
    data_0209d684 = 0;
    data_0209d6b8 = 0;
    gMsgBoxStyle = 0;
    data_0209d668 = 0;
    data_0209d654 = 0;
    data_0209d67c = 0;
    data_0209d6a8 = 0;
    data_0209d65c = 0;
    data_0209d66c = 0;
    data_0209d690 = 0;
    data_0209d670 = 0;
}

void Message::DisplaySaving(unsigned short n)
{
    volatile int zero;
    volatile unsigned short blank;
    u8 *map;
    int i;
    int cols;
    int rem;
    int idx;
    unsigned tiles;
    MessageEntry *base;
    MessageEntry *entry;

    data_0209d6d4 = n;
    func_0201eaac();

    data_0209d67c = 0x3c;
    data_0209d660 = 1;
    data_0209d654 = 1;
    gMsgBoxWinLeft = 0x7f;
    gMsgBoxWinTop = 0x87;
    gMsgBoxStyle = 0;
    func_02012790(4);

    idx = data_0209d6d4;
    base = data_0209d708;
    entry = (MessageEntry *)((char *)base + idx * 8);
    data_0209d6f0 = (u8 *)entry;
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] +
                    ((MessageEntry *)((char *)base + idx * 8))->textOffset;

    tiles = entry->width / 8;
    data_0209d6a8 = ((int)(tiles * 9 + 7) / 8); /* an eighth wider than the text */
    gMsgBoxLeft = ((0x20 - (data_0209d6a8 + 2)) / 2 * 8);
    gMsgBoxCenterX = 0x7f;
    gMsgBoxCenterY = 0x43;
    gMsgBoxTop = (0x44 - (entry->lineCount * 2 + 1) * 8 / 2);
    gMsgBoxWinLeft = 0x7f;
    gMsgBoxWinTop = 0x43;
    data_0209d680 = 0;
    data_0209d65c = 0;

    {
        void *chars = (u8 *)G2_GetBG3CharPtr() + 0x4000;
        zero = 0;
        MI_CpuFill32(chars, zero, 0x2000);
    }
    {
        void *scr = G2_GetBG3ScrPtr();
        blank = 0x2ff;
        MI_CpuFill16(scr, blank, 0x800);
    }

    map = (u8 *)G2_GetBG3ScrPtr() + 0x42;
    SetBg3Offset(-gMsgBoxLeft, 4 - gMsgBoxTop);

    cols = data_0209d6a8;
    for (i = 0; i < cols * ((MessageEntry *)data_0209d6f0)->lineCount * 2; i++) {
        rem = i % cols;
        *(u16 *)(map + rem * 2) = i + 0x200;
        if (cols - 1 == rem)
            map += 0x40;
    }

    while (data_0209d6bc < 4)
        Message::Update();

    {
        int b = gMainScreenLayers | 0x20;
        gNextActorUpdateMask |= 0x80000000u;
        G2_SetBlendBrightness(b, -8);
    }

    {
        u8 planes = gMainScreenLayers;
        G2_SetWnd0InsidePlane(planes | GX_WND_PLANEMASK_BG3, 1);
        G2_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                                  GX_WND_PLANEMASK_OBJ,
                              0);
    }

    G2_SetWnd0Position(gMsgBoxWinLeft, gMsgBoxWinTop, gMsgBoxCenterX * 2 - gMsgBoxWinLeft + 1,
                       gMsgBoxCenterY * 2 - gMsgBoxWinTop + 1);
    GX_SetVisibleWnd(GX_WNDMASK_W0);
    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    GX_SetVisiblePlane(gMainScreenLayers);
}

extern "C" void func_0201ef38(void) {
    gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
}

/* Shows a credit text (message arg + 0x24e) on the sub screen's BG0: clears
   all of BG0's character tiles, then lays the message out one 32-tile line
   at a time, centred vertically on the screen. A negative arg only clears. */
extern "C" void func_0201ef50(int arg)
{
    volatile int zero;
    volatile int lineZero;
    int lines;
    int top;
    int idx;
    int charOffset;
    char *chars;
    char *line;

    func_0201eaac();
    data_0209d660 = 0;
    gMsgBoxStyle = 0;
    data_0209d690 = 1;
    chars = (char *)G2S_GetBG0CharPtr();
    zero = 0;
    MI_CpuFill32(chars, zero, 0x8000);
    if (arg < 0)
        return;

    data_0209d6d4 = (arg + 0x24e);
    idx = data_0209d6d4;
    data_0209d6f0 = (u8 *)((char *)data_0209d708 + idx * 8);
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] +
                    ((MessageEntry *)((char *)data_0209d708 + idx * 8))->textOffset;
    data_0209d6a8 = 0x20;
    data_0209d680 = 0;
    data_0209d65c = 0;
    lines = ((MessageEntry *)((char *)data_0209d708 + idx * 8))->lineCount;
    top = 0xc - lines;
    charOffset = (u16)(top << 10);
    data_0209d6e0 = (top << 5);
    while (data_0209d6bc != 8) {
        func_0201b6f8(0);
        data_0209d65c = (0x100 - data_0209d6b0) >> 1;
        line = (char *)G2S_GetBG0CharPtr() + charOffset;
        lineZero = 0;
        MI_CpuFill32(line, lineZero, 0x800);
        Message::LayoutLine();
        func_0201b388(0x20);
        charOffset = (u16)(charOffset + 0x800);
        data_0209d6e0 = data_0209d6e0 + 0x40;
    }
}

extern "C" void func_0201f108(void) {
    u16 *scr = (u16 *)G2S_GetBG0ScrPtr();
    int i = 0;
    do {
        scr[i] = i;
        i++;
    } while (i < 0x400);
}

extern "C" void func_0201f138(void)
{
    volatile int li;
    volatile unsigned short ls;
    int idx;
    int p, s;
    u16* q;
    int i, cnt, rem, div;
    u8* bcPtr;

    data_0209d6d4 = 0;
    func_0201eaac();

    idx = data_0209d6d4;
    gMsgBoxWinLeft = 0x7f;
    gMsgBoxWinTop = 0x87;
    data_0209d660 = 0;
    gMsgBoxStyle = 1;
    data_0209d6f0 = (u8*)((char*)data_0209d708 + idx * 8);
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] +
                    ((MessageEntry *)((char *)data_0209d708 + idx * 8))->textOffset;

    data_0209d6a8 = ((((((MessageEntry *)data_0209d6f0)->width / 8) * 9) + 7) / 8);
    data_0209d680 = 0;
    data_0209d65c = 0;

    p = (int)G2_GetBG3CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);

    s = (int)G2_GetBG3ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);

    q = (u16 *)((char *)G2_GetBG3ScrPtr() + 0x42);
    div = data_0209d6a8;
    cnt = div * ((MessageEntry *)data_0209d6f0)->lineCount * 2;
    for (i = 0; i < cnt; i++) {
        rem = i % div;
        q[rem] = (i + 0x200);
        if (div - 1 == rem) {
            q += 0x20;
        }
        cnt = div * ((MessageEntry *)data_0209d6f0)->lineCount * 2;
    }

    bcPtr = &data_0209d6bc;
    while (*bcPtr < 4) {
        Message::Update();
    }
    SetBg3Offset(-0x11, -0x3c);
    gMainScreenLayers |= DISPLAY_LAYER_BG3;
}

static inline int IsKuppaScriptRunning() {
    return gRunningKuppaScript != 0;
}

static inline int IsGameMode(int mode) {
    return gGameMode == mode;
}

/* Opens message arg0. Places the box one of three ways: against the left edge in
   game mode 0 before the intro is done (unless a Kuppa script runs), low on
   the screen in game mode 2, and centred otherwise. Then clears the target
   screen's text tiles and map, lays the box's tiles out row by row from tile
   0x200 (0x2200 on the sub screen), and pumps Message::Update until the
   open state reaches 4. */
extern "C" void func_0201f32c(int arg0)
{
    MessageEntry *entry;
    u8 disp2;
    u8 t;
    u16 *map;
    s32 tile;
    u8 divisor;
    s32 i;
    /* Each screen's fill values go through their own stack slots, as the cartridge does. */
    volatile u16 mainBlank;
    volatile u16 subBlank;
    volatile u16 bg3Blank;
    volatile s32 mainZero;
    volatile s32 subZero;
    volatile s32 bg3Zero;

    if (((MessageInfoSection *)data_0209d70c)->messageCount <= (u16)arg0)
        return;

    data_0209d6d4 = arg0;
    func_0201eaac();

    data_0209d660 = 1;
    gMsgBoxWinLeft = 0x7f;
    gMsgBoxWinTop = 0x87;
    if (data_0209d6d4 >= 0x2a && data_0209d6d4 < 0x83) {
        gMsgBoxStyle = 1;
        func_02012790(0x20);
    } else {
        gMsgBoxStyle = 0;
        if (!IsGameMode(2))
            func_02012790(4);
    }

    entry = &data_0209d708[data_0209d6d4];
    data_0209d6f0 = (u8 *)entry;
    {
        s32 ev = entry->textOffset;
        s32 sum = data_0209d6fc + 0x28 + data_0209d70c[1];
        data_0209d6f4 = sum + ev;
    }
    data_0209d6a8 = ((entry->width + 7) / 8);

    if (IsGameMode(0) && !(data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) &&
        !IsKuppaScriptRunning()) {
        gMsgBoxLeft = 0x10;
        gMsgBoxTop = 0x64;
        gMsgBoxCenterX = ((((data_0209d6a8 + 2) / 2) * 8) + 0xf);
        gMsgBoxCenterY = ((((entry->lineCount * 2 + 1) * 8) / 2) + 0x63);
        gMsgBoxWinLeft = gMsgBoxCenterX;
        gMsgBoxWinTop = gMsgBoxCenterY;
    } else if (IsGameMode(2)) {
        gMsgBoxLeft = (((0x20 - (data_0209d6a8 + 2)) / 2) * 8);
        gMsgBoxCenterX = 0x7f;
        gMsgBoxTop = 0xa0;
        gMsgBoxCenterY = ((((entry->lineCount) * 16) / 2) + 0x9f);
        gMsgBoxWinLeft = 0x7f;
        gMsgBoxWinTop = gMsgBoxCenterY;
    } else {
        gMsgBoxLeft = (((0x20 - (data_0209d6a8 + 2)) / 2) * 8);
        gMsgBoxCenterX = 0x7f;
        gMsgBoxCenterY = 0x63;
        gMsgBoxTop = (0x64 - (((entry->lineCount * 2 + 2) * 8) / 2));
        gMsgBoxWinLeft = 0x7f;
        gMsgBoxWinTop = 0x63;
        if (func_0201fb4c() != 0)
            Sound::ChangeMusicVolume(0x40, 0xc999);
    }

    data_0209d680 = 0;
    data_0209d65c = 0;
    if (data_0209d698 == 2) {
        {
            void *pa = (char *)G2_GetBG1CharPtr() + 0x4000;
            mainZero = 0;
            MI_CpuFill32(pa, mainZero, 0x4000);
        }
        {
            void *pb = (char *)G2_GetBG1ScrPtr() + 0x200;
            mainBlank = 0x3ff;
            MI_CpuFill16(pb, mainBlank, 0x400);
        }
        map = (u16 *)((char *)G2_GetBG1ScrPtr() + 0x24e);
        SetBg1Offset(0, 0);
        tile = 0x200;
        data_0209d660 = 0;
    } else if (data_0209d698 == 1) {
        {
            void *pa = (char *)G2S_GetBG1CharPtr() + 0x4000;
            subZero = 0;
            MI_CpuFill32(pa, subZero, 0x2000);
        }
        {
            void *pb = G2S_GetBG1ScrPtr();
            subBlank = 0x22ff;
            MI_CpuFill16(pb, subBlank, 0x800);
        }
        map = (u16 *)((char *)G2S_GetBG1ScrPtr() + 0x42);
        SetSubBg1Offset(4 - gMsgBoxLeft, 4 - gMsgBoxTop);
        tile = 0x2200;
    } else {
        {
            void *pa = (u8 *)G2_GetBG3CharPtr() + 0x4000;
            bg3Zero = 0;
            MI_CpuFill32(pa, bg3Zero, 0x3000);
        }
        {
            void *pb = G2_GetBG3ScrPtr();
            bg3Blank = 0x37f;
            MI_CpuFill16(pb, bg3Blank, 0x800);
        }
        map = (u16 *)((char *)G2_GetBG3ScrPtr() + 0x42);
        if (IsGameMode(2))
            SetBg3Offset(-gMsgBoxLeft, 8 - gMsgBoxTop);
        else
            SetBg3Offset(-gMsgBoxLeft, 4 - gMsgBoxTop);
        tile = 0x200;
    }

    divisor = data_0209d6a8;
    for (i = 0; i < (s32)(divisor * ((MessageEntry *)data_0209d6f0)->lineCount * 2); i++) {
        s32 rem = i % divisor;
        map[rem] = tile;
        if (divisor - 1 == rem)
            map += 0x20;
        tile++;
    }

    while ((t = data_0209d6bc) < 4)
        Message::Update();
    data_0209d6ac = t;

    disp2 = data_0209d698;
    if (disp2 == 2) {
        gMainScreenLayers |= DISPLAY_LAYER_BG1;
        GX_SetVisiblePlane(gMainScreenLayers);
        return;
    }
    if (disp2 == 1) {
        G2S_SetBlendBrightness(gSubScreenLayers | 0x20, -7);
        G2S_SetWnd0InsidePlane(gSubScreenLayers | GX_WND_PLANEMASK_BG1, 1);
        G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 |
                                   GX_WND_PLANEMASK_OBJ,
                               0);
        G2S_SetWnd0Position(gMsgBoxWinLeft, gMsgBoxWinTop, gMsgBoxCenterX * 2 - gMsgBoxWinLeft + 1,
                            gMsgBoxCenterY * 2 - gMsgBoxWinTop + 1);
        GXS_SetVisibleWnd(GX_WNDMASK_W0);
        gSubScreenLayers |= 2;
        GXS_SetVisiblePlane(gSubScreenLayers);
        return;
    }

    if (data_0209d6d4 >= 0x2a && data_0209d6d4 < 0x83) {
        G2_SetBlendBrightness(gMainScreenLayers & ~0x38, 7);
    } else {
        G2_SetBlendBrightness(gMainScreenLayers & ~0x38, -8);
    }
    G2_SetWnd0InsidePlane(gMainScreenLayers | GX_WND_PLANEMASK_BG3, 1);
    G2_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                              GX_WND_PLANEMASK_OBJ,
                          0);
    G2_SetWnd0Position(gMsgBoxWinLeft, gMsgBoxWinTop, gMsgBoxCenterX * 2 - gMsgBoxWinLeft + 1,
                       gMsgBoxCenterY * 2 - gMsgBoxWinTop + 1);
    data_0209d6bc = 0;
    GX_SetVisibleWnd(GX_WNDMASK_W0);
    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    GX_SetVisiblePlane(gMainScreenLayers);
}

/* Course ids that must not open the talk window. data_0209d664 is the byte
   after the shared reset flag at data_0209d660. */
extern "C" int func_0201fb4c(void)
{
    short v;
    unsigned int b;
    if (data_0209d664 != 0) goto no;
    b = (gGameMode == 2);
    if (b != 0) goto no;
    v = data_0209d6d4;
    if (v == 0xcc) goto no;
    if (v == 0xce) goto no;
    if (v == 0xd0) goto no;
    if (v == 0xd3) goto no;
    if (v == 0xd4) goto no;
    if (v == 0xd5) goto no;
    if (v == 0xd6) goto no;
    if (v == 0xdb) goto no;
    if (v == 0xdc) goto no;
    if (v == 0xdd) goto no;
    if (v == 0xde) goto no;
    if (v == 0xe3) goto no;
    if (v == 0xe4) goto no;
    if (v == 0xe5) goto no;
    if (v == 0xe6) goto no;
    return 1;
no:
    return 0;
}

struct Pair { unsigned short v; unsigned short pad; };

extern "C" int ObjectMessageIDToActualMessageID(s16 id) {
    int i;
    extern struct Pair data_0208eeec[];
    extern struct Pair data_0208eeee[];
    for (i = 0x61; i > 0; i--) {
        if (data_0208eeec[i].v > id && data_0208eeec[i - 1].v <= id) {
            return (unsigned short)((id - data_0208eeec[i - 1].v) + data_0208eeee[i - 1].v);
        }
    }
    return id;
}

extern "C" void func_0201fc88(s16 objectMsgID)
{
    s16 id = ObjectMessageIDToActualMessageID(objectMsgID);
    func_0201f32c(id);
}

void Message::ResetAllGlobals()
{
    func_0201eaac();
    data_0209d660 = 0;
}
