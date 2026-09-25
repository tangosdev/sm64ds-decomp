//cpp
/**
 * Course-entry / results scene.
 *
 * ov075 hosts ENTRY(6) and RESULT(7); both spawn dScEntry_c. Nested
 * icon_c (nine dThIcon_c leaves) and graphCallback_c (dGraph_c::callback_c
 * leaf). GraphCallback2 writes sub BG2 priority, decompresses the screen,
 * then steps every live icon through Render. icon_c::Behavior bobs the
 * selected character; icon_c::Render draws course, arrow and character
 * OAM.
 *
 * dScEntry_c_classInit_ENTRY / RESULT are reconstructed (RTTI dScEntry_c,
 * ENTRY/RESULT registry). They sit past the hole and are not in this TU.
 * Retail does not store those spellings.
 *
 * deslop
 * Leftover: helpers stay func_ov075_*. This TU's GraphCallback2 calls
 *   021160dc; 02115e8c / 0211601c / 02116028 / 02116030 are the icon
 *   and graphCallback writers. Naming belongs with recovered members,
 *   not coined here.
 * Leftover: func_ov001_020ab5b0 is still the linker name of the ov001
 *   dThIcon field writer (kind, x, y, w, h). This TU's 02115e8c calls
 *   it. Naming belongs in ov001.
 * Leftover: func_0200f0bc is still the linker name of the arm9 language
 *   mapper (GetOwnerLanguage, then 2..5 -> 1..4). This TU's 02115e8c
 *   indexes the width/height pairs with it.
 * Leftover: func_0203d974 is still the arm9 vs/region gate
 *   (0x27ffc40 == 2, else data_020a0f10). This TU's icon_c::Render
 *   case 13. Naming belongs in arm9.
 * Leftover: ApproachLinear2 stays mangled. No header. This TU's
 *   icon_c::Behavior.
 * Leftover: G2S::GetBG2ScrPtr stays mangled. No header. GraphCallback2
 *   and icon_c::Render.
 * Leftover: OAM::Render Fix12-by-value stays mangled (OAM.h, wall 6az).
 *   This TU's icon_c::Render.
 * Leftover: OAM::RenderSub stays mangled. OAM.h has the method; this
 *   TU's icon_c::Render still uses the linker name.
 * Leftover: data_ov075_0211cb64 / 0211c720 / 0211c730 / 0211c954 are
 *   OAM. 0211d72c / 0211d72e / 0211d740 / 0211d742 / 0211d754 /
 *   0211d756 / 0211d948 / 0211d94a are language- or type-indexed
 *   width/height pairs (4-byte stride). Overlay .data/.bss this TU
 *   does not own.
 * Leftover: data_0209b2e4 is still the linker name of the selected
 *   character. This TU's icon_c::Behavior. Naming belongs in arm9.
 * Leftover: g_profile_ENTRY / g_profile_RESULT live outside this TU
 *   (S14). leftover return new belongs with the factories past the
 *   hole.
 * Leftover: dScEntry_c D1/D0 stay in their own shards (out-of-line
 *   emits D0 before D1; inline needs the factories this prefix cannot
 *   reach).
 */

#include "dScEntry_c.h"
#include "OAM.h"

extern "C" {
unsigned int _ZN3G2S12GetBG2ScrPtrEv();
void DecompressLZ16(void *, void *);
void Deallocate(void *);

int _Z15ApproachLinear2Rsss(short *, short, short);
extern unsigned char data_0209b2e4;

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int a, void *oam, int b, int c, int d, int e, int f, int g, int h, int i);
extern int func_0203d974(void);

int func_0200f0bc(void);
void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);

extern int data_ov075_0211cb64[];
extern char data_ov075_0211c720[];
extern char data_ov075_0211c730[];
extern void *data_ov075_0211c954[];
extern short data_ov075_0211d72c[];
extern short data_ov075_0211d72e[];
extern short data_ov075_0211d740[];
extern short data_ov075_0211d742[];
extern short data_ov075_0211d754[];
extern short data_ov075_0211d756[];
extern short data_ov075_0211d948[];
extern short data_ov075_0211d94a[];
}

#define SH(base,i) (*(short*)((char*)(base) + (i)*4))

/* Local overlay of icon_c Render (slot 1). 0x24 stride. */
struct Base {
    virtual void method0();
    virtual void method1();
};

// @symbol func_ov075_021160dc
extern "C" void func_ov075_021160dc(char* c) {
    int count = *(unsigned char*)(c + 0x280);
    int i = 0;
    if (count <= 0) return;
    Base *elem = (Base*)(c + 0x70);
    do {
        elem->method1();
        count = *(unsigned char*)(c + 0x280);
        i++;
        elem = (Base*)((char*)elem + 0x24);
    } while (i < count);
}

// @symbol _ZN10dScEntry_c15graphCallback_cC1Ev
dScEntry_c::graphCallback_c::graphCallback_c()
    : compressedBg2Screen(0), entryScene(0)
{
}

// @symbol _ZN10dScEntry_c15graphCallback_c14GraphCallback2Ev
int dScEntry_c::graphCallback_c::GraphCallback2()
{
    volatile unsigned short *reg = (volatile unsigned short *)0x400100c;
    unsigned short value = (unsigned short)(*reg & ~0x1f00);
    *reg = value | (bg2Priority << 8);

    if (compressedBg2Screen != 0) {
        unsigned short *screen = (unsigned short *)_ZN3G2S12GetBG2ScrPtrEv();
        DecompressLZ16(compressedBg2Screen, screen);
        Deallocate(compressedBg2Screen);
        compressedBg2Screen = 0;
    }

    if (entryScene != 0)
        func_ov075_021160dc((char *)entryScene);

    return 1;
}

extern "C" {

// @symbol func_ov075_02116030
void func_ov075_02116030(void *c, int v)
{
    if (*(int *)((char *)c + 4) == 0) *(int *)((char *)c + 4) = v;
}

// @symbol func_ov075_02116028
void func_ov075_02116028(int *p, int v)
{
    p[3] = v;
}

// @symbol func_ov075_0211601c
void func_ov075_0211601c(char* c, int a, int b) {
  *(int*)(c+8) = a;
  *(int*)(c+0xc) = b;
}

// @symbol func_ov075_02115e8c
void func_ov075_02115e8c(char* self, int a, int b, short c, short e)
{
    int kind;
    int i, j;
    *(int*)(self + 0x1c) = a;
    *(int*)(self + 0x20) = b;
    kind = (*(int*)(self + 0x1c) < 7) ? 2 : (*(int*)(self + 0x1c) < 0xd) ? 1 : 0;
    if (a == 5) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d72c, i), SH(data_ov075_0211d72e, j));
    } else if (a == 6) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d740, i), SH(data_ov075_0211d742, j));
    } else if (a == 0xb) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d754, i), SH(data_ov075_0211d756, j));
    } else {
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d948, a), SH(data_ov075_0211d94a, a));
    }
}

}

// @symbol _ZN10dScEntry_c6icon_c8BehaviorEv
void dScEntry_c::icon_c::Behavior()
{
    if (unk_01c == 0xd) {
        if (unk_020 == data_0209b2e4) {
            _Z15ApproachLinear2Rsss(&unk_006, 0x14, 8);
        } else {
            if (_Z15ApproachLinear2Rsss(&unk_006, -0x24, 8) != 0)
                unk_020 = data_0209b2e4;
        }
    }
    dThIcon_c::Behavior();
}

// @symbol _ZN10dScEntry_c6icon_c6RenderEv
void dScEntry_c::icon_c::Render()
{
    volatile s16 xy[2];
    u16 *p;
    int mask;
    int row;
    int col;
    int w;
    int h;
    void *oam;
    int y;
    int x;

    xy[0] = unk_004;
    xy[1] = unk_006;

    switch (unk_01c) {
    case 7:
    {
        int idx;
        void *ptr;
        int v;

        idx = unk_020;
        if (unk_010 == 0) {
            if (unk_011 != 0)
                idx += 4;
        }
        ptr = (void *)data_ov075_0211cb64[idx];

        v = unk_00c;
        if (v <= 0) {
            v = 0x1000;
        } else if (v < 4) {
            v = (v << 8) + 0x1000;
        } else {
            v = ((8 - v) << 8) + 0x1000;
        }

        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, ptr, xy[0], xy[1], -1, -1, v, v, 0, -1);
        return;
    }

    case 0:
    case 1:
    case 2:
        mask = (unk_010 != 0) ? 0x6000 : 0x5000;
        goto fill;

    case 8:
    case 9:
    case 10:
    case 11:
        mask = (unk_010 != 0) ? 0x2000 : 0x1000;
    fill:
        w = unk_008;
        p = (u16 *)_ZN3G2S12GetBG2ScrPtrEv();
        x = unk_004;
        h = unk_00a;
        x -= w;
        x >>= 3;
        p += x;
        y = unk_006;
        y -= h;
        p += (y >> 3) << 5;
        for (row = 0; row < unk_00a >> 2; row++) {
            for (col = 0; col < w >> 2; col++) {
                p[col] = (u16)(mask | (p[col] & 0xfff));
                w = unk_008;
            }
            p += 0x20;
        }
        return;

    case 3:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] - 2);
        oam = data_ov075_0211c720;
        goto renderSub;

    case 4:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] + 2);
        oam = data_ov075_0211c730;
        goto renderSub;

    case 13:
        if (func_0203d974() != 0 && unk_020 >= 4)
            return;
        oam = data_ov075_0211c954[unk_020];
        goto renderSub;

    case 5:
    case 6:
    case 12:
    default:
        return;
    }

renderSub:
    OAM::RenderSub((OamAttr *)oam, xy[0], xy[1]);
}

// @symbol _ZN12OamAnimationD1Ev
OamAnimation::~OamAnimation()
{
}

// @symbol _ZN10dScEntry_c6icon_cD1Ev
dScEntry_c::icon_c::~icon_c()
{
}
