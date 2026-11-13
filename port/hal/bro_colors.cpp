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
const int kCatOfRow[kRows] = { vscg::CAT_HAT, vscg::CAT_OVERALLS, vscg::CAT_SHOES, vscg::CAT_UNDERSHIRT };

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

// the row's pick for chr, or -1 when it is no pick (unset, or the cartridge's own colour). PALSPLIT1: an undershirt
// with no pick of its own wears the hat's (the two were one row; the hat's cartridge colour is the undershirt's).
long pick_of(const Picks &p, int chr, int row)
{
    if (chr >= 0 && chr <= 2 && row == kRowUndershirt && !p.set[chr][row]) row = kRowHat;
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

// ---- BROSCOLOR1 (run coopfix1): the room's colour choices, the character-palettes mod's list. See bro_colors.h. ----
extern "C" int host_setting_online_bro(int chr, int cat, unsigned *rgb);                       /* hal/host_settings.h */
extern "C" int host_setting_set_online_bros(const unsigned rgb[3][4], const unsigned char set[3][4]);
extern "C" int host_setting_online_colours(unsigned *body, unsigned *shoes);
extern "C" int host_setting_set_online_profile(const char *name, unsigned body, unsigned shoes);
extern "C" const char *host_setting_online_name(void);
extern "C" int port_fs_mods_bro_custom(int chr, int part, unsigned *rgb);                       /* hal/fs_mods.cpp */

namespace port {
namespace bro {
namespace {

const char *const kBroChoice[2] = { "Original", "Custom" };
// Yoshi's eight: the profile popup's eight cells, in its order (hal/menu_online_profile.h kOnlineYoshiColors: the cartridge's four
// multiplayer rows as the classic cells give them, the drawn body and shoe shade of each row, then Pink, Cyan, Purple, Orange),
// then Custom. A popup pick reads back as its own name, never as Custom.
enum : int { kYoshiPresets = 8, kYoshiCustom = kYoshiPresets };
const char *const kYoshiChoice[kYoshiPresets + 1] = { "Green", "Red", "Blue", "Yellow", "Pink", "Cyan", "Purple", "Orange", "Custom" };
const unsigned kYoshiPair[kYoshiPresets][2] = {
    { 0x31A431u, 0xE66218u }, { 0xFF3139u, 0x004AFFu }, { 0x4A52B4u, 0xB410B4u }, { 0xFFCD00u, 0x31BD08u },
    { 0xFF7FBFu, 0x8A1F5Cu }, { 0x20D8E0u, 0x0E5E7Au }, { 0x8A4FD8u, 0x3A1A7Au }, { 0xFF8A1Eu, 0x7A3A10u },
};

// this session's memory of a Custom the player stepped away from, so Original then Custom brings it back
unsigned g_cust[3][kRows];
bool g_cust_have[3];
unsigned g_ycust[2];
bool g_ycust_have;
// Original picked this session: the mod's Custom stops standing in for that bro's online pick
bool g_orig_explicit[3];

unsigned cart_of(int chr, int row) { return vscg::kBroCart[chr][kCatOfRow[row]] & 0xFFFFFFu; }

bool online_any(int chr)
{
    for (int r = 0; r < kRows; ++r)
        if (host_setting_online_bro(chr, r, 0)) return true;
    return false;
}

// the rows a bro wears now (online pick, the mod standing in, else the cartridge); *picked when any row is a stored pick
void wearing(int chr, unsigned out[kRows], bool *picked)
{
    bool any_set = false;
    for (int r = 0; r < kRows; ++r) {
        unsigned v = 0;
        if (port_bro_my_row(chr, r, &v)) { out[r] = v & 0xFFFFFFu; any_set = true; }
        else out[r] = cart_of(chr, r);
    }
    if (picked) *picked = any_set;
}

bool not_cart(int chr, const unsigned rows[kRows])
{
    for (int r = 0; r < kRows; ++r)
        if (q555(rows[r]) != q555(cart_of(chr, r))) return true;
    return false;
}

// write one bro's rows (all set, or all unset), keeping the other two bros' rows as they are. All set writes the
// undershirt too, so a Custom that started from a joined hat keeps the shirt it showed when the hat moves (PALSPLIT1).
void write_bro(int chr, const unsigned rows[kRows], bool set)
{
    unsigned rgb[3][kRows];
    unsigned char st[3][kRows];
    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < kRows; ++r) {
            unsigned v = 0;
            if (c == chr) {
                st[c][r] = set ? 1 : 0;
                rgb[c][r] = set ? rows[r] & 0xFFFFFFu : 0;
                continue;
            }
            st[c][r] = host_setting_online_bro(c, r, &v) ? 1 : 0;
            rgb[c][r] = st[c][r] ? v : 0;
        }
    host_setting_set_online_bros(rgb, st);
}

bool yoshi_now(unsigned pair[2])
{
    if (host_setting_online_colours(&pair[0], &pair[1])) {
        pair[0] &= 0xFFFFFFu;
        pair[1] &= 0xFFFFFFu;
        return true;
    }
    pair[0] = kYoshiPair[0][0];
    pair[1] = kYoshiPair[0][1];
    return false;
}

int yoshi_preset_of(const unsigned pair[2])
{
    for (int i = 0; i < kYoshiPresets; ++i)
        if (q555(pair[0]) == q555(kYoshiPair[i][0]) && q555(pair[1]) == q555(kYoshiPair[i][1])) return i;
    return kYoshiCustom;
}

void yoshi_write(const unsigned pair[2])
{
    host_setting_set_online_profile(host_setting_online_name(), pair[0], pair[1]);
}

void choice_log(int chr, int i)
{
    const char *n = port_bro_choice_name(chr, i);
    std::fprintf(stderr, "[bro] choice set: chr %d -> %d (%s)\n", chr, i, n ? n : "?");
}

}  // namespace
}  // namespace bro
}  // namespace port

using namespace port::bro;

extern "C" int port_bro_my_row(int chr, int row, unsigned *rgb)
{
    if (chr < 0 || chr > 2 || row < 0 || row >= kRows) return 0;
    if (host_setting_online_bro(chr, row, rgb)) return 1;
    if (row == kRowUndershirt && host_setting_online_bro(chr, kRowHat, rgb)) return 1;   /* PALSPLIT1: a joined pick */
    if (online_any(chr) || g_orig_explicit[chr]) return 0;
    return port_fs_mods_bro_custom(chr, row, rgb);
}

extern "C" int port_bro_choice_count(int chr)
{
    return chr >= 0 && chr <= 2 ? 2 : chr == 3 ? kYoshiPresets + 1 : 0;
}

extern "C" const char *port_bro_choice_name(int chr, int i)
{
    if (i < 0 || i >= port_bro_choice_count(chr)) return 0;
    return chr == 3 ? kYoshiChoice[i] : kBroChoice[i];
}

extern "C" int port_bro_choice_current(int chr)
{
    if (chr == 3) {
        unsigned pair[2];
        yoshi_now(pair);
        return yoshi_preset_of(pair);
    }
    if (chr < 0 || chr > 2) return 0;
    bool picked = false;
    unsigned rows[kRows];
    wearing(chr, rows, &picked);
    return picked ? 1 : 0;
}

extern "C" int port_bro_choice_is_default(int chr)
{
    if (chr < 0) {
        for (int c = 0; c < 4; ++c)
            if (!port_bro_choice_is_default(c)) return 0;
        return 1;
    }
    if (chr == 3) {
        unsigned pair[2];
        return yoshi_now(pair) && yoshi_preset_of(pair) != 0 ? 0 : 1;
    }
    if (chr > 2) return 1;
    unsigned rows[kRows];
    wearing(chr, rows, 0);
    return not_cart(chr, rows) ? 0 : 1;
}

extern "C" int port_bro_custom_get(int chr, int part, unsigned *rgb)
{
    if (!rgb) return 0;
    if (chr == 3) {
        if (part < 0 || part > 1) return 0;
        unsigned pair[2];
        yoshi_now(pair);
        if (yoshi_preset_of(pair) != kYoshiCustom && g_ycust_have) {
            pair[0] = g_ycust[0];
            pair[1] = g_ycust[1];
        }
        *rgb = pair[part];
        return 1;
    }
    if (chr < 0 || chr > 2 || part < 0 || part >= kRows) return 0;
    bool picked = false;
    unsigned rows[kRows];
    wearing(chr, rows, &picked);
    *rgb = !picked && g_cust_have[chr] ? g_cust[chr][part] : rows[part];
    return 1;
}

extern "C" int port_bro_custom_set(int chr, int part, unsigned rgb)
{
    if (chr == 3) {
        if (part < 0 || part > 1) return 0;
        unsigned pair[2];
        port_bro_custom_get(3, 0, &pair[0]);
        port_bro_custom_get(3, 1, &pair[1]);
        pair[part] = rgb & 0xFFFFFFu;
        g_ycust[0] = pair[0];
        g_ycust[1] = pair[1];
        g_ycust_have = true;
        yoshi_write(pair);
        return 1;
    }
    if (chr < 0 || chr > 2 || part < 0 || part >= kRows) return 0;
    unsigned rows[kRows];
    for (int r = 0; r < kRows; ++r) port_bro_custom_get(chr, r, &rows[r]);
    rows[part] = rgb & 0xFFFFFFu;
    for (int r = 0; r < kRows; ++r) g_cust[chr][r] = rows[r];
    g_cust_have[chr] = true;
    g_orig_explicit[chr] = false;
    write_bro(chr, rows, true);
    return 1;
}

extern "C" int port_bro_choice_set(int chr, int i)
{
    if (i < 0 || i >= port_bro_choice_count(chr)) return 0;
    if (chr == 3) {
        unsigned pair[2];
        if (yoshi_now(pair) && yoshi_preset_of(pair) == kYoshiCustom) {
            g_ycust[0] = pair[0];
            g_ycust[1] = pair[1];
            g_ycust_have = true;
        }
        if (i < kYoshiPresets) {
            pair[0] = kYoshiPair[i][0];
            pair[1] = kYoshiPair[i][1];
        } else if (g_ycust_have) {
            pair[0] = g_ycust[0];
            pair[1] = g_ycust[1];
        }
        yoshi_write(pair);
        choice_log(chr, i);
        return 1;
    }
    bool picked = false;
    unsigned rows[kRows];
    wearing(chr, rows, &picked);
    if (i == 0) {
        if (picked) {
            for (int r = 0; r < kRows; ++r) g_cust[chr][r] = rows[r];
            g_cust_have[chr] = true;
        }
        g_orig_explicit[chr] = true;
        write_bro(chr, rows, false);
    } else {
        if (!picked && g_cust_have[chr])
            for (int r = 0; r < kRows; ++r) rows[r] = g_cust[chr][r];
        g_orig_explicit[chr] = false;
        write_bro(chr, rows, true);
    }
    choice_log(chr, i);
    return 1;
}

extern "C" unsigned port_bro_choice_swatch(int chr, int i)
{
    if (i < 0 || i >= port_bro_choice_count(chr)) return 0;
    unsigned v = 0;
    if (chr == 3) {
        if (i < kYoshiPresets) return kYoshiPair[i][0];
        port_bro_custom_get(3, 0, &v);
        return v;
    }
    if (i == 0) return cart_of(chr, kRowHat);
    port_bro_custom_get(chr, kRowHat, &v);
    return v;
}
