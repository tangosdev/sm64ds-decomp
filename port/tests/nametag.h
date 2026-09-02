/* ---- THE VS NAME TAGS: who that body is, and how they are doing ------------
 *
 * WHAT IT DRAWS. In a VS match, over every REMOTE player's head, two centred
 * lines: his carried-star count on top and the lobby nickname the room gave
 * that seat under it. Never over the local player -- his own body is the one
 * body on screen nobody needs told apart.
 *
 * WHY IT IS HERE AND NOT IN THE GAME. The cartridge's VS mode was four people
 * in one room who could look up from their own screen; the port's is strangers
 * on a wire, and the ROM has nothing to say about which body is which. So this
 * is a HOST OVERLAY, drawn where every other one in this file is drawn -- into
 * the composed framebuffer after the raster owns the frame -- and it READS game
 * state and WRITES none. That is the property the lockstep proofs depend on: an
 * overlay that touched an actor, a pad word or the heap would show up as a
 * divergence, and this one cannot, because every access below is a load.
 *
 * ---- THE GLYPHS ARE THE CARTRIDGE'S ---------------------------------------
 *
 * NOT overlay_font.h. That 5x7 ASCII font is the debug overlay's, and a tag
 * over a player's head in the shipped game reading in a different typeface from
 * every other word the game says would be the port's voice where the game's
 * belongs. So the tag is drawn with the ROM's own message font, and the whole
 * decode is read off matched src rather than guessed:
 *
 *   WHERE THE TILES ARE.  LoadFont (src/LoadFont.cpp) and LoadFont3D
 *     (src/LoadFont3D.c) both MultiCopy_Int file 0x980e -- 0x4000 bytes -- to
 *     func_02054d88(), which is engine A's BG character base in VRAM. VS mode
 *     takes the LoadFont3D arm (Stage::LoadGraphics2D(b=1),
 *     src/_ZN5Stage14LoadGraphics2DEbi.cpp:75). The port maps DS VRAM at its
 *     real address (ntr/mmio.h VRAM_BASE 0x06000000), so the font the game is
 *     itself reading is readable here, at the same address, with no second
 *     copy and no file load of our own.
 *
 *   WHERE A GLYPH IS INSIDE THEM.  Message::AddChar
 *     (src/_ZN7Message7AddCharEc.cpp) is the authority, and it is exact:
 *
 *         ip = font_base + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5)
 *         for (i = 0; i < 0x10; i++) { word = *ip++; if (i == 7) ip += 0xf8; }
 *
 *     Sixteen words is 64 bytes is two 4bpp 8x8 tiles, and the ip += 0xf8
 *     after the eighth word steps 992 bytes on top of the 32 already walked --
 *     1024 bytes, exactly 32 tiles. So the sheet is 32 TILES WIDE and a glyph
 *     is 8 wide by 16 tall: the top tile at the computed offset and the bottom
 *     tile one tile-row (0x400 bytes) below it. The (c & 0xe0) << 1 term is
 *     that same 32-wide row pitch written for the character code.
 *
 *   HOW WIDE A GLYPH IS.  data_0208f074[c], the ROM's per-code advance table
 *     (the font is proportional). AddChar reads it, func_0201b6f8 and
 *     func_02031028 measure lines with it, and the port already emits it as
 *     real ROM bytes (port/tools/romdata.py's NAMED list). Nothing here
 *     invents a width.
 *
 *   WHAT A CHARACTER CODE IS.  The message stream is not ASCII. The ranges are
 *     hal/message_boot.cpp's, derived there off AddChar for the message dumper
 *     and reused verbatim: digits 0x00..0x09, A..Z 0x0a..0x23, a..z
 *     0x2d..0x46, space 0x4d. THE FONT HAS LOWERCASE, so a nickname renders as
 *     the player typed it rather than shouted.
 *
 *   WHAT IS MISSING, SAID OUT LOUD.  Those four ranges are the only ones any
 *     matched TU in this tree names. SM64DS_VS_NAMES allows 0x20..0x7E, so a
 *     nickname with punctuation in it has characters this decode cannot place,
 *     and they are drawn as SPACES rather than as a guessed glyph. Guessing
 *     would put an arbitrary symbol in a stranger's name. The day somebody
 *     maps 0x24..0x2c and 0x47..0x4c off the font sheet itself, nt_code_for is
 *     the one function that changes.
 *
 *   THE COLOURS ARE NOT THE ROM'S, and that is the one deliberate deviation.
 *     A 4bpp glyph's non-zero indices are the font palette's, which for a tag
 *     floating over the open world would be unreadable against half the sky in
 *     the game. So every inked pixel is drawn in the tag's own colour with a
 *     black outline behind it, which is what makes it legible over water, sky
 *     and stone alike. The SHAPES are the cartridge's to the pixel; the ink is
 *     the port's, exactly as the winner banner's is.
 *
 * ---- WHERE ON THE SCREEN --------------------------------------------------
 *
 * There is no world-to-screen helper in this port, so this file has one, and
 * it is the same arithmetic the renderer does rather than a second opinion:
 *
 *   view    hal/particle_bridges.cpp already writes it out for the fx probe,
 *           and it is Model::Render's compose -- v' = p * R + T over
 *           data_0209b3ec, the Matrix4x3 View::Render parks through
 *           CopyToViewMat (src/_Z13CopyToViewMatPK9Matrix4x3.c). Its
 *           translation row is in SCENE units because Camera::Render feeds
 *           LookAt_ its eye and look-at as (v + 4) >> 3, so the actor's Fix12
 *           WORLD position crosses that same divide first.
 *   proj    ntr::gx_debug_proj, and ntr/gx.cpp's project() is the convention:
 *           mul(mul(v, pos), proj) with row vectors.
 *   screen  ntr/gx.cpp's to_screen, copied term for term, viewport included.
 *
 * BOTH MATRICES ARE STASHED, not read at draw time, and that is not a
 * micro-optimisation: by the time the overlay runs, the frame has drawn its 2D
 * layers and whatever projection they left behind is the live one.
 * nt_stash_view is called from the frame loop at the point where the camera has
 * been pushed and nothing else has touched the matrices yet, which is true on
 * both the real Camera actor's path and the old hand-tuned rig's (there
 * data_0209b3ec is the identity walk_window seeds it with and the view is
 * folded into the projection push_camera makes, so one stash covers both by
 * construction).
 *
 * Behind the camera and off the screen are both culled. The tag steps up one
 * size when the player is close, which is the whole of the distance treatment:
 * a continuous scale shimmers as it rounds to whole pixels. The head anchor
 * over the actor's feet is measured off the game's own camera rather than
 * chosen; NT_HEAD_UP carries the measurement.
 *
 * ---- WHICH SLOTS -----------------------------------------------------------
 *
 * data_0209f2d8 == 1 (a VS match), i != data_0209f250 (not me),
 * data_0209f394[i] non-null (a Player exists) and data_0209fc5c[i] non-zero.
 * The last one is Player::Render's OWN guard (src/_ZN6Player6RenderEv.cpp:46
 * returns early on it), so a tag exists exactly when the body under it does.
 *
 * The name and the star count come out of hal/star_flow.cpp through
 * port_vs_slot_name and port_vs_slot_stars rather than being read here, so the
 * tag and the end-of-match banner cannot disagree about either -- including
 * about the lobby's known slot-order caveat, which that file states.
 */
#ifndef PORT_TESTS_NAMETAG_H
#define PORT_TESTS_NAMETAG_H

extern "C" {
/* the settings key and its env override (hal/host_settings.cpp) */
int host_setting_name_tags(void);
/* the two facts, from hal/star_flow.cpp */
const char *port_vs_slot_name(int slot);
int port_vs_slot_stars(int slot);
/* engine A's BG character base: where LoadFont/LoadFont3D put file 0x980e */
int func_02054d88(void);
/* the ROM's per-character advance table (romdata.py NAMED) */
extern unsigned char data_0208f074[];
/* per-slot "this slot is live", Player::Render's own guard */
extern unsigned char data_0209fc5c[];
}

/* ---- the glyph atlas ------------------------------------------------------
   Decoded ONCE, out of the font the game itself is reading, and kept. One
   entry per printable ASCII code; have is 0 for a code the message encoding
   has no place for (see nt_code_for). */
enum { NT_GW = 8, NT_GH = 16, NT_FIRST = 0x20, NT_LAST = 0x7e };

struct NtGlyph {
    unsigned char ink[NT_GH][NT_GW];   /* the 4bpp index, 0 = transparent */
    unsigned char adv;                 /* data_0208f074's advance, in pixels */
    unsigned char have;
};

static NtGlyph g_nt_glyph[NT_LAST - NT_FIRST + 1];
static int g_nt_font_ready;
static int g_nt_font_said;

/* ASCII to the message stream's character code, hal/message_boot.cpp's ranges.
   -1 for a code this encoding has no glyph for; the caller draws a space. */
static int nt_code_for(unsigned char a)
{
    if (a >= '0' && a <= '9') return a - '0';            /* 0x00..0x09 */
    if (a >= 'A' && a <= 'Z') return 0x0a + (a - 'A');   /* 0x0a..0x23 */
    if (a >= 'a' && a <= 'z') return 0x2d + (a - 'a');   /* 0x2d..0x46 */
    if (a == ' ') return 0x4d;
    return -1;
}

/* Build the atlas if the font is up. Returns 1 once it is built. Cheap to call
   every frame: one compare after the first success, and one VRAM probe before
   it (the font is not in VRAM until the scene's LoadGraphics2D has run). */
static int nt_font_build(void)
{
    if (g_nt_font_ready) return 1;

    const unsigned base = (unsigned)func_02054d88();
    if (!base) return 0;                 /* no BG char base in this mode */

    /* THE PROBE IS THE LETTER A, not the base pointer. func_02054d88 answers an
       address whenever the display is in a mode that has one, whether or not
       anybody has loaded a font into it, so a build that trusted the pointer
       would cache 96 blank glyphs on the first frame and never look again. */
    {
        const unsigned off =
            (unsigned)(((0x0a & 0x1f) + ((0x0a & 0xe0) << 1)) << 5);
        const unsigned char *t = (const unsigned char *)(base + off);
        int any = 0;
        for (int i = 0; i < 32 && !any; ++i) any = t[i] != 0;
        if (!any) return 0;
    }

    for (int a = NT_FIRST; a <= NT_LAST; ++a) {
        NtGlyph &g = g_nt_glyph[a - NT_FIRST];
        g.have = 0;
        g.adv = 6;
        const int c = nt_code_for((unsigned char)a);
        if (c < 0) continue;

        const unsigned off =
            (unsigned)(((c & 0x1f) + ((c & 0xe0) << 1)) << 5);
        const unsigned char *top = (const unsigned char *)(base + off);
        const unsigned char *bot = top + 0x400;   /* one 32-tile row down */
        for (int r = 0; r < NT_GH; ++r) {
            const unsigned char *t = (r < 8 ? top : bot) + (r & 7) * 4;
            for (int x = 0; x < NT_GW; ++x) {
                const unsigned char b = t[x >> 1];
                g.ink[r][x] = (unsigned char)((x & 1) ? (b >> 4) : (b & 0x0f));
            }
        }
        {
            const unsigned w = data_0208f074[c];
            g.adv = (unsigned char)((w >= 1 && w <= 16) ? w : 6);
        }
        g.have = 1;
    }
    g_nt_font_ready = 1;
    if (!g_nt_font_said) {
        g_nt_font_said = 1;
        fprintf(stderr, "[tag] the ROM's message font is decoded from VRAM "
                "%08x: 8x16 glyphs, 32-tile rows, widths from data_0208f074\n",
                base);
    }
    return 1;
}

/* ---- one line of ROM-font text --------------------------------------------
   pass 0 paints the black outline (every inked pixel's eight neighbours),
   pass 1 paints the ink. The caller runs the whole line through pass 0 before
   pass 1 so a glyph's outline cannot land on its neighbour's ink. measure
   returns the width and paints nothing. */
static int nt_line(const OvlSurface &fb, int x0, int y0, const char *s,
                   int scale, uint32_t rgb, int pass, int measure)
{
    int x = x0;
    for (; *s; ++s) {
        const unsigned char a = (unsigned char)*s;
        if (a < NT_FIRST || a > NT_LAST) continue;
        const NtGlyph &g = g_nt_glyph[a - NT_FIRST];
        const int adv = (int)g.adv * scale;
        if (measure || !g.have) { x += adv; continue; }
        for (int r = 0; r < NT_GH; ++r)
            for (int c = 0; c < NT_GW; ++c) {
                if (!g.ink[r][c]) continue;
                for (int sy = 0; sy < scale; ++sy)
                    for (int sx = 0; sx < scale; ++sx) {
                        const int fx = x + c * scale + sx;
                        const int fy = y0 + r * scale + sy;
                        for (int oy = -1; oy <= 1; ++oy)
                            for (int ox = -1; ox <= 1; ++ox) {
                                if (pass == 1 && (ox || oy)) continue;
                                if (pass == 0 && !ox && !oy) continue;
                                const int qx = fx + ox * scale;
                                const int qy = fy + oy * scale;
                                if (qx < 0 || qx >= ntr::SCREEN_W ||
                                    qy < 0 || qy >= ntr::SCREEN_H)
                                    continue;
                                fb.px[(size_t)qy * (size_t)fb.stride + qx] =
                                    pass ? rgb : 0xFF000000u;
                            }
                    }
            }
        x += adv;
    }
    return x - x0;
}

static void nt_text_centred(const OvlSurface &fb, int cx, int y, const char *s,
                            int scale, uint32_t rgb)
{
    const int w = nt_line(fb, 0, 0, s, scale, rgb, 1, 1);
    const int x = cx - w / 2;
    nt_line(fb, x, y, s, scale, rgb, 0, 0);
    nt_line(fb, x, y, s, scale, rgb, 1, 0);
}

/* ---- the stashed frame transform ------------------------------------------
   Taken where the camera has just been pushed. See the banner. */
static float g_nt_proj[16];
static int g_nt_view[12];
static int g_nt_vp[4] = {0, 0, ntr::SCREEN_W, ntr::SCREEN_H};
static int g_nt_have_view;

static void nt_stash_view(void)
{
    ntr::gx_debug_proj(g_nt_proj);
    for (int i = 0; i < 12; ++i) g_nt_view[i] = data_0209b3ec[i];
    int vx = 0, vy = 0, vw = 0, vh = 0, sets = 0;
    ntr::gx_debug_viewport(vx, vy, vw, vh, sets);
    if (sets && vw > 0 && vh > 0) {
        g_nt_vp[0] = vx; g_nt_vp[1] = vy; g_nt_vp[2] = vw; g_nt_vp[3] = vh;
    } else {
        g_nt_vp[0] = 0; g_nt_vp[1] = 0;
        g_nt_vp[2] = ntr::SCREEN_W; g_nt_vp[3] = ntr::SCREEN_H;
    }
    g_nt_have_view = 1;
}

/* Fix12 WORLD position -> framebuffer pixel. 0 when it is behind the camera or
   the frame has no stashed transform yet. depth comes back as the clip w, in
   scene units, which is what the size step reads. */
static int nt_project(const int wp[3], float *sx, float *sy, float *depth)
{
    if (!g_nt_have_view) return 0;
    const int *m = g_nt_view;
    /* the ROM's own world -> scene conversion (Camera::Render's (v + 4) >> 3),
       then out of Fix12 */
    const double px = (double)((wp[0] + 4) >> 3) / 4096.0;
    const double py = (double)((wp[1] + 4) >> 3) / 4096.0;
    const double pz = (double)((wp[2] + 4) >> 3) / 4096.0;

    const double vx = (px * m[0] + py * m[3] + pz * m[6]) / 4096.0
                      + m[9] / 4096.0;
    const double vy = (px * m[1] + py * m[4] + pz * m[7]) / 4096.0
                      + m[10] / 4096.0;
    const double vz = (px * m[2] + py * m[5] + pz * m[8]) / 4096.0
                      + m[11] / 4096.0;

    const float *P = g_nt_proj;
    const double cx = vx * P[0] + vy * P[4] + vz * P[8] + P[12];
    const double cy = vx * P[1] + vy * P[5] + vz * P[9] + P[13];
    const double cw = vx * P[3] + vy * P[7] + vz * P[11] + P[15];
    if (cw <= 1e-4) return 0;                 /* behind the camera, or on it */

    *sx = (float)((cx / cw + 1.0) * 0.5 * g_nt_vp[2] + g_nt_vp[0]);
    *sy = (float)((1.0 - (cy / cw + 1.0) * 0.5) * g_nt_vp[3] + g_nt_vp[1]);
    *depth = (float)cw;
    return 1;
}

/* THE HEAD, MEASURED RATHER THAN GUESSED, and it is the only chosen number in
   this file.

   The actor's position is at his feet. What sits above them is not written
   down anywhere in src/, so it was read off the GAME'S OWN CAMERA in a live
   four-window arena (SM64DS_TAG_PROBE=2, whose ruler line prints exactly this
   measurement): the player's feet at world y 254.0, and Camera::Render's
   look-at (cam+0x80) at world y 415.2. The ROM films the player from 161 world
   units above his feet, which is that camera's idea of his head.

   So the anchor is 200 -- a little clear of the ROM's own head height, and the
   name's bottom row lands just above it. The first pass used 320 and the same
   ruler is what caught it: at the distance a four-window arena films from, 320
   lifted the tag 164 px on a 384-row screen, a whole second body above him. */
enum { NT_HEAD_UP = 200 * 4096 };

/* SM64DS_TAG_PROBE=1 prints one line per tag on the frames it draws, which is
   what turns "no tag appeared" into "the slot was skipped" or "it projected
   off screen". =2 adds the RULER: the same body's FEET projected beside its
   head, which is how NT_HEAD_UP above was sized instead of guessed -- the
   pixel gap between the two is what a chosen world-unit offset is actually
   worth on screen at the distance the game films from. Off unless asked. */
static int nt_probe(void)
{
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("SM64DS_TAG_PROBE");
        on = e ? (e[0] == '2' ? 2 : 1) : 0;
    }
    return on;
}

/* ---- the tags -------------------------------------------------------------
   Called once per frame, with the surface every other host overlay draws on.
   Reads only. */
static void nt_draw(const OvlSurface &fb)
{
    if (data_0209f2d8 != 1) return;              /* not a VS match */
    if (!host_setting_name_tags()) return;
    if (!nt_font_build()) return;

    const int me = (int)data_0209f250;
    for (int i = 0; i < kPortMaxPlayers; ++i) {
        if (i == me) continue;                   /* never over the local body */
        const char *p = (const char *)data_0209f394[i];
        if (!p) continue;
        if (data_0209fc5c[i] == 0) continue;     /* Player::Render's own guard */

        const int wp[3] = {*(const int *)(p + 0x5c),
                           *(const int *)(p + 0x60) + NT_HEAD_UP,
                           *(const int *)(p + 0x64)};
        float sx = 0, sy = 0, d = 0;
        if (!nt_project(wp, &sx, &sy, &d)) continue;

        /* THE WHOLE DISTANCE TREATMENT, and it is two tiers on purpose: a
           continuous scale shimmers as it rounds to whole pixels. The
           threshold is in the clip w the projection hands back, which is scene
           units, and 200 is read off the measured range -- a four-window arena
           films its other players at w 100..130, so an arena tag is the
           readable size and a body across a course shrinks. */
        const int scale = OVL_SCALE * (d < 200.0f ? 2 : 1);
        const int lh = NT_GH * scale + 2 * scale;
        const int cx = (int)(sx + 0.5f);
        const int name_y = (int)(sy + 0.5f) - lh;
        const int star_y = name_y - lh;
        /* off the screen entirely, with a whole tag's margin so a name that is
           half on does not flicker at the edge */
        if (cx < -256 || cx > ntr::SCREEN_W + 256) continue;
        if (star_y < -2 * lh || star_y > ntr::SCREEN_H) continue;

        char who[24];
        const char *nick = port_vs_slot_name(i);
        if (nick && nick[0])
            snprintf(who, sizeof who, "%s", nick);
        else
            snprintf(who, sizeof who, "PLAYER %d", i + 1);

        const int stars = port_vs_slot_stars(i);
        char sline[24];
        snprintf(sline, sizeof sline, "%d STAR%s", stars,
                 stars == 1 ? "" : "S");

        nt_text_centred(fb, cx, star_y, sline, scale, 0xFFFFE060u);
        nt_text_centred(fb, cx, name_y, who, scale, 0xFFFFFFFFu);

        if (nt_probe()) {
            fprintf(stderr, "[tag] slot %d \"%s\" %s at (%.0f,%.0f) w=%.2f "
                    "scale %d\n", i, who, sline, sx, sy, d, scale);
            if (nt_probe() >= 2) {
                const int fp[3] = {*(const int *)(p + 0x5c),
                                   *(const int *)(p + 0x60),
                                   *(const int *)(p + 0x64)};
                float fx = 0, fy = 0, fd = 0;
                if (nt_project(fp, &fx, &fy, &fd))
                    fprintf(stderr, "[tag]   ruler: feet y=%.1f world at screen"
                            " y %.0f, so +%d world lifts the anchor %.0f px\n",
                            fp[1] / 4096.0, fy, (int)(NT_HEAD_UP / 4096),
                            fy - sy);
            }
        }
    }
}

#endif /* PORT_TESTS_NAMETAG_H */
