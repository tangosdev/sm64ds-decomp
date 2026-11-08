// run after093, lane BROCOLOR1: a bro in his player's own colours. See bro_colors.h.
//
// THE WINDOW. Palette VRAM is 0x06880000 + base * 16 for the 4x4-compressed
// format every bro texture uses (port/ntr/gx.cpp bind_from_vram). The DS's
// texture palette slots end at 0x18000 and the port keeps its OBJ extended
// palettes at 0x20000; 0x30000..0x80000 is reserved address space nothing
// else writes, and gx.cpp reads a base in it with the rest of the window as
// its length. A copy is placed there once per (palette, source words, picks)
// and never rewritten with other words while the decoded-texture cache can
// still hold a decode of it: the cache key is the palette base, the texture
// words and the palette's first word, so two different copies at one base in
// one cache generation could be served each other's decode. Every copy gets
// its own base; the table is emptied when ntr::gx_texture_generation() moves
// (the cache was dropped), and when the window is full a further body keeps
// the cartridge's colours (logged once) until the next drop.
#include "bro_colors.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "vs_char_groups.h"
#include "vs_char_palette_gen.h"

namespace ntr { unsigned gx_texture_generation(); }

namespace port {
namespace bro {
namespace {

const unsigned kPltt = 0x06880000u;
const unsigned kWinBase = 0x30000u, kWinEnd = 0x80000u;
const int kRegions = 512;
const int kCatOfRow[kRows] = { vscg::CAT_HAT_UNDERSHIRT, vscg::CAT_OVERALLS, vscg::CAT_SHOES };

struct Region {
    unsigned src_off, size;     // the palette span copied (bytes from kPltt)
    unsigned key;               // hash of the source words and the picks that apply
    unsigned off;               // where the copy is (bytes from kPltt)
    unsigned short *words;      // what the copy holds
};
Region g_reg[kRegions];
int g_nreg;
unsigned g_bump = kWinBase;
unsigned g_gen;
bool g_gen_have;
int g_log = -1;

int log_level()
{
    if (g_log < 0) {
        const char *e = std::getenv("SM64DS_BRO_COLORS_LOG");
        g_log = e ? std::atoi(e) : 0;
    }
    return g_log;
}

unsigned fnv(unsigned h, const void *p, unsigned n)
{
    const unsigned char *b = (const unsigned char *)p;
    for (unsigned i = 0; i < n; ++i) { h ^= b[i]; h *= 16777619u; }
    return h;
}

unsigned q555(unsigned rgb) { return rgb & 0xF8F8F8u; }

// the row's pick for chr, or -1 when it is no pick (unset, or the cartridge's own colour)
long pick_of(const Picks &p, int chr, int row)
{
    if (chr < 0 || chr > 2 || !p.set[chr][row]) return -1;
    const unsigned v = p.rgb[chr][row] & 0xFFFFFFu;
    if (q555(v) == q555(vscg::kBroCart[chr][kCatOfRow[row]])) return -1;
    return (long)v;
}

int row_of_cat(int cat)
{
    for (int r = 0; r < kRows; ++r)
        if (kCatOfRow[r] == cat) return r;
    return -1;
}

void reset_if_dropped()
{
    const unsigned gen = ntr::gx_texture_generation();
    if (g_gen_have && gen == g_gen) return;
    if (g_gen_have && log_level() > 0 && g_nreg)
        std::fprintf(stderr, "[bro] texture cache dropped (generation %u): %d copies released\n", gen, g_nreg);
    for (int i = 0; i < g_nreg; ++i) std::free(g_reg[i].words);
    g_nreg = 0;
    g_bump = kWinBase;
    g_gen = gen;
    g_gen_have = true;
}

// the copy of palette span [src_off, src_off + size) named `name`, recoloured with p; its base offset or 0
unsigned copy_for(const char *name, unsigned src_off, unsigned size, const Picks &p)
{
    if (!name || size < 2 || size > 0x2000 || src_off + size > 0x18000) return 0;
    /* the groups this palette has, with a real pick */
    const vscg::Group *gs[8];
    long pk[8];
    int ng = 0;
    for (int i = 0; i < vscg::kVscGroupCount && ng < 8; ++i) {
        const vscg::Group *g = &vscg::kVscGroups[i];
        if (std::strcmp(g->pal, name) != 0) continue;
        const int r = row_of_cat(g->cat);
        const long v = r >= 0 ? pick_of(p, g->chr, r) : -1;
        if (v < 0) continue;
        gs[ng] = g;
        pk[ng] = v;
        ++ng;
    }
    if (!ng) return 0;
    const unsigned short *src = (const unsigned short *)(kPltt + src_off);
    unsigned key = fnv(2166136261u, src, size);
    key = fnv(key, &src_off, 4);
    for (int k = 0; k < ng; ++k) {
        key = fnv(key, &gs[k], sizeof gs[k]);
        key = fnv(key, &pk[k], sizeof pk[k]);
    }
    for (int i = 0; i < g_nreg; ++i) {
        Region &r = g_reg[i];
        if (r.key != key || r.src_off != src_off || r.size != size) continue;
        unsigned short *dst = (unsigned short *)(kPltt + r.off);
        if (std::memcmp(dst, r.words, size) != 0) std::memcpy(dst, r.words, size);
        return r.off;
    }
    /* a new copy: same offset within 16 bytes as the source, so base + delta lands on it */
    const unsigned off = ((g_bump + 15u) & ~15u) + (src_off & 15u);
    if (g_nreg >= kRegions || off + size > kWinEnd) {
        static bool said;
        if (!said) {
            said = true;
            std::fprintf(stderr, "[bro] palette window full (%d copies): further bros keep the cartridge's colours until the next level load\n", g_nreg);
        }
        return 0;
    }
    unsigned short *w = (unsigned short *)std::malloc(size);
    if (!w) return 0;
    std::memcpy(w, src, size);
    const int ncol = (int)(size / 2);
    for (int k = 0; k < ng; ++k) {
        const unsigned v = (unsigned)pk[k];
        const vspal::u8 t[3] = { (vspal::u8)(v >> 16), (vspal::u8)(v >> 8), (vspal::u8)v };
        const char *why = vspal::char_recolor_palette((vspal::u16 *)w, ncol, gs[k]->idx, gs[k]->count, t, gs[k]->main);
        if (why && log_level() > 0) std::fprintf(stderr, "[bro] %s: %s\n", name, why);
    }
    std::memcpy((void *)(kPltt + off), w, size);
    Region &r = g_reg[g_nreg++];
    r.src_off = src_off;
    r.size = size;
    r.key = key;
    r.off = off;
    r.words = w;
    g_bump = off + size;
    if (log_level() > 0)
        std::fprintf(stderr, "[bro] copy %d: %s %05x+%u -> %05x (%d categor%s)\n", g_nreg - 1, name, src_off, size, off, ng, ng == 1 ? "y" : "ies");
    return off;
}

}  // namespace

bool any(const Picks &p)
{
    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < kRows; ++r)
            if (pick_of(p, c, r) >= 0) return true;
    return false;
}

// point material i's base (*pb) at its recoloured copy when its palette has a pick; true when it did.
// dst caches copy_for per palette j for one walk (0xFFFFFFFF: not asked yet).
static bool stamp_base(int *pb, const unsigned char *hdr, unsigned *dst, const Picks &p)
{
    unsigned np = 0, pals = 0;
    std::memcpy(&np, hdr + 0x1c, 4);                 /* numPalettes */
    std::memcpy(&pals, hdr + 0x20, 4);               /* palettes: name, data, size, vramOffset */
    const unsigned at = (unsigned)*pb * 16u;
    for (unsigned j = 0; j < np; ++j) {
        const unsigned char *pr = (const unsigned char *)(size_t)(pals + j * 0x10);
        unsigned nmp = 0, size = 0, voff = 0;
        std::memcpy(&nmp, pr + 0, 4);
        std::memcpy(&size, pr + 8, 4);
        std::memcpy(&voff, pr + 0xc, 4);
        if (at < voff || at >= voff + size) continue;
        if (dst[j] == 0xFFFFFFFFu) dst[j] = copy_for((const char *)(size_t)nmp, voff, size, p);
        if (!dst[j]) return false;
        *pb += (int)((dst[j] - voff) / 16u);
        return true;
    }
    return false;
}

void stamp_model(Stamp &st, int k, char *mdl, const Picks &p)
{
    st.rec[k] = 0;
    st.hdr[k] = 0;
    st.n[k] = 0;
    if (!mdl) return;
    const int *mc = (const int *)(mdl + 8);           /* ModelComponents at +0x8 */
    const unsigned char *hdr = (const unsigned char *)(size_t)(unsigned)mc[0];   /* BMD_File */
    char *rec = (char *)(size_t)(unsigned)mc[1];     /* runtime material records, 0x30 each */
    if (!hdr || !rec) return;
    unsigned nm = 0, np = 0, pals = 0;
    std::memcpy(&nm, hdr + 0x24, 4);                 /* numMaterials */
    std::memcpy(&np, hdr + 0x1c, 4);                 /* numPalettes */
    std::memcpy(&pals, hdr + 0x20, 4);               /* palettes: name, data, size, vramOffset */
    if (nm == 0 || nm > (unsigned)kMaxMat || np == 0 || np > 16 || !pals) return;
    reset_if_dropped();
    unsigned dst[16];
    for (unsigned j = 0; j < np; ++j) dst[j] = 0xFFFFFFFFu;
    for (unsigned i = 0; i < nm; ++i) {
        int *pb = (int *)(rec + i * 0x30 + 0x20);
        st.old[k][i] = *pb;
        st.hit[k][i] = stamp_base(pb, hdr, dst, p) ? 1 : 0;
        st.now[k][i] = *pb;
    }
    /* kept with no hit too: a texture sequence can move a material onto a picked palette before the mirror draw */
    st.rec[k] = rec;
    st.hdr[k] = hdr;
    st.n[k] = nm;
}

void restamp(Stamp &st, const Picks &p)
{
    for (int k = 0; k < 2; ++k) {
        if (!st.rec[k] || !st.hdr[k]) continue;
        unsigned np = 0;
        std::memcpy(&np, st.hdr[k] + 0x1c, 4);
        unsigned dst[16];
        for (unsigned j = 0; j < np && j < 16; ++j) dst[j] = 0xFFFFFFFFu;
        for (unsigned i = 0; i < st.n[k]; ++i) {
            int *pb = (int *)(st.rec[k] + i * 0x30 + 0x20);
            if (*pb == st.now[k][i]) continue;           /* still what this draw stamped (or left) */
            st.old[k][i] = *pb;
            st.hit[k][i] = stamp_base(pb, st.hdr[k], dst, p) ? 1 : 0;
            st.now[k][i] = *pb;
        }
    }
}

void unstamp(Stamp &st)
{
    for (int k = 1; k >= 0; --k)
        for (unsigned i = 0; i < st.n[k]; ++i) {
            if (!st.hit[k][i]) continue;
            int *pb = (int *)(st.rec[k] + i * 0x30 + 0x20);
            if (*pb == st.now[k][i]) *pb = st.old[k][i];
        }
    st.n[0] = st.n[1] = 0;
}

}  // namespace bro
}  // namespace port
