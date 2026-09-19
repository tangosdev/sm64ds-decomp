//cpp
/* dScEntry_c -- the "entry" scene (course / minigame currently loaded).
 *
 * This file licenses the ROM-ascending prefix 0x02115ab8..0x02116128 of the
 * ov075 translation unit the linker shows as OamAnimation+dScEntry_c
 * (0x02115ab8..0x0211a854, 82 functions -- build/tu_map.json,
 * tools/tu_map.py). The rest of that run stays in its own enrolled shards;
 * config/tu_manifest.d/ov075/dScEntry_c.json records why.
 *
 * mwccarm 2004/b56 emits one .text section per function in the REVERSE of
 * source order, so the definitions below run from the HIGHEST cartridge
 * address to the lowest.
 */
#include "dScEntry_c.h"

/* The one file-scope extern "C" region, above the first @symbol marker so that
   no banked fragment absorbs a mangled _Z spelling. A fragment runs from its
   marker to the next one, so a declaration block placed between two markers is
   scored as part of the member above it. These names are already the ROM's
   mangled spellings; without extern "C" a .cpp re-mangles them into phantoms
   no module defines. */
extern "C" {
unsigned int _ZN3G2S12GetBG2ScrPtrEv();
void DecompressLZ16(void *, void *);
void Deallocate(void *);

int _Z15ApproachLinear2Rsss(short *, short, short);
extern unsigned char data_0209b2e4;

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int a, void *oam, int b, int c, int d, int e, int f, int g, int h, int i);
extern void _ZN3OAM9RenderSubEP7OamAttrii(void *oam, int x, int y);
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

/* Steps every icon in the 9-entry array at 0x70 through its second virtual
   slot. Written against a minimal polymorphic view rather than icon_c so the
   0x24 stride and the slot index stay explicit. */
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
    _ZN3OAM9RenderSubEP7OamAttrii(oam, xy[0], xy[1]);
}

// @symbol _ZN12OamAnimationD1Ev
OamAnimation::~OamAnimation()
{
}

// @symbol _ZN10dScEntry_c6icon_cD1Ev
dScEntry_c::icon_c::~icon_c()
{
}

/* dScEntry_c's own destructor pair (D1 0x02115ab8, D0 0x02115b28) is NOT in this
   file and NOT in the claimed range. mwccarm orders a D0/D1 group by source form,
   and the only form that emits D1 first requires the TU to INSTANTIATE the class
   (decomp-cpp-class-form). dScEntry_c is instantiated only by its two factories at
   0x0211a740 and 0x0211a854, which sit above the unenrolled 0x02116128..0x021165b0
   hole and cannot be reached from this prefix. Both variants therefore keep their
   own enrolled shards; see config/tu_manifest.d/ov075/dScEntry_c.json. */
