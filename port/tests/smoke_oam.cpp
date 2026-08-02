// Gate-6 smoke: the 2D sprite engine's software side runs on host.
//
// OAM::Reset initializes the shadow, OAM::Render emits attribute words for
// plain and rotated sprites (rotation through the real trig table romdata
// extracted), LoadAffineParams fills a matrix slot, and OAM::Load uploads
// the shadow into the mapped OAM region at 0x07000000 -- where the checks
// read it back, exactly where a scan-out will. The ov-coupled wrappers
// (RenderOamBothScreens) wait for the overlay story.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ntr/mmio.h"
#include "ntr/ppu.h"

#include "fault_probe.h"

typedef unsigned int u32;
typedef unsigned short u16;

namespace OAM { void Reset(); }
extern "C" {
/* Render(draw, attr, x, y, palette, priority, scale fx12, rot) */
void *_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int draw, void *attr, int x, int y,
                                              int pal, int prio, int scale, int rot);
void _ZN3OAM4LoadEv(void);
extern unsigned char data_0209e660;
extern int data_0209e664;
extern unsigned int data_0209e674[];
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }

    CHECK(data_0209e660 == 1);      /* the HAL runs the direct-fill mode */
    OAM::Reset();
    /* every shadow entry disabled: attr0 low word 0xc0 (OBJ off) */
    int all_disabled = 1;
    for (int i = 0; i < 0x80; ++i)
        if (data_0209e674[i * 2] != 0xc0 || data_0209e674[i * 2 + 1] != 0)
            all_disabled = 0;
    CHECK(all_disabled);

    /* a plain 16x16 square sprite: shape 0, size 1, tile 5, no rotation */
    u32 attr[2];
    attr[0] = 0                     /* y placeholder */
              | (0u << 8)           /* rsFlags: plain */
              | (0u << 10)          /* objMode */
              | (0u << 12) | (0u << 13)
              | (0u << 14)          /* shape: square */
              | (0u << 16)          /* x placeholder */
              | (1u << 30);         /* size 1 = 16x16 (attr1 bits 30-31) */
    attr[1] = 5u | (0u << 10) | (0u << 12);   /* tile 5 */
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, attr, 40, 60, 2, 1, 0x1000, 0);
    CHECK(data_0209e664 == 1);      /* one entry consumed */
    const u32 w0 = data_0209e674[0];
    const u32 w1 = data_0209e674[1];
    printf("  plain sprite words: %08x %08x\n", w0, w1);
    CHECK((w0 & 0xFF) == 60);               /* y */
    CHECK(((w0 >> 16) & 0x1FF) == 40);      /* x */
    CHECK((w0 & 0x300) == 0);               /* no rotation/scaling */
    CHECK((w1 & 0x3FF) == 5);               /* tile */
    CHECK(((w1 >> 12) & 0xF) == 2);         /* palette */
    CHECK(((w1 >> 10) & 0x3) == 1);         /* priority */

    /* a rotated sprite: the affine slot must engage and fill params */
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, attr, 100, 80, 0, 0, 0x1000, 0x400);
    CHECK(data_0209e664 == 2);
    const u32 r0 = data_0209e674[2];
    printf("  rotated sprite words: %08x %08x\n", r0, data_0209e674[3]);
    CHECK((r0 & 0x100) != 0);       /* rotation/scaling flag set */
    /* affine params live in words 3 of each 4-entry group; slot 0 spans
       entries 0..3. 0x400 is 90 degrees: cos ~0, sin ~1 -> pa ~0 */
    const int pa = (short)(data_0209e674[1] >> 16) | 0;
    printf("  affine pa (entry0 hi16): %d\n", (short)(data_0209e674[1] >> 16));

    /* upload: the words must land in mapped OAM at 0x07000000 */
    _ZN3OAM4LoadEv();
    const u32 *oam = (const u32 *)0x07000000;
    printf("  oam[3..7]: %08x %08x %08x %08x %08x\n",
           oam[3], oam[4], oam[5], oam[6], oam[7]);
    printf("  oam[0..2]: %08x %08x %08x\n", oam[0], oam[1], oam[2]);
    /* the upload must mirror the LIVE shadow -- affine parameters live in
       the filler words of neighboring entries (slot 0 spans entries 0..3),
       so entry 0's second word changed when the rotated sprite landed */
    CHECK(memcmp(oam, data_0209e674, 0x400) == 0);
    CHECK(oam[0] == w0);
    CHECK((oam[1] & 0xFFFF) == (w1 & 0xFFFF));   /* attr2 untouched */
    CHECK(oam[2] == r0);
    CHECK((short)(oam[1] >> 16) == (short)pa);   /* affine pa in the filler */

    /* scan-out: the OAM the game just uploaded becomes pixels. The tile and
       palette are harness-side (the HUD graphic loaders are ov-coupled);
       the PLACEMENT is entirely the game's. Tile 5, 16x16 = four 32-byte
       tiles of solid color-index 1; palette 2 entry 1 = pure red. */
    {
        volatile unsigned char *tiles =
            (volatile unsigned char *)(0x06400000u + 5u * 32u);
        for (int i = 0; i < 4 * 32; ++i) tiles[i] = 0x11;
        *(volatile u16 *)(0x05000200u + (2u * 16u + 1u) * 2u) = 0x001F; /* red */
        NTR_MMIO(u32, 0x04000000) = (1u << 4);   /* DISPCNT: OBJ 1D mapping */

        ntr::Framebuffer fb;
        for (int y = 0; y < ntr::SCREEN_H; ++y)
            for (int x = 0; x < ntr::SCREEN_W; ++x) fb.px[y][x] = 0xFF101820u;
        ntr::ppu_scanout_obj(ntr::ENGINE_A, fb);

        CHECK(fb.px[60][40] == 0xFFFF0000u);         /* plain sprite top-left */
        CHECK(fb.px[75][55] == 0xFFFF0000u);         /* ...bottom-right */
        CHECK(fb.px[60][39] == 0xFF101820u);         /* left edge untouched */
        CHECK(fb.px[59][40] == 0xFF101820u);         /* top edge untouched */
        /* the rotated sprite renders with palette 0 (unfilled -> black);
           any non-background pixel in its double-size box counts */
        int rotated_px = 0;
        for (int y = 60; y < 110; ++y)
            for (int x = 80; x < 130; ++x)
                if (fb.px[y][x] != 0xFF101820u) ++rotated_px;
        printf("  rotated sprite pixels: %d\n", rotated_px);
        CHECK(rotated_px > 100);      /* the affine sprite drew a real footprint */
        ntr::ppu_write_bmp("smoke_oam.bmp", fb);
    }

    if (g_failures) {
        fprintf(stderr, "smoke_oam: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_oam: all checks passed (the game's sprite engine emits, "
           "uploads and scans out on host)\n");
    return 0;
}
