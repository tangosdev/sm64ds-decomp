/* The scene's hand-drawn gap band. See port/hal/gap_art.h for the file
   contract; this is the reader for it. */

#include "hal/gap_art.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

/* THE ROM'S OWN "WHICH MINIGAME IS RUNNING". dScMgBase_c's base constructor
   (src/func_ov004_020b2adc.c) ends with `data_ov004_020beb68 = self`, and ov004
   reads the live scene back out of it everywhere -- the score getters
   func_ov004_020adbc0 and func_ov004_020adc1c, the minigame index
   func_ov004_020adc3c, hal/scene_mg.cpp's probe. It is the word immediately
   BEFORE the gap's own G at 0x020beb6c, which is not a coincidence: both are
   the framework's per-minigame state.

   AS BYTES, for the same reason hal/screen_gap.cpp assembles G out of bytes:
   the ov004 mount defines this as `u8 [4]` of real packed overlay storage with
   a PACK BROKEN check on its offset, and a C++ `extern void *` of the same name
   would be a second opinion about that storage rather than a reader of it. The
   host build is 32-bit (port/build-port.cmd calls vcvars32), so the pointer the
   ROM body stored is exactly these four bytes little endian. */
extern "C" unsigned char data_ov004_020beb68[4];

namespace {

/* the decoded band, and the pair it was decoded for */
uint32_t *g_art;
int g_art_scene = -1;
int g_art_gap = -1;

const uint32_t kKey = 0xFFFF00FFu;      /* magenta #FF00FF, the exact value */

/* ---- where the file is ----------------------------------------------------
   The same three candidates hal/host_settings.cpp tries for settings.json, in
   the same order: beside the exe, under SM64DS_ASSET_ROOT, then the working
   directory. In a player's kit all three are the same folder -- the launcher
   puts the exe, settings.json and the assets in one bundle and launches with
   its working directory and SM64DS_ASSET_ROOT both set to it -- so this is one
   folder in practice and three only for the ways a developer runs it.

   The first candidate that OPENS wins, rather than the first that exists as a
   directory, because "the art is in the other folder" and "the art is not there
   at all" want the same answer: draw the band without it. */
FILE *open_art(int scene_id, char *shown, size_t cap)
{
    char path[1024];
    std::snprintf(shown, cap, "gap/scene_%d.bmp", scene_id);
#ifdef _WIN32
    {
        char exe[MAX_PATH];
        DWORD n = GetModuleFileNameA(NULL, exe, MAX_PATH);
        if (n > 0 && n < MAX_PATH) {
            char *slash = std::strrchr(exe, '\\');
            char *fwd = std::strrchr(exe, '/');
            if (fwd && (!slash || fwd > slash)) slash = fwd;
            if (slash) {
                *slash = '\0';
                std::snprintf(path, sizeof path, "%s\\gap\\scene_%d.bmp", exe,
                              scene_id);
                FILE *f = std::fopen(path, "rb");
                if (f) {
                    std::snprintf(shown, cap, "%s", path);
                    return f;
                }
            }
        }
    }
#endif
    {
        const char *root = std::getenv("SM64DS_ASSET_ROOT");
        if (root && *root) {
            std::snprintf(path, sizeof path, "%s/gap/scene_%d.bmp", root,
                          scene_id);
            FILE *f = std::fopen(path, "rb");
            if (f) {
                std::snprintf(shown, cap, "%s", path);
                return f;
            }
        }
    }
    {
        std::snprintf(path, sizeof path, "gap/scene_%d.bmp", scene_id);
        FILE *f = std::fopen(path, "rb");
        if (f) {
            std::snprintf(shown, cap, "%s", path);
            return f;
        }
    }
    return 0;
}

unsigned rd_u16(const unsigned char *p) { return p[0] | ((unsigned)p[1] << 8); }
unsigned rd_u32(const unsigned char *p)
{
    return p[0] | ((unsigned)p[1] << 8) | ((unsigned)p[2] << 16) |
           ((unsigned)p[3] << 24);
}

/* what a refusal costs the picture, in the asker's own terms */
const char *instead(int for_custom)
{
    return for_custom ? "the band falls back to the ambient fill"
                      : "peek's backdrop stays black";
}

/* ---- the reader -----------------------------------------------------------
   STRICT ON PURPOSE. The one converter that writes these files is ours (the
   band is cut out of Tango's template and written straight), so anything this
   refuses is a file that did not come through it, and a reader that guessed
   what such a file meant would put a wrong picture on the screen and call it
   the art. Every refusal names the number that failed. */
uint32_t *load_bmp(FILE *f, const char *shown, int gap_ds, int scene_id,
                   int for_custom)
{
    unsigned char hdr[54];
    if (std::fread(hdr, 1, sizeof hdr, f) != sizeof hdr) {
        std::fprintf(stderr, "[gapart] scene %d: %s is too short to be a BMP; "
                     "%s\n", scene_id, shown, instead(for_custom));
        return 0;
    }
    if (hdr[0] != 'B' || hdr[1] != 'M') {
        std::fprintf(stderr, "[gapart] scene %d: %s does not start with BM; "
                     "%s\n", scene_id, shown, instead(for_custom));
        return 0;
    }
    const unsigned off = rd_u32(hdr + 10);
    const unsigned dib = rd_u32(hdr + 14);
    const int w = (int)rd_u32(hdr + 18);
    const int raw_h = (int)rd_u32(hdr + 22);
    const unsigned bpp = rd_u16(hdr + 28);
    const unsigned comp = rd_u32(hdr + 30);
    /* a NEGATIVE height is a top-down BMP. The contract asks for bottom-up,
       which is what every writer in this tree emits, but a top-down file is
       unambiguous rather than doubtful -- the sign IS the answer -- so it is
       read rather than refused, and the log says which way round it was. */
    const int top_down = raw_h < 0;
    const int h = top_down ? -raw_h : raw_h;

    if (dib < 40) {
        std::fprintf(stderr, "[gapart] scene %d: %s has a %u-byte DIB header, "
                     "wants at least 40 (BITMAPINFOHEADER); %s\n", scene_id,
                     shown, dib, instead(for_custom));
        return 0;
    }
    if ((bpp != 24 && bpp != 32) || comp != 0) {
        std::fprintf(stderr, "[gapart] scene %d: %s is %u-bit compression %u, "
                     "wants 24-bit or 32-bit BI_RGB; %s\n", scene_id, shown,
                     bpp, comp, instead(for_custom));
        return 0;
    }
    /* THE TWO SIZES, and nothing between them. 256 x G is DS pixels; 512 x 2G
       is the 2x sheet. A file that is neither is the wrong crop -- the template
       with its context rows still attached is 256 x (G + 128) and lands here --
       and the message says both shapes so the fix is readable off the line. */
    int step = 0;
    if (w == 256 && h == gap_ds) step = 1;
    else if (w == 512 && h == 2 * gap_ds) step = 2;
    if (!step) {
        std::fprintf(stderr, "[gapart] scene %d: %s is %dx%d, wants 256x%d or "
                     "512x%d for G=%d; %s\n", scene_id, shown, w, h, gap_ds,
                     2 * gap_ds, gap_ds, instead(for_custom));
        return 0;
    }

    const unsigned bytespp = bpp / 8;
    const size_t stride = (((size_t)w * bytespp) + 3u) & ~(size_t)3;
    unsigned char *rows = (unsigned char *)std::malloc(stride * (size_t)h);
    uint32_t *out =
        (uint32_t *)std::malloc(sizeof(uint32_t) * 256u * (size_t)gap_ds);
    if (!rows || !out) {
        std::free(rows);
        std::free(out);
        std::fprintf(stderr, "[gapart] scene %d: out of memory reading %s; "
                     "%s\n", scene_id, shown, instead(for_custom));
        return 0;
    }
    if (std::fseek(f, (long)off, SEEK_SET) != 0 ||
        std::fread(rows, 1, stride * (size_t)h, f) != stride * (size_t)h) {
        std::free(rows);
        std::free(out);
        std::fprintf(stderr, "[gapart] scene %d: %s is truncated (wanted %u "
                     "bytes of pixels at offset %u); %s\n", scene_id, shown,
                     (unsigned)(stride * (size_t)h), off, instead(for_custom));
        return 0;
    }

    int keyed = 0;
    for (int k = 0; k < gap_ds; ++k) {
        const int sy = k * step;
        const unsigned char *row = rows + stride * (size_t)(top_down
                                                                ? sy
                                                                : h - 1 - sy);
        for (int x = 0; x < 256; ++x) {
            const unsigned char *p = row + (size_t)(x * step) * bytespp;
            const uint32_t c = 0xFF000000u | ((uint32_t)p[2] << 16) |
                               ((uint32_t)p[1] << 8) | (uint32_t)p[0];
            if (c == kKey) {
                out[(size_t)k * 256 + x] = 0;      /* alpha 0: the key */
                ++keyed;
            } else {
                out[(size_t)k * 256 + x] = c;
            }
        }
    }
    std::free(rows);
    std::fprintf(stderr, "[gapart] scene %d: %s, %dx%d %u-bit %s -> 256x%d "
                 "band pixels, %d keyed (%.1f%%)\n", scene_id, shown, w, h,
                 bpp, top_down ? "top-down" : "bottom-up", gap_ds, keyed,
                 100.0 * keyed / (256.0 * gap_ds));
    return out;
}

}  // namespace

unsigned char *hal_gap_scene(void)
{
    unsigned p = (unsigned)data_ov004_020beb68[0] |
                 ((unsigned)data_ov004_020beb68[1] << 8) |
                 ((unsigned)data_ov004_020beb68[2] << 16) |
                 ((unsigned)data_ov004_020beb68[3] << 24);
    return (unsigned char *)(size_t)p;
}

int hal_gap_scene_id(void)
{
    unsigned char *s = hal_gap_scene();
    if (!s) return -1;
    /* ActorBase's actorID, +0xc, u16 (include/ActorBase.h). The ROM reads it
       there itself: the dScMgBase_c constructor func_ov004_020b2adc compares
       *(u16 *)(self + 0xc) against 0x16e, 0x185, 0x16d and 0x182 to pick a
       timer, and those four are minigame scene ids.

       NOTHING CLEARS THE POINTER WHEN A MINIGAME ENDS, so this can be read
       through a `this` the heap has taken back, and what comes out is then a
       number rather than an id. The range test is the whole guard: the ROM's
       own spawn table is 512 entries and hal/scene_boot.cpp refuses anything
       outside it, so a value past that is not a scene and is reported as no
       scene. Nothing downstream does more with it than fail to find a table
       row and fail to open a file. */
    const int id = (int)(*(unsigned short *)(s + 0xc));
    return id >= 0 && id < 512 ? id : -1;
}

const uint32_t *hal_gap_art(int scene_id, int gap_ds, int for_custom)
{
    if (scene_id < 0 || gap_ds <= 0) return 0;
    /* THE LATCH IS THE CACHE. One (scene, G) pair is loaded once and held; the
       compose asks for the same pair every frame and gets the same pointer back
       without touching the disk, and a scene change re-reads because the pair
       changed. A file that failed to load is remembered as a failure the same
       way -- g_art null with the pair set -- so a missing file is one line per
       scene rather than one line per frame. */
    if (scene_id == g_art_scene && gap_ds == g_art_gap) return g_art;
    std::free(g_art);
    g_art = 0;
    g_art_scene = scene_id;
    g_art_gap = gap_ds;

    char shown[1024];
    FILE *f = open_art(scene_id, shown, sizeof shown);
    if (!f) {
        std::fprintf(stderr, "[gapart] scene %d: no %s beside the exe, under "
                     "SM64DS_ASSET_ROOT or in the working directory; %s\n",
                     scene_id, shown, instead(for_custom));
        return 0;
    }
    g_art = load_bmp(f, shown, gap_ds, scene_id, for_custom);
    std::fclose(f);
    return g_art;
}
