/* vs_palette_gen.h -- build a Yoshi VS palette row from two picked colours.
 *
 * WHAT THIS IS. The owner asked for "the ability to choose a hex color to make
 * your yoshi and its shoes so two [colors]", and he named where the rules come
 * from: "it will show you what parts of the texture you need to change and how
 * the blending and shading works" -- the palette section of tangOS SM64DS
 * Edition. This header is that section's arithmetic, ported, so the game
 * recolours a Yoshi the same way the Studio's own panel does.
 *
 * HEADER-ONLY AND PURE ON PURPOSE. Every function here is a function of its
 * arguments and nothing else: no globals, no allocation, no I/O, no clock, no
 * randomness. That is what makes the online story work. Nothing about a colour
 * is ever put on the wire -- each launcher is told the two hex values every
 * player picked and every copy of the game computes every player's sixteen
 * palette words for itself. Identical inputs through identical arithmetic give
 * identical bytes on every machine, so there is no blob to trust, no payload to
 * relay, and nothing a hostile client can inject beyond a colour. It is also
 * why this lives in a header: the same code compiles into the game and into the
 * unit-test binary, so the tests test the shipped arithmetic.
 *
 * ---- WHERE EVERY RULE COMES FROM ------------------------------------------
 *
 * All citations are C:\Users\bmanu\Documents\tangOS-SM64DS\app\src\
 * palette_editor.rs at 35376a8. That file is read-only to this lane; nothing
 * here changes it, and every constant and formula below is transcribed rather
 * than reinvented.
 *
 *   word_rgb / rgb_word           :129 / :139
 *   srgb_to_linear/linear_to_srgb :213 / :217
 *   srgb_to_oklab                 :221  (Ottosson's published matrices)
 *   oklab_to_linear               :242
 *   in_gamut                      :253
 *   srgb_to_oklch                 :265
 *   word_oklch                    :274
 *   max_chroma_at                 :290
 *   gamut_chroma                  :307
 *   hue_cusp                      :313
 *   oklch_to_srgb_mapped          :355
 *   rgb_hsl_f                     :155
 *   rep_score                     :2458
 *   family_edit                   :2854
 *   apply_transform               :2884
 *   SAT_CUTOFF 0.15               :1683
 *   OK_L_MIN   0.02               :1708
 *   OK_L_CEIL  0.95               :1713
 *   OK_C_MIN   0.004              :1716
 *
 * WHY OKLAB AND NOT HSL, in the Studio's own words at :203: "HSL's 'lightness'
 * is a number, not a brightness", so shading measured in HSL means something
 * different for every hue. The two properties that matter to a player picking a
 * colour are that his Yoshi still has shading, and that the shading still looks
 * like the colour he picked. Oklab is what buys both.
 *
 * ---- WHICH SIXTEEN WORDS, AND WHICH OF THEM ARE WHAT -----------------------
 *
 * data/player/yoshi_model.bmd (ARCHIVE/arc0.narc member 196) holds one palette
 * named yoshi_all_16p_pl, 128 bytes: FOUR stacked sixteen-colour rows, one per
 * VS player. fs_mods.cpp's PaletteYoshi block derives the whole arrangement and
 * this header does not restate it.
 *
 * The group membership -- the owner's "what parts of the texture you need to
 * change" -- is read off the ROM's own four rows rather than guessed, and it is
 * verified by kVsRomRows below, which is the retail palette verbatim:
 *
 *   BODY  0,1,2,3   the skin ramp. Green in row 0, red in row 1, blue in
 *                   row 2, yellow in row 3 -- the four VS Yoshis.
 *   SHOES 7,8       the boots. Orange under the green Yoshi, blue under the
 *                   red one, magenta under the blue one, green under the
 *                   yellow one. Two shades: a dark and a bright.
 *   FIXED the rest  4,5,6,9,10,11,12,13,14,15 are left exactly as the ROM
 *                   authored row 0.
 *
 * Nine of those ten are byte-identical in all four ROM rows -- near-black, the
 * eye yellow, two reds, a dark red and four greys and whites -- so the ROM
 * itself says they are not anybody's player colour. Index 11 is the exception
 * and it is the one judgement call in this file: it DOES move between rows, but
 * it does not follow the body hue and it does not follow the boot hue (it is
 * gold at row 0, gold at row 1, yellow at row 2, orange at row 3, while the
 * boots go orange/blue/magenta/green underneath it). It is left on the ROM's
 * value, which is the same thing the other nine fixed slots get, because
 * putting it in the wrong group is visible and leaving it is not.
 *
 * WHAT WAS NOT MIRRORED, said plainly. The Studio decides group membership by
 * counting how many texels of each rig part reference each palette entry
 * (PartCoverage, :1806, with bone_group at :1757 putting every bone whose name
 * starts "ankle" in the shoes part). Reproducing that needs the model geometry
 * and the texture decoder, which is the Studio's whole asset pipeline. What is
 * mirrored exactly is the arithmetic the owner actually named -- the blending
 * and shading -- and the group membership is taken from the stronger evidence
 * for this particular question: which slots the ROM's own artists moved when
 * they authored the four players.
 */

#ifndef PORT_HAL_VS_PALETTE_GEN_H
#define PORT_HAL_VS_PALETTE_GEN_H

#include <math.h>

namespace vspal {

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

/* ---- the ROM's own four rows, for the tests and for a shape check ---------
 *
 * Read out of the retail cartridge (0022 - Super Mario 64 DS.nds,
 * ARCHIVE/arc0.narc member 196, palette yoshi_all_16p_pl at offset 14168) and
 * pasted here verbatim. Nothing at runtime reads these -- the game always uses
 * the player's own extraction -- but the unit tests do, and a build whose
 * arithmetic drifts is caught against real ROM bytes rather than against a
 * fixture somebody typed. */
static const u16 kVsRomRows[4][16] = {
    { 0x0d83, 0x1224, 0x1a86, 0x2eab, 0x0022, 0x075d, 0x084e, 0x0d36,
      0x0d9c, 0x107c, 0x1093, 0x1a99, 0x56d7, 0x675a, 0x77bd, 0x7fff },
    { 0x1094, 0x083b, 0x1cdf, 0x357c, 0x0022, 0x075d, 0x084e, 0x6500,
      0x7d20, 0x107c, 0x1093, 0x167a, 0x56d7, 0x675a, 0x77bd, 0x7fff },
    { 0x34c5, 0x4d07, 0x5949, 0x72ad, 0x0022, 0x075d, 0x084e, 0x4c33,
      0x5856, 0x107c, 0x1093, 0x0dbd, 0x56d7, 0x675a, 0x77bd, 0x7fff },
    { 0x167c, 0x22de, 0x033f, 0x579e, 0x0022, 0x075d, 0x084e, 0x02a6,
      0x06e6, 0x107c, 0x1093, 0x167d, 0x56d7, 0x675a, 0x77bd, 0x7fff },
};

/* The two groups, and the fact that the rest are left alone. */
static const int kBodyIdx[4] = { 0, 1, 2, 3 };
static const int kShoeIdx[2] = { 7, 8 };

/* palette_editor.rs:1683, :1708, :1713, :1716 */
static const float kSatCutoff = 0.15f;
static const float kOkLMin    = 0.02f;
static const float kOkLCeil   = 0.95f;
static const float kOkCMin    = 0.004f;

/* ---- colour conversion ---------------------------------------------------*/

/* palette_editor.rs:129. BGR555 word -> 8-bit RGB. */
static inline void word_rgb(u16 w, u8 out[3])
{
    u32 r = (u32)(w & 0x1f), g = (u32)((w >> 5) & 0x1f), b = (u32)((w >> 10) & 0x1f);
    out[0] = (u8)(r * 255 / 31);
    out[1] = (u8)(g * 255 / 31);
    out[2] = (u8)(b * 255 / 31);
}

/* palette_editor.rs:139. 8-bit RGB -> BGR555, carrying `old`'s bit 15 through
 * untouched, and ROUNDING to the nearest of the 32 levels rather than
 * truncating -- `v >> 3` always rounds down and biases every quantisation
 * darker. */
static inline u16 rgb_word(const u8 rgb[3], u16 old)
{
    u16 r = (u16)((((u32)rgb[0] * 31 + 127) / 255) & 0x1f);
    u16 g = (u16)((((u32)rgb[1] * 31 + 127) / 255) & 0x1f);
    u16 b = (u16)((((u32)rgb[2] * 31 + 127) / 255) & 0x1f);
    return (u16)((old & 0x8000) | (u16)(b << 10) | (u16)(g << 5) | r);
}

static inline void unit_rgb(const u8 rgb[3], float out[3])
{
    out[0] = (float)rgb[0] / 255.0f;
    out[1] = (float)rgb[1] / 255.0f;
    out[2] = (float)rgb[2] / 255.0f;
}

static inline void rgb_u8(const float unit[3], u8 out[3])
{
    for (int i = 0; i < 3; ++i) {
        float v = unit[i] * 255.0f;
        v = (float)floor((double)v + 0.5);      /* Rust f32::round: half away from zero */
        if (v < 0.0f) v = 0.0f;
        if (v > 255.0f) v = 255.0f;
        out[i] = (u8)v;
    }
}

static inline float rem_euclid(float v, float m)
{
    float r = fmodf(v, m);
    return r < 0.0f ? r + fabsf(m) : r;
}

/* palette_editor.rs:155. Unit-range RGB -> HSL, hue in degrees. */
static inline void rgb_hsl_f(const float rgb[3], float out[3])
{
    float r = rgb[0], g = rgb[1], b = rgb[2];
    float mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
    float mn = r < g ? (r < b ? r : b) : (g < b ? g : b);
    float l = (mx + mn) / 2.0f;
    float d = mx - mn;
    if (d <= 1.1920929e-7f) {           /* f32::EPSILON */
        out[0] = 0.0f; out[1] = 0.0f; out[2] = l;
        return;
    }
    float s = (l > 0.5f) ? d / (2.0f - mx - mn) : d / (mx + mn);
    float h;
    if (mx == r)      h = 60.0f * fmodf((g - b) / d, 6.0f);
    else if (mx == g) h = 60.0f * ((b - r) / d + 2.0f);
    else              h = 60.0f * ((r - g) / d + 4.0f);
    if (s < 0.0f) s = 0.0f;
    if (s > 1.0f) s = 1.0f;
    out[0] = rem_euclid(h, 360.0f);
    out[1] = s;
    out[2] = l;
}

static inline void word_hsl(u16 w, float out[3])
{
    u8 c[3]; float u[3];
    word_rgb(w, c);
    unit_rgb(c, u);
    rgb_hsl_f(u, out);
}

/* palette_editor.rs:213/:217 */
static inline float srgb_to_linear(float c)
{
    return (c <= 0.04045f) ? c / 12.92f : powf((c + 0.055f) / 1.055f, 2.4f);
}

static inline float linear_to_srgb(float c)
{
    return (c <= 0.0031308f) ? c * 12.92f : 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
}

/* palette_editor.rs:221. Ottosson's published Oklab matrices, unchanged. */
static inline void srgb_to_oklab(const float rgb[3], float out[3])
{
    float r = srgb_to_linear(rgb[0]);
    float g = srgb_to_linear(rgb[1]);
    float b = srgb_to_linear(rgb[2]);
    float l = 0.41222147f * r + 0.53633255f * g + 0.051445995f * b;
    float m = 0.2119035f  * r + 0.6806995f  * g + 0.10739696f  * b;
    float s = 0.08830246f * r + 0.28171885f * g + 0.6299787f   * b;
    float l_ = cbrtf(l), m_ = cbrtf(m), s_ = cbrtf(s);
    out[0] = 0.21045426f  * l_ + 0.7936178f  * m_ - 0.004072047f * s_;
    out[1] = 1.9779985f   * l_ - 2.4285922f  * m_ + 0.4505937f   * s_;
    out[2] = 0.025904037f * l_ + 0.78277177f * m_ - 0.80867577f  * s_;
}

/* palette_editor.rs:242 */
static inline void oklab_to_linear(const float lab[3], float out[3])
{
    float l_ = lab[0] + 0.39633778f  * lab[1] + 0.21580376f  * lab[2];
    float m_ = lab[0] - 0.105561346f * lab[1] - 0.06385417f  * lab[2];
    float s_ = lab[0] - 0.08948418f  * lab[1] - 1.2914855f   * lab[2];
    float l = l_ * l_ * l_, m = m_ * m_ * m_, s = s_ * s_ * s_;
    out[0] =  4.0767417f    * l - 3.3077116f  * m + 0.23096994f * s;
    out[1] = -1.268438f     * l + 2.6097574f  * m - 0.34131938f * s;
    out[2] = -0.0041960863f * l - 0.7034186f  * m + 1.7076147f  * s;
}

/* palette_editor.rs:253 */
static inline int in_gamut(const float lin[3])
{
    for (int i = 0; i < 3; ++i)
        if (!(lin[i] >= -1e-4f && lin[i] <= 1.0001f))
            return 0;
    return 1;
}

static inline void oklab_to_srgb(const float lab[3], float out[3])
{
    float lin[3];
    oklab_to_linear(lab, lin);
    out[0] = linear_to_srgb(lin[0]);
    out[1] = linear_to_srgb(lin[1]);
    out[2] = linear_to_srgb(lin[2]);
}

/* palette_editor.rs:265. Oklab in cylindrical form. */
static inline void srgb_to_oklch(const float rgb[3], float out[3])
{
    float lab[3];
    srgb_to_oklab(rgb, lab);
    out[0] = lab[0];
    out[1] = sqrtf(lab[1] * lab[1] + lab[2] * lab[2]);
    out[2] = rem_euclid(atan2f(lab[2], lab[1]) * (180.0f / 3.14159265358979323846f), 360.0f);
}

/* palette_editor.rs:274 */
static inline void word_oklch(u16 w, float out[3])
{
    u8 c[3]; float u[3];
    word_rgb(w, c);
    unit_rgb(c, u);
    srgb_to_oklch(u, out);
}

/* palette_editor.rs:290. The most chroma this hue can carry at this lightness,
 * by eighteen steps of bisection -- the same count, so the same answer. */
static inline float max_chroma_at(float l, float ch, float sh)
{
    float lo = 0.0f, hi = 0.5f;
    for (int i = 0; i < 18; ++i) {
        float mid = 0.5f * (lo + hi);
        float lab[3], lin[3];
        lab[0] = l; lab[1] = mid * ch; lab[2] = mid * sh;
        oklab_to_linear(lab, lin);
        if (in_gamut(lin)) lo = mid; else hi = mid;
    }
    return lo;
}

/* palette_editor.rs:307 */
static inline float gamut_chroma(float l, float h)
{
    float r = h * (3.14159265358979323846f / 180.0f);
    return max_chroma_at(l, cosf(r), sinf(r));
}

/* palette_editor.rs:313. The sRGB gamut cusp for a hue: the lightness at which
 * it holds the most chroma, and that chroma. A 360-entry table, built once,
 * exactly as the Studio builds it (a coarse 1/32 sweep, then a refinement of
 * sixteen steps at an eighth of that). */
struct CuspTable {
    float l[360];
    float c[360];
    int   built;
};

static inline CuspTable *cusp_table(void)
{
    static CuspTable t;
    if (!t.built) {
        for (int deg = 0; deg < 360; ++deg) {
            float r = (float)deg * (3.14159265358979323846f / 180.0f);
            float ch = cosf(r), sh = sinf(r);
            float bl = 0.0f, bc = 0.0f;
            for (int i = 1; i < 32; ++i) {
                float l = (float)i / 32.0f;
                float c = max_chroma_at(l, ch, sh);
                if (c > bc) { bl = l; bc = c; }
            }
            {
                float step = 1.0f / 32.0f;
                float base = bl;
                for (int i = -8; i <= 8; ++i) {
                    float l = base + step * (float)i / 8.0f;
                    if (l < 0.0f) l = 0.0f;
                    if (l > 1.0f) l = 1.0f;
                    float c = max_chroma_at(l, ch, sh);
                    if (c > bc) { bl = l; bc = c; }
                }
            }
            t.l[deg] = bl;
            t.c[deg] = bc;
        }
        t.built = 1;
    }
    return &t;
}

static inline float hue_cusp_l(float h)
{
    int i = (int)rem_euclid(h, 360.0f);
    if (i > 359) i = 359;
    if (i < 0) i = 0;
    return cusp_table()->l[i];
}

/* palette_editor.rs:355. Oklch -> sRGB, gamut-mapped by projecting toward the
 * hue's own cusp. Winding chroma down at a fixed lightness bleaches; sliding
 * toward the cusp trades a little lightness and keeps the colour. */
static inline void oklch_to_srgb_mapped(const float lch[3], float out[3])
{
    float l = lch[0];
    if (l < 0.0f) l = 0.0f;
    if (l > 1.0f) l = 1.0f;
    float c = lch[1] > 0.0f ? lch[1] : 0.0f;
    float h = lch[2];
    float r = h * (3.14159265358979323846f / 180.0f);
    float ch = cosf(r), sh = sinf(r);
    float lab[3], lin[3];

    lab[0] = l; lab[1] = c * ch; lab[2] = c * sh;
    oklab_to_linear(lab, lin);
    if (in_gamut(lin)) {
        oklab_to_srgb(lab, out);
    } else {
        float cusp_l = hue_cusp_l(h);
        float lo = 0.0f, hi = 1.0f;
        for (int i = 0; i < 22; ++i) {
            float t = 0.5f * (lo + hi);
            float tl = cusp_l + (l - cusp_l) * t;
            float tc = c * t;
            float a2[3], l2[3];
            a2[0] = tl; a2[1] = tc * ch; a2[2] = tc * sh;
            oklab_to_linear(a2, l2);
            if (in_gamut(l2)) lo = t; else hi = t;
        }
        {
            float tl = cusp_l + (l - cusp_l) * lo;
            float tc = c * lo;
            lab[0] = tl; lab[1] = tc * ch; lab[2] = tc * sh;
            oklab_to_srgb(lab, out);
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (out[i] < 0.0f) out[i] = 0.0f;
        if (out[i] > 1.0f) out[i] = 1.0f;
    }
}

/* ---- the family transform ------------------------------------------------*/

/* palette_editor.rs:2458. Which swatch stands for a family: the one that shows
 * its hue most clearly. High saturation wins, distance from mid lightness
 * loses, so a run of shades is represented by its punchiest mid-tone. */
static inline float rep_score(const float hsl[3])
{
    return hsl[1] - fabsf(hsl[2] - 0.5f);
}

/* palette_editor.rs:2854, family_edit. The transform a family carries: the
 * colour the owner picked, plus the anchor and the shading span read off the
 * ROM, plus whether the family is a neutral one. */
struct FamilyEdit {
    float target[3];    /* unit RGB */
    float anchor[3];    /* Oklch of the representative's ROM colour */
    float span[2];      /* how far the family's ROM lightness reached below/above */
    int   neutral;
};

/* Rust's `max_by` keeps the LAST maximum on a tie, so `>=` here, not `>`. */
static inline int family_rep(const u16 *row, const int *idx, int n)
{
    int best = idx[0];
    float bs;
    {
        float hsl[3];
        word_hsl(row[idx[0]], hsl);
        bs = rep_score(hsl);
    }
    for (int i = 1; i < n; ++i) {
        float hsl[3];
        word_hsl(row[idx[i]], hsl);
        float s = rep_score(hsl);
        if (s >= bs) { bs = s; best = idx[i]; }
    }
    return best;
}

static inline void family_edit(const u16 *row, const int *idx, int n,
                               const u8 target[3], FamilyEdit *out)
{
    int rep = family_rep(row, idx, n);
    float dmin = 0.0f, dmax = 0.0f;
    float hsl[3];

    word_oklch(row[rep], out->anchor);
    for (int i = 0; i < n; ++i) {
        float m[3];
        word_oklch(row[idx[i]], m);
        float d = m[0] - out->anchor[0];
        if (d < 0.0f) { if (-d > dmin) dmin = -d; }
        else          { if (d > dmax) dmax = d; }
    }
    unit_rgb(target, out->target);
    out->span[0] = dmin;
    out->span[1] = dmax;
    word_hsl(row[rep], hsl);
    out->neutral = hsl[1] < kSatCutoff;
}

/* palette_editor.rs:2884, apply_transform.
 *
 * Every member is recomputed FROM ITS ROM ORIGINAL, so the transform is
 * absolute rather than cumulative. Two rules carry the Studio's hard-won
 * corrections and both are transcribed rather than simplified:
 *
 * LIGHTNESS. Brightening keeps the ABSOLUTE perceptual gap below the anchor, so
 * the family reads as the colour that was picked with its shading intact;
 * darkening keeps the PROPORTIONAL gap, so shadows compress instead of crushing
 * to black. Above the anchor the gap is squeezed into whatever headroom is left
 * under OK_L_CEIL, so highlights stay tinted instead of all clamping to white.
 * Scaling proportionally in both directions is what turned a red ramp into
 * olive when it was recoloured yellow: red's ceiling is L=0.63 and yellow's is
 * L=0.97, so 67-95% of red lands at 67-95% of yellow, and yellow at two thirds
 * brightness is olive.
 *
 * CHROMA travels in GAMUT-RELATIVE terms, not as a raw ratio. sRGB cannot hold
 * much chroma at high lightness for any hue, so a light pick always has a small
 * chroma, and charging that smallness to every member is what made recoloured
 * models look chalky. Each member's ROM chroma is read as a FRACTION of the
 * most chroma sRGB holds at that member's own (L,h); the target is read the
 * same way at its own; and the ratio between those fractions is what travels.
 * At the anchor both readings are the same number, so the representative lands
 * exactly on the picked colour and an identity edit is still the identity. */
static inline void apply_transform(const u16 *rom_row, const int *idx, int n,
                                   const FamilyEdit *e, u16 *out_row)
{
    float t[3], r[3];
    srgb_to_oklch(e->target, t);
    r[0] = e->anchor[0]; r[1] = e->anchor[1]; r[2] = e->anchor[2];

    float dmin = e->span[0] > 1e-4f ? e->span[0] : 1e-4f;
    float dmax = e->span[1] > 1e-4f ? e->span[1] : 1e-4f;
    float k_down;
    if (t[0] >= r[0]) {
        k_down = t[0] / dmin;
        if (k_down > 1.0f) k_down = 1.0f;
    } else {
        k_down = t[0] / (r[0] > kOkLMin ? r[0] : kOkLMin);
    }
    float k_up = (kOkLCeil - t[0]);
    if (k_up < 0.0f) k_up = 0.0f;
    k_up /= dmax;
    if (k_up > 1.0f) k_up = 1.0f;

    float sat_scale;
    if (e->neutral || r[1] < kOkCMin) {
        sat_scale = 1.0f;
    } else {
        float gt = gamut_chroma(t[0], t[2]);
        if (gt < kOkCMin) gt = kOkCMin;
        float gr = gamut_chroma(r[0], r[2]);
        if (gr < kOkCMin) gr = kOkCMin;
        float den = r[1] / gr;
        if (den < 1e-4f) den = 1e-4f;
        sat_scale = (t[1] / gt) / den;
    }

    for (int i = 0; i < n; ++i) {
        int ci = idx[i];
        u16 rom = rom_row[ci];
        float m[3];
        word_oklch(rom, m);
        float d = m[0] - r[0];
        float l = t[0] + d * (d <= 0.0f ? k_down : k_up);
        if (l < 0.0f) l = 0.0f;
        if (l > 1.0f) l = 1.0f;
        float h = e->neutral ? t[2] : rem_euclid(m[2] + (t[2] - r[2]), 360.0f);
        float c;
        if (e->neutral) {
            c = t[1];
        } else {
            float head_rom = gamut_chroma(m[0], m[2]);
            float share = (head_rom < kOkCMin) ? 0.0f : m[1] / head_rom;
            float v = share * sat_scale;
            if (v < 0.0f) v = 0.0f;
            if (v > 1.0f) v = 1.0f;
            c = v * gamut_chroma(l, h);
        }
        {
            float lch[3], unit[3];
            u8 rgb[3];
            lch[0] = l; lch[1] = c; lch[2] = h;
            oklch_to_srgb_mapped(lch, unit);
            rgb_u8(unit, rgb);
            out_row[ci] = rgb_word(rgb, rom);
        }
    }
}

/* ---- the one call the game makes -----------------------------------------
 *
 * Take the ROM's sixteen words for a Yoshi, a body colour and a shoe colour,
 * and write the sixteen words that Yoshi should wear. Slots outside the two
 * groups come through untouched, so a row generated from the ROM's own row-0
 * colours is the ROM's own row 0, bit for bit. */
static inline void vs_palette_row(const u16 rom_row[16],
                                  const u8 body_rgb[3], const u8 shoe_rgb[3],
                                  u16 out_row[16])
{
    FamilyEdit e;
    for (int i = 0; i < 16; ++i)
        out_row[i] = rom_row[i];
    family_edit(rom_row, kBodyIdx, 4, body_rgb, &e);
    apply_transform(rom_row, kBodyIdx, 4, &e, out_row);
    family_edit(rom_row, kShoeIdx, 2, shoe_rgb, &e);
    apply_transform(rom_row, kShoeIdx, 2, &e, out_row);
}

/* ---- the wire form: "bbbbbb:ssssss" --------------------------------------*/

static inline int hex_nib(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

/* Parse exactly six hex digits into an 8-bit RGB triple. 1 on success. */
static inline int parse_hex6(const char *s, u8 out[3])
{
    int v[6];
    for (int i = 0; i < 6; ++i) {
        v[i] = hex_nib(s[i]);
        if (v[i] < 0) return 0;
    }
    out[0] = (u8)((v[0] << 4) | v[1]);
    out[1] = (u8)((v[2] << 4) | v[3]);
    out[2] = (u8)((v[4] << 4) | v[5]);
    return 1;
}

} /* namespace vspal */

#endif /* PORT_HAL_VS_PALETTE_GEN_H */
