/* plain_rom_art.inc -- the plain skin from the game's own 2D art (run after093, lane PLAINSKIN1). Included by hal/settings_page.cpp right after
   its piece cache, inside its anonymous namespace, so the settings page and the pause page (hal/pause_page.inc) can both use it.

   WHAT IT IS FOR. With no HD UI Textures mod the port's own pause and settings pieces (the LB / RB tabs, the tracker's title plate, the star and
   coin icons, the settings sheet, its title / back / key plates and the reset plank) used to be flat shapes drawn by code. These are the same
   pieces built from the cartridge's own pause art instead, the art the DS's own buttons and panels on the same screens are made of.

   WHERE THE ART COMES FROM (nothing is stored in the repo or the exe; it is read from the player's own game files):
   - Background tiles: Stage::LoadGraphics2D (src/_ZN10dScStage_c14LoadGraphics2DEbi.cpp) loads the sub screen's pause tiles into BG0 char
     memory from archive file 0xa00d (English; 0xa40d / 0xa80d / 0xac0d / 0xb00d for the other four languages, LZ10) and their palette
     from 0x9807 to sub BG palette memory (sixteen banks of sixteen). The pause buttons (d_2d_framework_select_pause_nsc), the option fields
     (archive 0xa009) and the dialog panel (archive 0x9802) are 8 x 8 tile frames out of that one tile set:
       button  tiles  1  2  3 / 33 34 35 / 65 66 67 / 72 73 74   (the bottom row is the button's shadow line), bank 1 orange, bank 2 lit
       field   tiles 11 12 13 / 43 44 45 /  8  9 10 / 40 41 42 / 72 73 74, bank 6 cream, bank 5 grey
       panel   tiles 29 30 31 / 61 62 63 / 93 94 95, bank 4 (the tan dialog sheet)
     A frame of any size is the corner tiles whole, the edge tiles repeated along the edges and the middle tile repeated over the inside
     (the way the ROM's own maps make the 240 px buttons out of the same tiles), at a whole number of host pixels per DS pixel.
   - Sprites: dMeter_c::InitResources (src/ui/hud/d_meter.cpp) loads d_2d_cmn_icon_<lang>_ncg to OBJ char memory 0 and archive 0xa00a to OBJ
     palette banks 0..8 (2D mapping, 32 tiles a row). The HUD's own OamAttr entries pick the icons: OAM::POWER_STAR (ov001 0x020abad0: 16 x 16,
     tile 158, bank 6), OAM::SMALL_STAR_EMPTY (ov001 0x020abac0: tile 220, bank 8), OAM::COIN (ov001 0x020abad8: tile 154, bank 0) and the map's
     OAM::MM_RED_COIN (ov002 0x0210c6d8: tile 150, bank 0).
   Anything missing (a short file, a failed read) leaves the old code-drawn shape in place: every caller checks for a null piece. */

extern "C" const unsigned char *port_fs_archive_member(unsigned fileID, unsigned *len_out);   /* hal/fs.cpp */
extern "C" const char *port_fs_catalog_path(unsigned file_id);                                 /* hal/fs.cpp */
extern "C" int GetOwnerLanguage(void);

enum { PR_BUTTON = 0, PR_FIELD = 1, PR_PANEL = 2, PR_FRAMES };
enum { PR_STAR_GOT = 0, PR_STAR_OFF = 1, PR_COIN_GOLD = 2, PR_COIN_RED = 3, PR_SPRITES };

struct PlainRom { int state = 0; std::vector<uint8_t> bgch, objch; uint32_t bgpal[256] = {}, objpal[144] = {}; };
PlainRom g_pr;
std::mutex g_pr_mx;

uint32_t pr_rgb(unsigned c)
{
    const unsigned r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
    return 0xFF000000u | ((r << 3 | r >> 2) << 16) | ((g << 3 | g >> 2) << 8) | (b << 3 | b >> 2);
}

/* Nintendo LZ10 into out; 0 on a malformed stream (bounds checked both sides) */
bool pr_lz10(const unsigned char *s, size_t n, std::vector<uint8_t> &out)
{
    if (!s || n < 4 || s[0] != 0x10) return false;
    const size_t want = (size_t)s[1] | (size_t)s[2] << 8 | (size_t)s[3] << 16;
    if (want == 0 || want > (1u << 20)) return false;
    out.clear(); out.reserve(want);
    size_t i = 4;
    while (out.size() < want) {
        if (i >= n) return false;
        const unsigned f = s[i++];
        for (int k = 0; k < 8 && out.size() < want; ++k) {
            if (f & (0x80 >> k)) {
                if (i + 1 >= n) return false;
                const unsigned x = (unsigned)s[i] << 8 | s[i + 1];
                i += 2;
                const size_t len = (x >> 12) + 3, d = (x & 0xFFF) + 1;
                if (d > out.size()) return false;
                for (size_t j = 0; j < len && out.size() < want; ++j) out.push_back(out[out.size() - d]);
            } else {
                if (i >= n) return false;
                out.push_back(s[i++]);
            }
        }
    }
    return true;
}

/* the owner language's archive prefix, as LoadGraphics2D / InitResources pick it: 2 FRA, 3 GER, 4 ITA, 5 SPA, else ENG */
unsigned pr_lang_arc(void)
{
    switch (GetOwnerLanguage()) {
    case 2: return 0xa400;
    case 3: return 0xa800;
    case 4: return 0xac00;
    case 5: return 0xb000;
    default: return 0xa000;
    }
}

/* d_2d_cmn_icon_<lang>_ncg's FAT file id (data/2D_cad/EUR/<LANG>/; the asset catalog's numbering, as hal/head_icons.cpp reads its files) */
unsigned pr_cmn_icon_fat(void)
{
    switch (GetOwnerLanguage()) {
    case 2: return 188;
    case 3: return 190;
    case 4: return 192;
    case 5: return 194;
    default: return 187;
    }
}

bool pr_load_steps(void)
{
    const unsigned arc = pr_lang_arc();
    unsigned n = 0;
    const unsigned char *p = port_fs_archive_member(arc | 0x0d, &n);
    if (!pr_lz10(p, n, g_pr.bgch) || g_pr.bgch.size() < 96 * 32) { fprintf(stderr, "plainskin: pause tiles 0x%x missing\n", arc | 0x0d); return false; }
    p = port_fs_archive_member(0x9807, &n);
    if (!p || n < 512) { fprintf(stderr, "plainskin: pause palette 0x9807 missing\n"); return false; }
    for (int i = 0; i < 256; ++i) g_pr.bgpal[i] = pr_rgb((unsigned)p[i * 2] | (unsigned)p[i * 2 + 1] << 8);
    p = port_fs_archive_member(arc | 0x0a, &n);
    if (!p || n < 288) { fprintf(stderr, "plainskin: icon palette 0x%x missing\n", arc | 0x0a); return false; }
    for (int i = 0; i < 144; ++i) g_pr.objpal[i] = pr_rgb((unsigned)p[i * 2] | (unsigned)p[i * 2 + 1] << 8);
    /* a catalog path is relative to <asset root>/extracted/dsd/files, where hal/fs.cpp reads (as hal/menu_online_profile.cpp bust_read) */
    const char *path = port_fs_catalog_path(pr_cmn_icon_fat()), *root = getenv("SM64DS_ASSET_ROOT");
    char full[1200];
    if (path && root && *root) { snprintf(full, sizeof full, "%s/extracted/dsd/files/%s", root, path); path = full; }
    FILE *f = path ? fopen(path, "rb") : 0;
    if (!f) { fprintf(stderr, "plainskin: icon tiles (file %u) missing\n", pr_cmn_icon_fat()); return false; }
    std::vector<unsigned char> raw;
    unsigned char buf[4096];
    size_t got;
    while ((got = fread(buf, 1, sizeof buf, f)) > 0 && raw.size() < (1u << 20)) raw.insert(raw.end(), buf, buf + got);
    fclose(f);
    if (!pr_lz10(raw.data(), raw.size(), g_pr.objch) || g_pr.objch.size() < 256 * 32) { fprintf(stderr, "plainskin: icon tiles unreadable\n"); return false; }
    return true;
}

/* the art, read once (any thread; the first caller does the work under the lock). false keeps the code-drawn skin; the reason is one line */
bool pr_load(void)
{
    std::lock_guard<std::mutex> lk(g_pr_mx);
    if (!g_pr.state) g_pr.state = pr_load_steps() ? 1 : -1;
    return g_pr.state > 0;
}

/* one 8 x 8 BG tile's colour index at (x, y), 0 = transparent */
inline int pr_bgpix(int tile, int x, int y)
{
    const size_t o = (size_t)tile * 32 + (size_t)y * 4 + (size_t)(x >> 1);
    if (o >= g_pr.bgch.size()) return 0;
    const int v = g_pr.bgch[o];
    return (x & 1) ? v >> 4 : v & 15;
}

/* a whole number of host pixels per DS pixel for the plain pieces: the scale of the ROM's own plates on the pause page of this picture
   (menu_pause_geo.h pause_geo with the ROM's plates: 240 DS pixels over pw host pixels), rounded to a whole number, at least 1 */
int pr_ds_px(int w, int h)
{
    if (w < 1 || h < 1) return 1;
    PauseFacts f;
    memset(&f, 0, sizeof f);
    f.rom_art = 1;
    const PauseGeo g = pause_geo(w, h, &f, 3);
    const int k = (int)(g.pw / 240.0 + 0.5);
    return k < 1 ? 1 : k;
}

/* the frame's tile section for one DS row / column: n DS pixels across, `lo` fixed pixels at the start and `hi` at the end, the rest the
   repeated middle tile. Where n is under lo + hi (a piece smaller than the ROM's corners) the start and the end each keep their outer part
   and the inner part of both is left out, so the outline is still the ROM's own and nothing is stretched. Returns the section (0 start,
   1 middle, 2 end) and the pixel inside that section in *at. */
inline int pr_section(int i, int n, int lo, int hi, int *at)
{
    if (n >= lo + hi) {
        if (i < lo) { *at = i; return 0; }
        if (i >= n - hi) { *at = i - (n - hi); return 2; }
        *at = (i - lo) % 8; return 1;
    }
    const int keep_lo = n * lo / (lo + hi);
    if (i < keep_lo) { *at = i; return 0; }
    *at = hi - (n - i); return 2;
}

/* frame fk in palette bank `bank` into piece p (its whole w x h) at k host pixels per DS pixel. The frame is (w / k) x (h / k) DS pixels,
   centred; a remainder under k host pixels stays clear at the edges. false when the art is missing or the piece is under 4 DS pixels. */
bool pr_frame_fill(Piece *p, int fk, int bank, int k)
{
    static const int kCols[PR_FRAMES][3] = { { 1, 2, 3 }, { 11, 12, 13 }, { 29, 30, 31 } };
    /* the tile offsets of each frame's tile rows, top to bottom (added to kCols); the row repeated over the inside is kMid */
    static const int kRows[PR_FRAMES][5] = { { 0, 32, 64, 71, -1 }, { 0, 32, -3, 29, 61 }, { 0, 32, 64, -1, -1 } };
    static const int kNRows[PR_FRAMES] = { 4, 5, 3 }, kMid[PR_FRAMES] = { 1, 2, 1 };
    /* the clear rows the ROM's tiles carry: the field starts one pixel down its top tile row; the button and the field end two pixels into
       their last tile row (the black foot line), the rest of that row is clear. Measured off the ROM's own maps. */
    static const int kTopSkip[PR_FRAMES] = { 0, 1, 0 }, kFootKeep[PR_FRAMES] = { 2, 2, 8 };
    if (!p || fk < 0 || fk >= PR_FRAMES || k < 1 || !pr_load()) return false;
    const int dw = p->w, dh = p->h, W = dw / k, H = dh / k, nr = kNRows[fk], mid = kMid[fk];
    if (W < 4 || H < 4) return false;
    const int top_h = mid * 8 - kTopSkip[fk], bot_h = (nr - 1 - mid) * 8 - (8 - kFootKeep[fk]);
    const int ox = (dw - W * k) / 2, oy = (dh - H * k) / 2;
    p->px.assign(p->px.size(), 0u);
    for (int y = 0; y < H; ++y) {
        int ty;
        const int sec = pr_section(y, H, top_h, bot_h, &ty);
        if (sec == 0) ty += kTopSkip[fk];
        const int row = sec == 0 ? ty / 8 : (sec == 1 ? mid : mid + 1 + ty / 8);
        ty &= 7;
        for (int x = 0; x < W; ++x) {
            int tx;
            const int col = pr_section(x, W, 8, 8, &tx);
            const int v = pr_bgpix(kCols[fk][col] + kRows[fk][row], tx & 7, ty);
            if (!v) continue;
            const uint32_t c = g_pr.bgpal[(bank & 15) * 16 + v];
            for (int b = 0; b < k; ++b) {
                uint32_t *d = &p->px[(size_t)(oy + y * k + b) * dw + ox + x * k];
                for (int a = 0; a < k; ++a) d[a] = c;
            }
        }
    }
    return true;
}

/* the same as a cached piece of its own (kind 20 + fk), or null */
const Piece *pr_frame(int fk, int bank, int dw, int dh, int k)
{
    if (fk < 0 || fk >= PR_FRAMES || dw < 1 || dh < 1 || k < 1 || !pr_load()) return 0;
    Piece *p = find_piece(20 + fk, bank | k << 8, dw, dh);
    if (p) return p;
    if (dw / k < 4 || dh / k < 4) return 0;
    p = new_piece(20 + fk, bank | k << 8, dw, dh, dw, dh);
    pr_frame_fill(p, fk, bank, k);
    return p;
}

/* the DS pixel size the plain pieces are drawn at on this thread's picture (set by each draw entry from pr_ds_px; 1 before any) */
thread_local int tl_pr_k = 1;

/* sprite s (16 x 16 DS pixels) scaled by a whole number to fit dw x dh, centred; cached. Never under 1:1: in a box smaller than 16 the piece
   is 16 wide / tall and the caller centres it on the box (it stands a few pixels past it). */
const Piece *pr_sprite(int s, int dw, int dh)
{
    if (dw < 16) dw = 16;
    if (dh < 16) dh = 16;
    static const int kTile[PR_SPRITES] = { 158, 220, 154, 150 }, kBank[PR_SPRITES] = { 6, 8, 0, 0 };
    if (s < 0 || s >= PR_SPRITES || dw < 1 || dh < 1) return 0;
    if (!pr_load()) return 0;
    Piece *p = find_piece(30 + s, 0, dw, dh);
    if (p) return p;
    int k = (dw < dh ? dw : dh) / 16;
    if (k < 1) k = 1;
    p = new_piece(30 + s, 0, dw, dh, dw, dh);
    const int ox = (dw - 16 * k) / 2, oy = (dh - 16 * k) / 2;
    for (int y = 0; y < 16; ++y)
        for (int x = 0; x < 16; ++x) {
            const size_t o = (size_t)(kTile[s] + (y >> 3) * 32 + (x >> 3)) * 32 + (size_t)(y & 7) * 4 + (size_t)((x & 7) >> 1);
            if (o >= g_pr.objch.size()) continue;
            const int v = (x & 1) ? g_pr.objch[o] >> 4 : g_pr.objch[o] & 15;
            if (!v) continue;
            const uint32_t c = g_pr.objpal[kBank[s] * 16 + v];
            for (int b = 0; b < k; ++b) {
                const int yy = oy + y * k + b;
                if (yy < 0 || yy >= dh) continue;
                for (int a = 0; a < k; ++a) {
                    const int xx = ox + x * k + a;
                    if (xx >= 0 && xx < dw) p->px[(size_t)yy * dw + xx] = c;
                }
            }
        }
    return p;
}
