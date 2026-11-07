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
 * - BG pointer getters are G2:: / G2S:: calls. Their return types stay the
 *   ones these call sites already use; no header spelling matches them.
 * - Address-named helpers stay address-named. The ROM has no English name
 *   for them.
 * - dScStage_c::UpdateMessage, the stage's per-frame message driver, opens
 *   the file: it is first by address (0x0201cb7c).
 */

#pragma defer_codegen off
#include "types.h"
#include "nitro/hw/registers.h"
#include "Message.h"
#include "G2x.h"
#include "SaveData.h"
#include "dScStage_c.h"
#include "Sound.h"
#include "MessageBank.h"

namespace G2 {
void *GetBG1ScrPtr(void);
void *GetBG2CharPtr(void);
void *GetBG2ScrPtr(void);
void *GetBG3ScrPtr(void);
}

namespace G2S {
u32 GetBG0CharPtr(void);
u32 GetBG0ScrPtr(void);
u32 GetBG1CharPtr(void);
u32 GetBG1ScrPtr(void);
}

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
extern u8 data_0209f220;
extern u8 gCollectedStarId;
extern u8 data_0209f2d8;
extern SaveBlock data_0209caa0;
extern s32 gRunningKuppaScript;
extern s8 data_02092110;
extern u32 gNextActorUpdateMask;
extern s32 data_0208ee44;

extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void SetBg1Offset(int a, int b);
extern void SetBg3Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void func_0201b6f8(int a);
extern void func_0201b388(int a);
extern void func_0201b100(int a);
extern int func_0201fb4c(void);
extern int func_02054d88(void);
extern void *func_02054ea8(void);
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
void Message_DrawCenteredLine(int a, int b);
void func_0201d850(u32 arg0);
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
    p = G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int *)p, 0x2000);
    s = G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char *)s, 0x800);
    data_0209d6d4 = (s16)arg;
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
    p = G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);
    s = G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);
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
        int *dst = (int *)(G2S::GetBG0CharPtr() + 0x4000);
        z = 0;
        MultiStore_Int(z, dst, 0x2000);
    }
    {
        char *dst = (char *)G2S::GetBG0ScrPtr();
        v = 0x2ff;
        MultiStore16(v, dst, 0x800);
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
    data_0209d6d4 = (u16)arg0;
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
    p = G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);
    s = G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);
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

    data_0209d6d4 = (short)param0;
    data_0209d660 = 0;
    func_0201eaac();

    data_0209d668 = 1;
    data_0209d6a8 = 0;
    data_0209d674 = 0;
    Message_DrawLineBG2();

    n = data_0209d6b0;
    m = (n + 7) / 8;
    {
        int base = (int)G2::GetBG2ScrPtr();
        scr = (short*)(base + 0x40 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int v2 = m + 0x280;
        do {
            scr[i] = (short)(i + 0x280);
            (scr + i)[0x20] = (short)v2;
            v2++;
            i++;
        } while (i < m);
    }

    data_0209d6d4++;
    Message_DrawLineBG2();
    n = data_0209d6b0;
    m = (n + 7) / 8;
    {
        int base = (int)G2::GetBG2ScrPtr();
        scr = (short*)(base + 0x840 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209d674;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = (short)v1;
            (scr + i)[0x20] = (short)v2;
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
        int base = (int)G2::GetBG2ScrPtr();
        scr = (short*)(base + 0x1040 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209d674;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = (short)v1;
            (scr + i)[0x20] = (short)v2;
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
    p = G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);
    s = G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);
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
            int cp = G2S::GetBG0CharPtr() + 0x4000;
            li = 0;
            MultiStore_Int(li, (int*)(cp + (data_0209d6a8 << 6)), n << 6);
        }
        {
            short *scr = (short *)(G2S::GetBG0ScrPtr() + p0 * 2);
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
        int p = (int)G2::GetBG2CharPtr() + ((data_0209d674 + 0x280) << 5);
        li = 0;
        MultiStore_Int(li, (int*)p, n);
    }
    func_0201b100(1);
    data_0209d674 = data_0209d674 + (u8)(((data_0209d6b0 + 7) / 8) << 1);
}

extern "C" void Message_DrawCenteredLine(int a, int b)
{
    int new_var5;
    volatile int li;
    int n;
    int new_var4;
    int p;
    int i;
    int idx = data_0209d6d4;
    int new_var;
    char *base = (char *) data_0209d708;
    u16 *ptr;
    int new_var3;
    int new_var2;
    int col;
    int cnt;
    int old;
    data_0209d6f0 = (u8*) (base + (idx * 8));
    data_0209d6f4 = ((data_0209d6fc + 0x28) + data_0209d70c[1]) +
                    ((MessageEntry *)(base + (idx * 8)))->textOffset;
    new_var4 = 1;
    func_0201b6f8(new_var4);
    old = data_0209d6b0;
    data_0209d65c = (((b * 8) - old) & 0xf) >> new_var4;
    data_0209d6b0 = old + data_0209d65c;
    n = ((data_0209d6b0 + 7) / 8) << 6;
    new_var5 = (G2S::GetBG0CharPtr() + 0x4000) + (data_0209d6a8 << 5);
    p = new_var5;
    new_var3 = b - ((old + 7) / 8);
    new_var2 = new_var3;
    li = 0;
    MultiStore_Int(li, (int*)p, n);
    col = (u16) (new_var2 >> 1);
    old = old;
    ptr = ((u16 *)(G2S::GetBG0ScrPtr() + (a * 2))) + col;
    cnt = data_0209d6b0;
    cnt = (cnt + 7) / 8;
    for (i = 0; i < cnt; i++)
    {
        new_var = 0x200;
        ptr[i] = (u16) ((data_0209d6a8 + new_var) + i);
        (ptr + i)[0x20] = (u16) (((data_0209d6a8 + 0x200) + cnt) + i);
    }

    func_0201b100(0);
    data_0209d6a8 = data_0209d6a8 + ((u8) (((data_0209d6b0 + 7) / 8) << new_var4));
}

extern "C" void func_0201d850(u32 arg0)
{
    u16 var_r5;
    u16 var_r4;
    volatile u16 sp0;
    volatile s32 sp4;
    volatile s32 sp8;

    data_0209d660 = 0;
    var_r5 = 0x200;
    var_r4 = 0x80;
    func_0201eaac();
    data_0209d6a8 = 0;
    data_0209d65c = 0;

    {
        void *pa = (char *)func_02054d88() + 0x4000;
        sp4 = 0;
        MultiStore_Int(sp4, (int *)pa, 0x2000);
    }
    {
        void *pb = G2::GetBG3ScrPtr();
        sp0 = 0x2ff;
        MultiStore16(sp0, (char *)pb, 0x800);
    }
    SetBg3Offset(0, 0);

    if (arg0 < 0xf) {
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

        num = (arg0 + 1) & 0xff;
        if (num / 10 != 0) {
            Message::AddChar(data_0208ee74[num / 10]);
        }
        Message::AddChar(data_0208ee74[num % 10]);

        data_0209d6a8 = (data_0209d6b0 + 7) / 8;
        func_0201b388(data_0209d6a8);
        data_0209d6c0 = 1;
        n = data_0209d6a8;
        p = (u16 *)G2::GetBG3ScrPtr() + (var_r4 + ((0x20 - n) >> 1));
        for (i = 0; i < n; i++) {
            p[i] = (u16)(var_r5 + i);
            (p + i)[0x20] = (u16)(n + (var_r5 + i));
        }
        var_r4 += 0x40;
        var_r5 += (u16)(n * 2);
    }

    if (((data_0209f20c != 0) && (arg0 < 0x15)) || (data_0209f20c == 0)) {
        s32 n;
        s32 tile;
        u16 *p;
        s32 i;

        if (data_0209f20c != 0) {
            if (arg0 >= 0xf) {
                var_r4 = 0x100;
            } else {
                goto L_r4_else;
            }
        } else {
L_r4_else:
            if (arg0 >= 0xf) var_r4 = 0x140;
        }

        data_0209d65c = 0;
        data_0209d6a4 = 0;
        data_0209d6f0 = (u8 *)(
            (char *)data_0209d708 + (arg0 + 0x196) * 8);
        {
            s32 sum = data_0209d6fc + 0x28 + data_0209d70c[1] +
                      ((MessageEntry *)((char *)data_0209d708 + (arg0 + 0x196) * 8))->textOffset;
            data_0209d6f4 = sum;
            data_0209d6f4 = sum + 3;
        }
        Message::LayoutLine();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)G2::GetBG3ScrPtr() + (var_r4 + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            tile = var_r5;
            do {
                p[i] = (u16)tile;
                (p + i)[0x20] = (u16)(n + tile);
                tile++;
                i++;
            } while (i < n);
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        var_r5 += (u16)(n * 2);
        var_r4 += 0x40;
    }

    if ((arg0 < 0xf) || ((data_0209f20c != 0) && ((arg0 < 0xf) || (arg0 >= 0x15)))) {
        s32 n;
        s32 tile;
        u16 *p;
        s32 i;

        sp8 = 0;
        data_0209d65c = 0;
        data_0209d678 = 0;
        data_0209d704 = 0;
        data_0209d6a4 = 0;
        data_0209d6b0 = 0;
        MultiStore_Int(sp8, data_0209d74c, 0xf00);
        data_0209d6b8 = 1;
        if ((IsStarCollected(arg0, data_0209f220) != 0) || (data_0209f20c != 0)) {
            Message::AddChar(0xf0);
        } else {
            Message::AddChar(0xf1);
        }
        data_0209d65c += data_0208f074[0xf0];
        Message::AddChar(0x4d);
        data_0209d65c += data_0208f074[0x4d];
        if (arg0 < 0xf) {
            if (data_0209f20c != 0) {
                s32 off = arg0 * 7 + 0x1b3;
                data_0209d6f0 = (u8 *)&data_0209d708[off + gCollectedStarId];
            } else {
                s32 off = arg0 * 7 + 0x1b3;
                data_0209d6f0 = (u8 *)&data_0209d708[off + data_0209f220];
            }
            var_r4 = 0x140;
        } else {
            data_0209d6f0 = (u8 *)&data_0209d708[0x21d];
            var_r4 = 0x100;
        }
        {
            s32 base = data_0209d6fc + 0x28 + data_0209d70c[1];
            data_0209d6f4 = base + ((MessageEntry *)data_0209d6f0)->textOffset;
        }
        Message::LayoutLine();
        data_0209d65c = 0;

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)G2::GetBG3ScrPtr() + (var_r4 + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            tile = var_r5;
            do {
                p[i] = (u16)tile;
                (p + i)[0x20] = (u16)(n + tile);
                tile++;
                i++;
            } while (i < n);
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        var_r5 += (u16)(n * 2);
        var_r4 += 0x40;
    }

    if (data_0209f20c != 0) {
        s32 n;
        s32 tile;
        u16 *p;
        s32 i;

        if ((arg0 < 0xf) || (arg0 >= 0x15)) {
            data_0209d6f0 = (u8 *)&data_0209d708[0x28c];
        } else {
            data_0209d6f0 = (u8 *)&data_0209d708[0x2b3];
        }
        data_0209d6b8 = 0;
        {
            s32 base = data_0209d6fc + 0x28 + data_0209d70c[1];
            data_0209d6f4 = base + ((MessageEntry *)data_0209d6f0)->textOffset;
        }
        Message::LayoutLine();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)G2::GetBG3ScrPtr() + (var_r4 + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            tile = var_r5;
            do {
                p[i] = (u16)tile;
                (p + i)[0x20] = (u16)(n + tile);
                tile++;
                i++;
            } while (i < n);
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        return;
    }

    if (arg0 >= 0xf) {
        return;
    }

    data_0209d6b8 = 0;
    {
        MessageEntry *ep = &data_0209d708[0x28e];
        s32 base;
        s32 n;
        s32 tile;
        u16 *p;
        s32 i;

        data_0209d6f0 = (u8 *)ep;
        base = data_0209d6fc + 0x28 + data_0209d70c[1];
        data_0209d6f4 = base + ep->textOffset;
        Message::LayoutLine();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)G2::GetBG3ScrPtr() + ((u16)(var_r4 + 0x40) + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            tile = var_r5;
            do {
                p[i] = (u16)tile;
                (p + i)[0x20] = (u16)(n + tile);
                tile++;
                i++;
            } while (i < n);
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
    p = G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);
    s = G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);
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
    p = G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);
    s = G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);
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
    p = G2S::GetBG0CharPtr() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);
    s = G2S::GetBG0ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);
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
    u16* q;
    int cnt, div;
    int base;

    data_0209d6d4 = (short)t;
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

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);

    s = (int)G2::GetBG3ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);

    if (t != 0x1b3) {
        func_0201b6f8(0);
        data_0209d6a8 = (u8)(((int)data_0209d6b0 + 7) / 8);
        data_0209d65c = 0;
        div = 0x107;
    } else {
        data_0209d6f4 += 3;
        func_0201b6f8(0);
        data_0209d65c = (u8)((0xb0 - (int)data_0209d6b0) >> 1);
        data_0209d6a8 = 0x16;
        data_0209d678 = data_0209d65c & 7;
        div = 0x105;
    }

    q = (u16 *)((char *)G2::GetBG3ScrPtr() + div * 2);
    cnt = data_0209d6a8;
    for (i = 0; i < cnt; i++) {
        u16* q2 = q + 0x20;
        q[i] = (u16)(i + 0x200);
        q2[i] = (u16)(i + 0x200 + cnt);
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
    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);
    s = (int)G2::GetBG3ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);
    DisplayText(msg);
    data_0209d6d4 = 0x276;
    gMsgBoxStyle = 0;
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    p = G2S::GetBG0CharPtr() + 0x4000;
    li2 = 0;
    MultiStore_Int(li2, (int*)p, 0x2000);
    s = G2S::GetBG0ScrPtr();
    ls2 = 0x2ff;
    MultiStore16(ls2, (char*)s, 0x800);
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

    data_0209d6d4 = (short)(msg + (SublevelToLevel(data_02092110) * 7 + 0x1b4));
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

    p = func_02054d88() + 0x5000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x800);

    q = (u16 *)((char *)G2::GetBG3ScrPtr() + 0x180);
    for (i = 0; i < 0x40; i++) {
        q[i] = (u16)(i + 0x280);
    }

    LayoutLine();
    func_0201b388(data_0209d6a8);
}

void Message::DisplayCourseNameForStarSelect(u32 base)
{
    volatile unsigned short ls;
    volatile int li1;
    volatile int li2;
    int idx;
    int sum;
    u16* q;
    int div;
    int div2;

    data_0209d6d4 = (short)(base + 0x196);
    data_0209d660 = 0;
    func_0201eaac();

    idx = data_0209d6d4;
    data_0209d6f0 = (u8*)((char*)data_0209d708 + idx * 8);
    sum = data_0209d6fc + 0x28 + data_0209d70c[1] +
          ((MessageEntry *)((char *)data_0209d708 + idx * 8))->textOffset;
    data_0209d6f4 = sum;
    data_0209d6f4 = sum + 3;
    func_0201b6f8(0);

    data_0209d65c = (u8)((0x100 - data_0209d6b0) / 2);
    data_0209d6a8 = 0x20;

    {
        int p1 = func_02054d88() + 0x4000;
        li1 = 0;
        MultiStore_Int(li1, (int*)p1, 0x2000);
    }

    {
        int s1 = (int)G2::GetBG3ScrPtr();
        ls = 0x2ff;
        MultiStore16(ls, (char*)s1, 0x800);
    }

    if (data_0209d6d4 >= 0x1a5) {
        gMsgBoxStyle = 0;
        q = (u16 *)((char *)G2::GetBG3ScrPtr() + 0x400);
    } else {
        gMsgBoxStyle = 1;
        q = (u16 *)((char *)G2::GetBG3ScrPtr() + 0x4c0);
    }
    div = 0;
    do {
        q[div] = (u16)(div + 0x200);
        div++;
    } while (div < 0x40);

    SetSubBg1Offset(0, 0);
    LayoutLine();
    func_0201b388(data_0209d6a8);

    if (data_0209d6d4 >= 0x1a5) {
        return;
    }

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

    data_0209d65c = (u8)((0xd2 - data_0209d6b0) / 2);
    data_0209d6a8 = 0x20;

    {
        int p2 = func_02054d88() + 0x4800;
        li2 = 0;
        MultiStore_Int(li2, (int*)p2, 0x800);
    }

    q = (u16 *)((char *)G2::GetBG3ScrPtr() + 0x240);
    div2 = 0;
    do {
        q[div2] = (u16)(div2 + 0x240);
        div2++;
    } while (div2 < 0x40);

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
    volatile int li;
    volatile unsigned short ls;
    int p;
    int s;
    u8* scr;
    int i;
    int cols;
    int rem;
    int idx;
    int one;
    int v7f;
    int v87;
    int v43;
    unsigned w;
    MessageEntry *base;
    MessageEntry *entry;

    data_0209d6d4 = (s16)n;
    func_0201eaac();

    one = 1;
    v7f = 0x7f;
    v87 = 0x87;
    data_0209d67c = 0x3c;
    data_0209d660 = (u8)one;
    data_0209d654 = (u8)one;
    gMsgBoxWinLeft = (u8)v7f;
    gMsgBoxWinTop = (u8)v87;
    gMsgBoxStyle = 0;
    func_02012790(4);

    idx = data_0209d6d4;
    base = data_0209d708;
    entry = (MessageEntry *)((char *)base + idx * 8);
    data_0209d6f0 = (u8*)entry;
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] +
                    ((MessageEntry *)((char *)base + idx * 8))->textOffset;

    w = entry->width;
    w = w >> 3;
    data_0209d6a8 = (u8)((int)(w * 9 + 7) / 8);
    gMsgBoxLeft = (u8)(((0x20 - ((int)data_0209d6a8 + 2)) / 2) << 3);
    gMsgBoxCenterX = (u8)v7f;
    v43 = 0x43;
    gMsgBoxCenterY = (u8)v43;
    gMsgBoxTop = (u8)(0x44 - (int)(((((int)entry->lineCount << 1) + 1) << 3) / 2));
    gMsgBoxWinLeft = (u8)v7f;
    gMsgBoxWinTop = (u8)v43;
    data_0209d680 = 0;
    data_0209d65c = 0;

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);

    s = (int)G2::GetBG3ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);

    scr = (u8 *)((char *)G2::GetBG3ScrPtr() + 0x42);
    SetBg3Offset(-(int)gMsgBoxLeft, 4 - (int)gMsgBoxTop);

    cols = data_0209d6a8;
    i = 0;
    if ((cols * (int)((MessageEntry *)data_0209d6f0)->lineCount) * 2 > 0) {
        int last = cols - 1;
        do {
            rem = i % cols;
            *(u16*)(scr + (rem << 1)) = (u16)(i + 0x200);
            if (last == rem)
                scr += 0x40;
            i++;
        } while (i < (cols * (int)((MessageEntry *)data_0209d6f0)->lineCount) * 2);
    }

    while (data_0209d6bc < 4)
        Message::Update();

    {
        int b = gMainScreenLayers | 0x20;
        gNextActorUpdateMask |= 0x80000000u;
        G2x::SetBlendBrightness((volatile u16 *)0x4000050, b, -8);
    }

    {
        volatile u16 *win0h = (volatile u16 *)0x4000048;
        volatile u16 *win0v = (volatile u16 *)0x400004a;
        u8 d = gMainScreenLayers;
        *win0h = (u16)((*win0h & ~0x3f) | (d | 8) | 0x20);
        *win0v = (u16)((*win0v & ~0x3f) | 0x17);
    }

    *(volatile u16 *)0x4000040 = (u16)(((gMsgBoxWinLeft << 8) & 0xff00) |
                                       ((gMsgBoxCenterX * 2 - gMsgBoxWinLeft + 1) & 0xff));
    *(volatile u16 *)0x4000044 =
        (u16)(((gMsgBoxWinTop << 8) & 0xff00) | ((gMsgBoxCenterY * 2 - gMsgBoxWinTop + 1) & 0xff));

    {
        volatile unsigned int *disp = (volatile unsigned int *)0x4000000;
        *disp = (*disp & ~0xe000u) | 0x2000u;
        gMainScreenLayers |= DISPLAY_LAYER_BG3;
        *disp = (*disp & ~0x1f00u) | ((unsigned int)gMainScreenLayers << 8);
    }
}

extern "C" void func_0201ef38(void) {
    gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
}

extern "C" void func_0201ef50(int arg)
{
    volatile int li;
    volatile int li2;
    int b;
    int new_var;
    int t;
    int idx;
    int sl;
    int cp;
    func_0201eaac();
    data_0209d660 = 0;
    gMsgBoxStyle = 0;
    data_0209d690 = 1;
    cp = G2S::GetBG0CharPtr();
    li = 0;
    MultiStore_Int(li, (int *) cp, 0x8000);
    if (arg < 0)
    {
        return;
    }
    data_0209d6d4 = (s16) (arg + 0x24e);
    idx = data_0209d6d4;
    data_0209d6f0 = (u8 *) (((char *) data_0209d708) + (idx * 8));
    data_0209d6f4 = ((data_0209d6fc + 0x28) + data_0209d70c[1]) +
                    ((MessageEntry *)(((char *)data_0209d708) + (idx * 8)))->textOffset;
    data_0209d6a8 = 0x20;
    data_0209d680 = 0;
    data_0209d65c = 0;
    b = ((MessageEntry *)(((char *)data_0209d708) + (idx * 8)))->lineCount;
    t = 0xc - b;
    sl = (u16) (t << 10);
    data_0209d6e0 = (s16) (t << 5);
    while (data_0209d6bc != 8)
    {
        func_0201b6f8(0);
        data_0209d65c = (0x100 - ((int) data_0209d6b0)) >> 1;
        cp = G2S::GetBG0CharPtr();
        new_var = cp;
        new_var = new_var + sl;
        li2 = 0;
        MultiStore_Int(li2, (int *) new_var, 0x800);
        Message::LayoutLine();
        func_0201b388(0x20);
        sl = (u16) (sl + 0x800);
        data_0209d6e0 = data_0209d6e0 + 0x40;
    }
}

extern "C" void func_0201f108(void) {
    u16 *scr = (u16 *)G2S::GetBG0ScrPtr();
    int i = 0;
    do {
        scr[i] = (u16)i;
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

    data_0209d6a8 = (u8)((((((MessageEntry *)data_0209d6f0)->width / 8) * 9) + 7) / 8);
    data_0209d680 = 0;
    data_0209d65c = 0;

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, (int*)p, 0x2000);

    s = (int)G2::GetBG3ScrPtr();
    ls = 0x2ff;
    MultiStore16(ls, (char*)s, 0x800);

    q = (u16 *)((char *)G2::GetBG3ScrPtr() + 0x42);
    div = data_0209d6a8;
    cnt = div * ((MessageEntry *)data_0209d6f0)->lineCount * 2;
    for (i = 0; i < cnt; i++) {
        rem = i % div;
        q[rem] = (u16)(i + 0x200);
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

extern "C" void func_0201f32c(int arg0)
{
    MessageEntry *entry;
    u8 disp;
    u8 f2d8_val;
    u8 disp2;
    u8 t;
    void *var_r8;
    s32 var_r7;
    s32 last_rem;
    u8 divisor;
    s32 i;
    volatile u16 sp0;
    volatile u16 sp2;
    volatile u16 sp4;
    volatile s32 sp8;
    volatile s32 spC;
    volatile s32 sp10;

    if (((MessageInfoSection *)data_0209d70c)->messageCount <= (u16)arg0) {
        return;
    }

    data_0209d6d4 = arg0;
    func_0201eaac();

    data_0209d660 = 1;
    gMsgBoxWinLeft = 0x7f;
    gMsgBoxWinTop = 0x87;
    if (data_0209d6d4 >= 0x2a && data_0209d6d4 < 0x83) {
        gMsgBoxStyle = 1;
        func_02012790(0x20);
    } else {
        u32 b0;
        gMsgBoxStyle = 0;
        b0 = (data_0209f2d8 == 2);
        if (b0 == 0) {
            func_02012790(4);
        }
    }

    entry = &data_0209d708[data_0209d6d4];
    data_0209d6f0 = (u8 *)entry;
    {
        s32 ev = entry->textOffset;
        s32 sum = data_0209d6fc + 0x28 + data_0209d70c[1];
        data_0209d6f4 = sum + ev;
    }
    data_0209d6a8 = (u8)((entry->width + 7) / 8);

    f2d8_val = data_0209f2d8;
    {
        u32 b1 = (f2d8_val == 0);
        if (b1 != 0 && !(data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE)) {
            u32 b2 = (gRunningKuppaScript != 0);
            if (b2 == 0) {
                gMsgBoxLeft = 0x10;
                gMsgBoxTop = 0x64;
                gMsgBoxCenterX = (u8)((((data_0209d6a8 + 2) / 2) * 8) + 0xf);
                gMsgBoxCenterY = (u8)((((entry->lineCount * 2 + 1) * 8) / 2) + 0x63);
                gMsgBoxWinLeft = gMsgBoxCenterX;
                gMsgBoxWinTop = gMsgBoxCenterY;
                goto branchDone;
            }
        }
        {
            u32 b3 = (f2d8_val == 2);
            if (b3 != 0) {
                gMsgBoxLeft = (u8)(((0x20 - (data_0209d6a8 + 2)) / 2) * 8);
                gMsgBoxCenterX = 0x7f;
                gMsgBoxTop = 0xa0;
                gMsgBoxCenterY = (u8)((((entry->lineCount) * 16) / 2) + 0x9f);
                gMsgBoxWinLeft = 0x7f;
                gMsgBoxWinTop = gMsgBoxCenterY;
            } else {
                gMsgBoxLeft = (u8)(((0x20 - (data_0209d6a8 + 2)) / 2) * 8);
                gMsgBoxCenterX = 0x7f;
                gMsgBoxCenterY = 0x63;
                gMsgBoxTop = (u8)(0x64 - (((entry->lineCount * 2 + 2) * 8) / 2));
                gMsgBoxWinLeft = 0x7f;
                gMsgBoxWinTop = 0x63;
                if (func_0201fb4c() != 0) {
                    Sound::ChangeMusicVolume(0x40, 0xc999);
                }
            }
        }
    branchDone: ;
    }

    data_0209d680 = 0;
    data_0209d65c = 0;
    if (data_0209d698 == 2) {
        {
            void *pa = (char *)func_02054ea8() + 0x4000;
            sp8 = 0;
            MultiStore_Int(sp8, (int *)pa, 0x4000);
        }
        {
            void *pb = (char *)G2::GetBG1ScrPtr() + 0x200;
            sp0 = 0x3ff;
            MultiStore16(sp0, (char *)pb, 0x400);
        }
        var_r8 = (char *)G2::GetBG1ScrPtr() + 0x24e;
        SetBg1Offset(0, 0);
        var_r7 = 0x200;
        data_0209d660 = 0;
    } else if (data_0209d698 == 1) {
        {
            void *pa = (char *)G2S::GetBG1CharPtr() + 0x4000;
            spC = 0;
            MultiStore_Int(spC, (int *)pa, 0x2000);
        }
        {
            void *pb = (void *)G2S::GetBG1ScrPtr();
            sp2 = 0x22ff;
            MultiStore16(sp2, (char *)pb, 0x800);
        }
        var_r8 = (char *)G2S::GetBG1ScrPtr() + 0x42;
        SetSubBg1Offset(4 - gMsgBoxLeft, 4 - gMsgBoxTop);
        var_r7 = 0x2200;
    } else {
        {
            void *pa = (char *)func_02054d88() + 0x4000;
            sp10 = 0;
            MultiStore_Int(sp10, (int *)pa, 0x3000);
        }
        {
            void *pb = G2::GetBG3ScrPtr();
            sp4 = 0x37f;
            MultiStore16(sp4, (char *)pb, 0x800);
        }
        var_r8 = (char *)G2::GetBG3ScrPtr() + 0x42;
        {
            u32 b4 = (data_0209f2d8 == 2);
            if (b4 != 0) {
                SetBg3Offset(0 - gMsgBoxLeft, 8 - gMsgBoxTop);
            } else {
                SetBg3Offset(0 - gMsgBoxLeft, 4 - gMsgBoxTop);
            }
        }
        var_r7 = 0x200;
    }

    {
        divisor = data_0209d6a8;
        i = 0;
        if ((s32)(divisor * ((MessageEntry *)data_0209d6f0)->lineCount * 2) > 0) {
            do {
                s32 rem = i % divisor;
                *(u16 *)((char *)var_r8 + rem * 2) = (u16)var_r7;
                if (divisor - 1 == rem) {
                    var_r8 = (char *)var_r8 + 0x40;
                }
                i++;
                var_r7++;
            } while (i < (s32)(divisor * ((MessageEntry *)data_0209d6f0)->lineCount * 2));
        }
    }

    t = data_0209d6bc;
    if (t < 4) {
        do {
            Message::Update();
            t = data_0209d6bc;
        } while (t < 4);
    }
    data_0209d6ac = t;

    disp2 = data_0209d698;
    if (disp2 == 2) {
        gMainScreenLayers |= DISPLAY_LAYER_BG1;
        *(volatile s32 *)0x04000000 =
            (*(volatile s32 *)0x04000000 & ~0x1f00) | (gMainScreenLayers << 8);
        return;
    }
    if (disp2 == 1) {
        G2x::SetBlendBrightness((volatile u16 *)0x04001050, gSubScreenLayers | 0x20, -7);
        *(volatile u16 *)0x04001048 =
            (*(volatile u16 *)0x04001048 & ~0x3f) | (gSubScreenLayers | 2) | 0x20;
        *(volatile u16 *)0x0400104a = (*(volatile u16 *)0x0400104a & ~0x3f) | 0x1d;
        *(volatile s16 *)0x04001040 =
            ((gMsgBoxWinLeft << 8) & 0xff00) | (u8)(((gMsgBoxCenterX * 2) - gMsgBoxWinLeft) + 1);
        *(volatile u16 *)0x04001044 =
            ((gMsgBoxWinTop << 8) & 0xff00) | (u8)(((gMsgBoxCenterY * 2) - gMsgBoxWinTop) + 1);
        *(volatile s32 *)0x04001000 = (*(volatile s32 *)0x04001000 & ~0xe000) | 0x2000;
        gSubScreenLayers |= 2;
        *(volatile s32 *)0x04001000 =
            (*(volatile s32 *)0x04001000 & ~0x1f00) | (gSubScreenLayers << 8);
        return;
    }

    if (data_0209d6d4 >= 0x2a && data_0209d6d4 < 0x83) {
        G2x::SetBlendBrightness((volatile u16 *)0x04000050, gMainScreenLayers & ~0x38, 7);
    } else {
        G2x::SetBlendBrightness((volatile u16 *)0x04000050, gMainScreenLayers & ~0x38, -8);
    }
    *(volatile u16 *)0x04000048 =
        (*(volatile u16 *)0x04000048 & ~0x3f) | (gMainScreenLayers | 8) | 0x20;
    *(volatile u16 *)0x0400004a = (*(volatile u16 *)0x0400004a & ~0x3f) | 0x17;
    *(volatile s16 *)0x04000040 =
        ((gMsgBoxWinLeft << 8) & 0xff00) | (u8)(((gMsgBoxCenterX * 2) - gMsgBoxWinLeft) + 1);
    *(volatile s16 *)0x04000044 =
        ((gMsgBoxWinTop << 8) & 0xff00) | (u8)(((gMsgBoxCenterY * 2) - gMsgBoxWinTop) + 1);
    data_0209d6bc = 0;
    *(volatile s32 *)0x04000000 = (*(volatile s32 *)0x04000000 & ~0xe000) | 0x2000;
    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    *(volatile s32 *)0x04000000 =
        (*(volatile s32 *)0x04000000 & ~0x1f00) | (gMainScreenLayers << 8);
}

/* Course ids that must not open the talk window. data_0209d664 is the byte
   after the shared reset flag at data_0209d660. */
extern "C" int func_0201fb4c(void)
{
    short v;
    unsigned int b;
    if (data_0209d664 != 0) goto no;
    b = (data_0209f2d8 == 2);
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
    s16 id = (s16)ObjectMessageIDToActualMessageID(objectMsgID);
    func_0201f32c(id);
}

void Message::ResetAllGlobals()
{
    func_0201eaac();
    data_0209d660 = 0;
}
