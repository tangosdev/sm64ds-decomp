/* Host player settings: the game's half of settings.json. See
   port/hal/host_settings.h for what the file is and why it is not an
   environment variable.

   The parser here is deliberately small. settings.json is a flat object of
   scalars written by System.Text.Json, and the only thing the game needs
   out of it is a handful of values; pulling in a JSON library to read three
   booleans would be the wrong trade. What it does have to get right is the
   failure mode: anything it cannot make sense of has to land on the
   defaults silently, because the file sits in a folder players edit by
   hand and a half-saved one must not stop the game from starting. */

#include "hal/host_settings.h"

/* run mg16 lane MP2: the instance suffix on the sibling temp file. */
#include "instance_tag.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace {

/* settings.json is a small hand-editable file. Anything bigger than this is
   not the launcher's, so it is treated as malformed. */
const long SETTINGS_MAX = 64 * 1024;

/* ASCII case-insensitive compare of exactly n bytes. The launcher
   deserializes with PropertyNameCaseInsensitive, so the game matches keys
   the same way and a hand-typed "swapcameraturndirection" still works. */
int ieq(const char *a, const char *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        char x = a[i], y = b[i];
        if (x >= 'A' && x <= 'Z') x = (char)(x - 'A' + 'a');
        if (y >= 'A' && y <= 'Z') y = (char)(y - 'A' + 'a');
        if (x != y) return 0;
    }
    return 1;
}

/* Whole file into a NUL-terminated buffer the caller frees. Null on any
   failure at all, which every caller reads as "use the defaults". */
char *slurp(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
    long n = ftell(f);
    if (n < 0 || n > SETTINGS_MAX) { fclose(f); return 0; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return 0; }
    char *buf = (char *)malloc((size_t)n + 1);
    if (!buf) { fclose(f); return 0; }
    size_t got = fread(buf, 1, (size_t)n, f);
    fclose(f);
    buf[got] = '\0';
    return buf;
}

/* Cheap malformed test: the launcher writes one JSON object, so the first
   and last non-space characters have to be the braces. This is what turns a
   truncated write or a half-finished hand edit into "use the defaults"
   rather than into a half-read setting. It is not a JSON validator and is
   not trying to be. A file that passes this and is still broken simply will
   not yield a key to match, which lands on the defaults as well. */
int looks_like_json_object(const char *s)
{
    const char *a = s;
    while (*a == ' ' || *a == '\t' || *a == '\r' || *a == '\n') ++a;
    /* System.Text.Json writes UTF-8; a BOM is legal in the wild. */
    if ((unsigned char)a[0] == 0xEF && (unsigned char)a[1] == 0xBB &&
        (unsigned char)a[2] == 0xBF) {
        a += 3;
        while (*a == ' ' || *a == '\t' || *a == '\r' || *a == '\n') ++a;
    }
    if (*a != '{') return 0;
    const char *b = a + strlen(a);
    while (b > a && (b[-1] == ' ' || b[-1] == '\t' || b[-1] == '\r' ||
                     b[-1] == '\n'))
        --b;
    return b > a && b[-1] == '}';
}

/* Find `key` used as an object KEY and return a pointer at the first
   character of its value, or null. Every reader below is built on this, so
   there is one notion of "used as an object KEY" in this file rather than
   one per type.

   Strings that are values rather than keys are stepped over, so a credit
   name that happens to contain the key's spelling cannot be mistaken for
   the key itself. */
const char *json_value(const char *s, const char *key)
{
    const size_t klen = strlen(key);
    for (const char *p = s; *p; ++p) {
        if (*p != '"') continue;
        const char *name = p + 1;
        const char *end = name;
        /* a backslash pair is stepped over and the SAME string goes on,
           the idiom json_value_end uses: stopping at the backslash used to
           leave the scanner inside the string, so every key after a value
           holding one read as absent (review of lane PADCAL) */
        while (*end && *end != '"') end += (*end == '\\' && end[1]) ? 2 : 1;
        if (*end != '"') break;
        const int is_key = (size_t)(end - name) == klen && ieq(name, key, klen);
        const char *q = end + 1;
        while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
        if (*q != ':') { p = end; continue; }
        ++q;
        while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
        if (is_key) return q;
        p = q - 1;
    }
    return 0;
}

/* One past the last character of the value token starting at `v`. A quoted
   string ends at its closing quote; anything else ends at the first
   separator. Only ever asked about the scalars this file writes, so it does
   not need to walk a nested object or array. */
const char *json_value_end(const char *v)
{
    if (*v == '"') {
        const char *p = v + 1;
        while (*p && *p != '"') p += (*p == '\\' && p[1]) ? 2 : 1;
        return *p ? p + 1 : p;
    }
    /* an array or object ends at its matching bracket, strings stepped over,
       so PadLayouts (the one non-scalar this file writes) can be replaced
       whole by json_set. Unbalanced text ends at the end of the string. */
    if (*v == '[' || *v == '{') {
        int depth = 0;
        const char *p = v;
        while (*p) {
            if (*p == '"') {
                ++p;
                while (*p && *p != '"') p += (*p == '\\' && p[1]) ? 2 : 1;
                if (!*p) break;
            } else if (*p == '[' || *p == '{') {
                ++depth;
            } else if (*p == ']' || *p == '}') {
                if (--depth == 0) return p + 1;
            }
            ++p;
        }
        return p;
    }
    {
        const char *p = v;
        while (*p && *p != ',' && *p != '}' && *p != ']' && *p != ' ' &&
               *p != '\t' && *p != '\r' && *p != '\n')
            ++p;
        return p;
    }
}

/* The JSON true/false after `key`. Returns dflt unless a real boolean is
   there, so a key present but half-edited reads as absent. */
int json_bool(const char *s, const char *key, int dflt)
{
    const char *v = json_value(s, key);
    if (!v) return dflt;
    /* length before the compare: ieq reads a fixed count, so a value shorter
       than the literal must never reach it */
    const size_t vlen = strlen(v);
    if (vlen >= 4 && ieq(v, "true", 4)) return 1;
    if (vlen >= 5 && ieq(v, "false", 5)) return 0;
    return dflt;
}

/* Find `key` used as an object KEY and read a decimal integer after it.
   Returns dflt unless digits (optionally signed) are actually there, so a
   key a player half-edited lands on the default like every other reader. */
int json_int(const char *s, const char *key, int dflt)
{
    const char *v = json_value(s, key);
    if (!v) return dflt;
    if (*v == '"') ++v;              /* "16" is as good as 16 here */
    {
        const char *p = v;
        if (*p == '-' || *p == '+') ++p;
        if (*p < '0' || *p > '9') return dflt;
    }
    return (int)strtol(v, 0, 10);
}

/* Copy `key`'s value token into out (a quoted string arrives unquoted, so a
   caller comparing against "analog" does not have to know how it was
   written). 1 when a value was there and fitted, 0 otherwise. */
int json_str(const char *s, const char *key, char *out, size_t cap)
{
    const char *v = json_value(s, key);
    if (!v || !cap) return 0;
    const char *e = json_value_end(v);
    if (*v == '"') { ++v; if (e > v) --e; }
    if ((size_t)(e - v) >= cap) return 0;
    memcpy(out, v, (size_t)(e - v));
    out[e - v] = '\0';
    return 1;
}

/* Where settings.json can be. The launcher puts the launcher exe, the game
   exe and settings.json in one bundle directory and launches the game with
   both its working directory and SM64DS_ASSET_ROOT set to that directory,
   so in a player's kit all three candidates are the same folder. They are
   tried in order of how literally each one means "next to the exe", and the
   first that EXISTS wins. A file that exists but will not parse falls back
   to the defaults rather than falling through to the next candidate,
   because a player who edited a file meant that file to be the one read. */
int find_settings(char *out, size_t cap)
{
#ifdef _WIN32
    {
        char exe[MAX_PATH];
        DWORD n = GetModuleFileNameA(NULL, exe, MAX_PATH);
        if (n > 0 && n < MAX_PATH) {
            char *slash = strrchr(exe, '\\');
            char *fwd = strrchr(exe, '/');
            if (fwd && (!slash || fwd > slash)) slash = fwd;
            if (slash) {
                *slash = '\0';
                if (strlen(exe) + 16 < cap) {
                    snprintf(out, cap, "%s\\settings.json", exe);
                    FILE *f = fopen(out, "rb");
                    if (f) { fclose(f); return 1; }
                }
            }
        }
    }
#endif
    {
        const char *root = getenv("SM64DS_ASSET_ROOT");
        if (root && *root && strlen(root) + 16 < cap) {
            snprintf(out, cap, "%s/settings.json", root);
            FILE *f = fopen(out, "rb");
            if (f) { fclose(f); return 1; }
        }
    }
    {
        snprintf(out, cap, "settings.json");
        FILE *f = fopen(out, "rb");
        if (f) { fclose(f); return 1; }
    }
    return 0;
}

/* Where settings.json WOULD go when there is not one yet. Same candidate
   order as find_settings and the same answer whenever a file exists, so a
   save lands on the file a load read. Beside the exe is the bundle layout,
   so that is the one a fresh save creates. */
int settings_write_path(char *out, size_t cap)
{
    if (find_settings(out, cap)) return 1;
#ifdef _WIN32
    {
        char exe[MAX_PATH];
        DWORD n = GetModuleFileNameA(NULL, exe, MAX_PATH);
        if (n > 0 && n < MAX_PATH) {
            char *slash = strrchr(exe, '\\');
            char *fwd = strrchr(exe, '/');
            if (fwd && (!slash || fwd > slash)) slash = fwd;
            if (slash) {
                *slash = '\0';
                if (strlen(exe) + 16 < cap) {
                    snprintf(out, cap, "%s\\settings.json", exe);
                    return 1;
                }
            }
        }
    }
#endif
    {
        const char *root = getenv("SM64DS_ASSET_ROOT");
        if (root && *root && strlen(root) + 16 < cap) {
            snprintf(out, cap, "%s/settings.json", root);
            return 1;
        }
    }
    snprintf(out, cap, "settings.json");
    return 1;
}

/* A COPY of `s` with `key` set to `val` -- which is a whole JSON value token,
   quotes and all, because the caller knows whether it is writing a string or
   a number. The key's existing value is replaced where it is; a key that is
   not there yet is inserted just inside the opening brace. Null on any
   failure, which every caller reads as "do not write anything".

   Replacing rather than rewriting is the whole point. settings.json is the
   LAUNCHER's file and it owns keys this program has never heard of (volume,
   crash reports, whatever the launcher grows next). A save that serialized
   the three values this file knows about would silently drop the rest, and
   the player would find out by losing their volume. Every byte that is not
   one of the values being set comes out the far side unchanged. */
char *json_set(const char *s, const char *key, const char *val)
{
    const char *cut_a, *cut_b;
    const char *insert = 0;
    int comma = 0;
    char *out;
    size_t need;

    const char *v = json_value(s, key);
    if (v) {
        cut_a = v;
        cut_b = json_value_end(v);
    } else {
        /* just past the opening brace, which looks_like_json_object has
           already proved is there (BOM and leading space included) */
        const char *b = s;
        while (*b && *b != '{') ++b;
        if (!*b) return 0;
        cut_a = cut_b = b + 1;
        insert = key;
        /* the separator only when something FOLLOWS. An object with no
           members at all is the case that made this necessary: a trailing
           comma is not JSON, System.Text.Json refuses to read it, and the
           launcher would have lost the file on the first save it made into
           an empty one. */
        {
            const char *t = cut_b;
            while (*t == ' ' || *t == '\t' || *t == '\r' || *t == '\n') ++t;
            comma = *t != '}' && *t != '\0';
        }
    }

    need = strlen(s) + strlen(val) + (insert ? strlen(key) + 12 : 0) + 8;
    out = (char *)malloc(need);
    if (!out) return 0;
    {
        char *w = out;
        memcpy(w, s, (size_t)(cut_a - s));
        w += cut_a - s;
        if (insert) {
            *w++ = '\n';
            *w++ = ' ';
            *w++ = ' ';
            *w++ = '"';
            memcpy(w, key, strlen(key));
            w += strlen(key);
            *w++ = '"';
            *w++ = ':';
            *w++ = ' ';
        }
        memcpy(w, val, strlen(val));
        w += strlen(val);
        if (comma) {
            *w++ = ',';
            /* one member a line, the way the launcher writes it. Only when
               the next thing along is not already on its own line, so a
               pretty-printed file keeps its own shape. */
            if (*cut_b != '\n' && *cut_b != '\r' && *cut_b != ' ' &&
                *cut_b != '\t') {
                *w++ = '\n';
                *w++ = ' ';
                *w++ = ' ';
            }
        }
        memcpy(w, cut_b, strlen(cut_b) + 1);
    }
    return out;
}

/* Whole text to `path`, through a sibling temp file and a replacing rename.
   A settings.json truncated by a crash mid-write would read as malformed and
   throw away every launcher key in it, and the launcher's own loader would
   do the same; going via a temp means the file a reader sees is either the
   old one or the new one.

   run mg16 lane MP2: THE TEMP NAME CARRIES THE INSTANCE SUFFIX, and that is a
   BUG FIX rather than tidiness. Two copies of the game share one settings.json,
   so with a single shared "settings.json.tmp" two overlapping saves both open
   the SAME temp, interleave their bytes into it, and then each rename it into
   place -- so MoveFileExA publishes a MIXTURE of two writes and the atomicity
   this comment claims is defeated exactly when it is needed. A per-instance
   temp restores it: each writer has its own file, and the rename is still the
   single atomic publish. Unset, the name is "settings.json.tmp" as before. */
int write_text(const char *path, const char *text)
{
    char tmp[1024];
    if (strlen(path) + 24 >= sizeof tmp) return 0;
    snprintf(tmp, sizeof tmp, "%s%s.tmp", path, port_instance_tag());
    {
        FILE *f = fopen(tmp, "wb");
        size_t n = strlen(text);
        if (!f) return 0;
        if (fwrite(text, 1, n, f) != n) { fclose(f); remove(tmp); return 0; }
        if (fclose(f) != 0) { remove(tmp); return 0; }
    }
#ifdef _WIN32
    if (!MoveFileExA(tmp, path, MOVEFILE_REPLACE_EXISTING)) {
        remove(tmp);
        return 0;
    }
#else
    if (rename(tmp, path) != 0) { remove(tmp); return 0; }
#endif
    return 1;
}

int g_loaded;
int g_swap_camera_turn;              /* default 0 */

/* ---- HOW RUNNING WORKS -------------------------------------------------
   The DS had no run button. Mario runs when the stick is pushed past about
   seven eighths of its travel (func_ov002_020d4748 and its siblings test the
   input record's magnitude against 0xdc7 of 0x1000, with a 0x80 of
   hysteresis), and the DS's stick was the touch screen. A host pad has a
   real stick and a host keyboard has neither, so this program has always
   fed the game the D-pad path -- full magnitude, all the time -- and put
   running on a HELD BUTTON, which is a binding the hardware never had.

   These three settings pick which of those the player gets. They shape the
   input record on the way in and nothing else: the game reads the same
   fields it always read, and src/ has no idea any of this exists.

     RunMode         "button" (default) | "analog" | "auto"
     RunButtonKey    Win32 virtual-key code for the keyboard binding.
                     Default 0x10, which is shift -- what the window has
                     always used. 0 means no keyboard binding at all.
     RunButtonPad    XInput button mask for the pad binding. Default 0x4000,
                     which is X on an Xbox layout -- again what the window
                     has always used. 0 means no pad binding.

   A missing file, a missing key or a value that will not parse is the
   default, so a player who never opens the menu gets exactly the program
   that shipped before this existed. */
int g_run_mode;                      /* default RUN_BUTTON (0) */
int g_run_key;                       /* default 0x10, VK_SHIFT */
int g_run_pad;                       /* default 0x4000, pad X */

const char *const RUN_MODE_KEY[3] = { "button", "analog", "auto" };

/* ---- CameraMode -----------------------------------------------------------
   0 analog, 1 freecam, 2 ds -- tests/walk_window.cpp's CAM_ numbering, and the
   default is analog because that is what main has always promoted an
   interactive run to. The header carries the rest. Read by name and, like
   RunMode, by number too. */
int g_camera_mode;                   /* default 0, analog */
const char *const CAMERA_MODE_KEY[3] = { "analog", "freecam", "ds" };

/* ---- THE CONTROL BINDINGS -------------------------------------------------
   One settings.json key per action, keyboard and pad in two tables indexed by
   the HOST_KEY_ / HOST_PAD_ enums in the header, which is also where the
   meaning of every default is written down. The two tables below hold the
   SPELLING and the DEFAULT side by side so they cannot drift apart, and the
   stderr line at the end of load_once walks the same tables.

   The run action is special in one way only: its old spellings, RunButtonKey
   and RunButtonPad, still work. KeyRun / PadRun win when present; the old
   name is read when the new one is not there; and the save path writes both.
   That is the whole of the alias, and it lives in the two lines that read the
   run entry rather than in a rule anywhere else. */
struct BindKey { const char *name; int dflt; };
const BindKey KEY_BIND[14] = {
    { "KeyUp",       0x57 },      /* W */
    { "KeyDown",     0x53 },      /* S */
    { "KeyLeft",     0x41 },      /* A */
    { "KeyRight",    0x44 },      /* D */
    { "KeyUpAlt",    0x26 },      /* VK_UP */
    { "KeyDownAlt",  0x28 },      /* VK_DOWN */
    { "KeyLeftAlt",  0x25 },      /* VK_LEFT */
    { "KeyRightAlt", 0x27 },      /* VK_RIGHT */
    { "KeyJump",     0x20 },      /* VK_SPACE */
    { "KeyAttack",   0x58 },      /* X */
    { "KeyCrouch",   0x11 },      /* VK_CONTROL */
    { "KeyRun",      0x10 },      /* VK_SHIFT; alias RunButtonKey */
    { "KeyStart",    0x0d },      /* VK_RETURN */
    { "KeySelect",   0x08 },      /* VK_BACK */
};
const BindKey PAD_BIND[6] = {
    { "PadJump",   0x1000 },      /* A */
    { "PadAttack", 0x2000 },      /* B */
    { "PadCrouch", 0x20000 },     /* RT, the trigger pseudo-button; see header */
    { "PadRun",    0x4000 },      /* X; alias RunButtonPad */
    { "PadStart",  0x0010 },      /* START */
    { "PadSelect", 0      },      /* BACK opens the debug menu; see header */
};
int g_key[14];
int g_pad[6];

/* ---- THE DS SCREEN GAP -------------------------------------------------
   The four keys behind the launcher's "remove minigame gap" checkbox and
   the three that shape the gap when it is left in. See port/hal/screen_gap.h
   for what the gap IS; these are only how a player says what they want.

     MinigameGap    true (default) leaves the hinge simulated, which is what
                    the game's own code does and what makes an object that
                    crosses between the screens move at one speed. The
                    launcher's checkbox is the INVERSE of this key: ticking
                    "remove minigame gap" writes false, and false pulls the
                    two screens back together and puts the seam jump back.
     GapFillMode    "ambient" (default), "solid" or "custom". Ignored when
                    GapPeek is true; see below.
     GapColor       "#RRGGBB" for the solid fill. Default "#000000". Ignored
                    when GapPeek is true; see below.
     GapPeek        false (default). True draws the sprites that are genuinely
                    inside the band, over the scene's own art if there is any
                    and over BLACK if there is not.

   "CUSTOM" IS A PICTURE DRAWN FOR ONE MINIGAME, and it comes off disk rather
   than out of a setting:

       <bundle>/gap/scene_<id>.bmp        gap/scene_368.bmp is Bob-omb Squad

   in the same folder settings.json is looked for, named by the scene's own id.
   24-bit or 32-bit uncompressed BMP, 256 x G or 512 x 2G (the 2x sheet, taken
   down by nearest sampling), BAND ROWS ONLY -- no top-screen or bottom-screen
   context rows. MAGENTA #FF00FF IS THE TRANSPARENCY KEY: a keyed pixel shows
   whatever is behind the art, which is the ambient wash with peek off and
   black with peek on, so a band can be a painted machine down the middle and
   the ordinary backdrop either side of it. Anything wrong with the file -- not
   there, wrong size, wrong depth, truncated -- is ONE LINE on stderr and the
   ambient fill, never a crash and never a guess. hal/gap_art.h carries the
   long form.

   PEEK IGNORES THE FILL MODE, and that is a decision about what the mode is
   FOR rather than a shortcut. With peek off the band is decoration and these
   keys are the player's taste in it. With peek on the band stops being
   decoration and becomes a view of what is actually in those rows, and the
   true state of a row with no sprite in it is empty. A FILL behind that would
   put something over the answer and call the mixture the answer: an ambient
   wash blurs into the sprites it sits behind, and a chosen colour cannot be
   told apart from a sprite of the same colour.

   THE ART IS ALLOWED BEHIND PEEK ANYWAY, because it is a different kind of
   thing. A hand-drawn band reads as a picture -- a hinge, a machine, a painted
   backing -- and nobody mistakes it for the game submitting a sprite, which is
   exactly what a wash or a flat colour invites. So peek's backdrop is the
   scene's art when the file is there and black when it is not, and everything
   over that backdrop is still only what the engines really submitted.

   All four are optional and each falls back on its own, so a settings.json
   written by a launcher that predates any of them is read exactly as a file
   that sets it to the default -- which is the property that lets the game
   and the launcher ship on their own schedules. */
int g_gap_on;                        /* default 1 */
/* 0 solid, 1 ambient, 2 custom -- the ntr::GAP_FILL_* numbering, which is what
   hal/screen_gap.cpp hands straight to ntr::stack_layout. That function
   re-validates it and reads anything it does not know as ambient, so the two
   spellings cannot disagree about an unknown value. */
int g_gap_fill;                      /* default 1, ambient */
unsigned g_gap_color;                /* default 0xFF000000, black */
int g_gap_peek;                      /* default 0 */

/* ---- GaplessMinigames, and it is not one of the four ----------------------
   The four above are the player's taste in a PICTURE and none of them writes
   the game's own G. This one writes it, to zero, once a SUPPORTED minigame's
   InitResources has finished, and that changes what the GAME does: a crossing
   object stops spending the hinge's rows in a place neither screen shows, so
   it arrives on the top screen sooner than the DS delivers it.

   SO IT IS A MOD, IT IS DEFAULTED OFF, AND IT IS "WHEN ABLE".
   port/hal/host_settings.h carries the full statement of what it costs, and
   hal/screen_gap.cpp carries the mechanism, the table of scenes it is proven
   for, and the one line it prints at every minigame latch. Nothing reads this
   except that file. */
int g_gapless_minigames;             /* default 0, and 0 is the ROM */

/* ---- LovesMeCharacter, the second Mods-panel key that changes the GAME ----
   Which character stars in the Loves Me...? petal minigame (scene 390).
   The ROM's answer is Yoshi and that is the default. The value is one of
   the spellings in LOVESME_KEY below (1-based into that table); "" or an
   absent key is Yoshi, the ROM.

   The swap itself lives in hal/fs_mods.cpp, at the file seam: the minigame
   asks for Yoshi's model by file id and is served a model composed from
   the player's own extraction instead. Nothing about it reloads live --
   the file layer caches what it serves -- so like every Mods key this one
   keeps its boot value and the launcher's dialog promises the restart.
   Anything that is not a spelling this build knows reads as the default,
   so a settings.json from a LATER launcher that offers more characters
   degrades to Yoshi here rather than to a guess. */
const char *const LOVESME_KEY[6] = {
    "mario", "yoshi_red", "yoshi_blue", "yoshi_yellow", "luigi", "wario",
};
int g_lovesme_character;             /* default 0, and 0 is the ROM */

/* ---- CustomPalette, the third Mods-panel key that changes the GAME --------
   Which palette combo recolors the playable characters. The ROM's colors
   are the default and 0 is the ROM. 1..3 name a combo FILE,
   palettes/combo<N>.pal beside settings.json, holding replacement color
   sets the file seam writes into the characters' model palettes as they
   load. The key only picks the file; every target name and color comes out
   of the file itself, so this build never learns who is recolored what.

   Like LovesMeCharacter the mechanism is hal/fs_mods.cpp's and is
   boot-latched -- the file layer caches what it serves. A value outside
   1..3 reads as the default, the ROM. */
int g_custom_palette;                /* default 0, and 0 is the ROM */

/* ---- PaletteMario / PaletteLuigi / PaletteWario / PaletteYoshi ------------
   The per-character picker that supersedes the single combo above. Each key
   is a STRING: "" or absent is the ROM, a built-in "yoshi:<color>" spelling
   is one of the ROM's own VS rows, and anything else is the basename of
   palettes/<value>.pal. The header carries the full statement; the mechanism,
   the per-character file ownership and the Yoshi rows are all in
   hal/fs_mods.cpp, which is the only reader.

   Kept as WRITTEN, not resolved: this file's job is to report what the
   player asked for, and deciding whether a name exists on disk belongs to
   the layer that opens it. A value too long for the buffer degrades to ""
   -- the ROM -- because json_str leaves the buffer alone when it will not
   fit, which is the same "a typo is not a choice" rule the rest of the
   file follows. */
const char *const PALETTE_KEY[4] = {
    "PaletteMario", "PaletteLuigi", "PaletteWario", "PaletteYoshi",
};
const char *const PALETTE_WHO[4] = { "Mario", "Luigi", "Wario", "Yoshi" };
char g_char_palette[4][96];          /* all "" by default, and "" is the ROM */

/* PaletteYoshi's four built-in spellings, the ROM's own four-player colors,
   in the order the rows are stacked in yoshi_all_16p_pl. The SPELLINGS live
   here and the mechanism lives in hal/fs_mods.cpp, which is the same split
   LovesMeCharacter has: this file turns a word into an index and that file
   knows what the index means. One owner, so the two cannot drift.

   A "yoshi:" value that is NOT one of these four is a typo rather than a
   file name -- no file system this program runs on allows a colon in one --
   so it reads as the default, the ROM, and says so. That is the same rule
   LovesMeCharacter states for a spelling this build does not know. */
const char *const YOSHI_ROW_KEY[4] = {
    "yoshi:green", "yoshi:red", "yoshi:blue", "yoshi:yellow",
};
int g_yoshi_row = -1;                /* -1 = not a built-in row */

/* Volume, 0..100, or -1 while the file has never named one. The launcher owns
   this key and also passes it as SM64DS_VOLUME at launch; the file copy exists
   so the live re-read below can move it while the game is running. */
int g_volume = -1;

/* MouseCapture: 1 when the player asked the window to hold the pointer and
   steer the camera with bare movement instead of a right-button drag. Default
   0, and 0 is what this program has always done. See the header for the list
   of places it deliberately does not engage; every one of them is enforced in
   tests/walk_window.cpp, because whether a pointer may be taken is a question
   about the window and not about this file. */
int g_mouse_capture;

/* NameTags: 1 when the VS name-and-star tag over a remote player's head is
   drawn. Default 1, and unlike the Mods keys the default is ON -- see the
   header for why a host overlay that reads no game state is a preference
   rather than a mod. */
int g_name_tags = 1;

/* AdventureGhosts: 1 when the player asked to play the single-player adventure
   together, seeing other players as see-through ghosts. Default 0, and 0 is
   the ROM's own solo game. Unlike NameTags the default is OFF -- see the
   header for why a MODE defaults off where a presentation preference defaults
   on. Read live like the gap keys; the accessor also honours SM64DS_ADVENTURE. */
int g_adventure_ghosts = 0;

/* ---- PROXIMITY VOICE CHAT, lane VOICE ------------------------------------
   Five keys, all of them host preferences and none of them a mod: nothing
   here touches game state, the lockstep input path, or a single byte the
   simulation reads. The whole feature is off by default, and off means NO
   CAPTURE DEVICE IS OPENED AT ALL -- not a device opened and discarded, not
   a muted stream. A player who never turns this on has a build whose
   microphone is untouched.

     VoiceEnabled     false (default). The master switch. RELOADS LIVE, so
                      the launcher's toggle takes effect mid-match: turning
                      it off closes the capture device and silences every
                      remote voice within a frame, turning it on opens the
                      device again.
     VoiceMicDevice   "" (default) is the system's default recording device.
                      Any other value is matched, case-insensitively and as a
                      SUBSTRING, against the names winmm reports for the
                      machine's recording devices; the first match wins and no
                      match falls back to the default device with one line on
                      stderr. Substring rather than exact because the names
                      winmm reports are truncated to 31 characters and a
                      launcher listing them cannot always show a player the
                      whole thing. RELOADS LIVE: a changed name reopens the
                      device.
     VoiceVolume      0..100, default 80. A linear gain on the decoded remote
                      audio, applied on top of the distance falloff. It is NOT
                      the game's Volume key and is deliberately independent of
                      it -- a player who muted the game to hear his friends
                      should still hear his friends -- so the voice mix runs
                      after the master trim in hal/sdat/mixer.cpp. RELOADS
                      LIVE.
     VoiceNearRadius  world units, default 512. Inside this distance a remote
                      player is at full VoiceVolume. RELOADS LIVE.
     VoiceFarRadius   world units, default 3072. At and beyond this distance a
                      remote player is silent, and between the two radii the
                      gain falls off logarithmically. A file that sets far <=
                      near gets the defaults back, because a falloff with no
                      span is not a choice anybody made. RELOADS LIVE.

   The radii are in the game's own world units -- the integer part of the
   Fix12 positions at Actor+0x5c -- and the defaults are measured rather than
   guessed; port/status/VOICE.md carries the arena span they came from. */
int  g_voice_enabled;                /* default 0, and 0 opens no device */
int  g_voice_volume = 80;
char g_voice_mic[96];                /* "" = the system default device */
int  g_voice_near = 512;
int  g_voice_far  = 3072;

/* Steps once per live re-read that changed an answer. hal/screen_gap.cpp
   latches on it. */
int g_setgen;

/* ---- PadLayouts ------------------------------------------------------------
   The learned controller maps, see the header. Kept as a small fixed table;
   the file's array is parsed object by object with the same scalar readers
   as every other key, each object copied out on its own so "a" inside one
   object cannot be found inside another. */
HostPadLayout g_padlayouts[HOST_PAD_LAYOUT_MAX];
int g_padlayout_n;

/* The generic fallback row of pad_backend.cpp's PAD_LAYOUTS, field for
   field. Every field a file object lacks reads as this. */
void padlayout_default(HostPadLayout *o)
{
    memset(o, 0, sizeof *o);
    o->a = 0; o->b = 1; o->x = 2; o->y = 3; o->lb = 4; o->rb = 5;
    o->back = 8; o->start = 9; o->lthumb = 10; o->rthumb = 11;
    o->lt_btn = 6; o->rt_btn = 7; o->lt_axis = -1; o->rt_axis = -1;
    o->lx_axis = 0; o->ly_axis = 1; o->rx_axis = 2; o->ry_axis = 5;
    o->lx_sign = 1; o->ly_sign = -1; o->rx_sign = 1; o->ry_sign = -1;
}

/* The field table: spelling, member, range. One row per PadLayout field so
   the parser, the clamp and the writer walk the same list. */
struct PadField { const char *name; int HostPadLayout::*f; int lo, hi; };
const PadField PAD_FIELDS[22] = {
    { "a",       &HostPadLayout::a,       -1, 31 },
    { "b",       &HostPadLayout::b,       -1, 31 },
    { "x",       &HostPadLayout::x,       -1, 31 },
    { "y",       &HostPadLayout::y,       -1, 31 },
    { "lb",      &HostPadLayout::lb,      -1, 31 },
    { "rb",      &HostPadLayout::rb,      -1, 31 },
    { "back",    &HostPadLayout::back,    -1, 31 },
    { "start",   &HostPadLayout::start,   -1, 31 },
    { "lthumb",  &HostPadLayout::lthumb,  -1, 31 },
    { "rthumb",  &HostPadLayout::rthumb,  -1, 31 },
    { "lt_btn",  &HostPadLayout::lt_btn,  -1, 31 },
    { "rt_btn",  &HostPadLayout::rt_btn,  -1, 31 },
    { "lt_axis", &HostPadLayout::lt_axis, -1, 5 },
    { "rt_axis", &HostPadLayout::rt_axis, -1, 5 },
    { "lx_axis", &HostPadLayout::lx_axis, -1, 5 },
    { "ly_axis", &HostPadLayout::ly_axis, -1, 5 },
    { "rx_axis", &HostPadLayout::rx_axis, -1, 5 },
    { "ry_axis", &HostPadLayout::ry_axis, -1, 5 },
    { "lx_sign", &HostPadLayout::lx_sign, -1, 1 },
    { "ly_sign", &HostPadLayout::ly_sign, -1, 1 },
    { "rx_sign", &HostPadLayout::rx_sign, -1, 1 },
    { "ry_sign", &HostPadLayout::ry_sign, -1, 1 },
};

/* One object's text (NUL-terminated, braces included) into *o. 0 when the
   object has no usable vid:pid, which drops it. A sign of 0 is out of
   range: an axis that moves neither way is not a choice. */
int padlayout_parse_object(const char *obj, HostPadLayout *o)
{
    padlayout_default(o);
    const int vid = json_int(obj, "vid", 0);
    const int pid = json_int(obj, "pid", 0);
    if (vid < 1 || vid > 0xffff || pid < 1 || pid > 0xffff) return 0;
    o->vid = vid;
    o->pid = pid;
    for (int i = 0; i < 22; ++i) {
        const int dflt = o->*PAD_FIELDS[i].f;
        const int v = json_int(obj, PAD_FIELDS[i].name, dflt);
        const int is_sign = PAD_FIELDS[i].lo == -1 && PAD_FIELDS[i].hi == 1;
        if (v >= PAD_FIELDS[i].lo && v <= PAD_FIELDS[i].hi && !(is_sign && v == 0))
            o->*PAD_FIELDS[i].f = v;
    }
    if (json_str(obj, "name", o->name, sizeof o->name)) {
        /* json_str hands the value back as written; the writer below
           escapes quotes and backslashes, so undo exactly those two */
        char *w = o->name;
        for (const char *r = o->name; *r; ++r) {
            if (*r == '\\' && (r[1] == '"' || r[1] == '\\')) ++r;
            *w++ = *r;
        }
        *w = '\0';
    }
    return 1;
}

/* The whole array. Malformed text stops the walk where it is found; the
   objects already read stay. */
void padlayouts_parse(const char *text)
{
    g_padlayout_n = 0;
    const char *v = json_value(text, "PadLayouts");
    if (!v || *v != '[') return;
    const char *end = json_value_end(v);
    const char *p = v + 1;
    while (p < end && g_padlayout_n < HOST_PAD_LAYOUT_MAX) {
        while (p < end && *p != '{' && *p != ']') ++p;
        if (p >= end || *p == ']') break;
        const char *e = json_value_end(p);
        if (e <= p || e > end) break;
        const size_t n = (size_t)(e - p);
        char *obj = (char *)malloc(n + 1);
        if (!obj) break;
        memcpy(obj, p, n);
        obj[n] = '\0';
        HostPadLayout o;
        const int ok = padlayout_parse_object(obj, &o);
        free(obj);
        if (ok) {
            /* a later object for the same pad replaces the earlier one, so
               a hand edit that duplicates a row reads as its last word */
            int slot = g_padlayout_n;
            for (int i = 0; i < g_padlayout_n; ++i)
                if (g_padlayouts[i].vid == o.vid && g_padlayouts[i].pid == o.pid)
                    slot = i;
            g_padlayouts[slot] = o;
            if (slot == g_padlayout_n) ++g_padlayout_n;
        }
        p = e;
    }
}

/* The array as text, the shape the header shows, into a malloc'd buffer. */
char *padlayouts_text(void)
{
    const size_t cap = 64 + (size_t)g_padlayout_n * 512;
    char *out = (char *)malloc(cap);
    if (!out) return 0;
    size_t w = 0;
    w += (size_t)snprintf(out + w, cap - w, "[");
    for (int i = 0; i < g_padlayout_n; ++i) {
        const HostPadLayout *o = &g_padlayouts[i];
        /* the name is written with quotes escaped, and backslashes and
           control characters DROPPED, so a product name cannot break the
           file; the name is for a person reading it, not for matching */
        char nm[96];
        size_t k = 0;
        for (const char *c = o->name; *c && k + 3 < sizeof nm; ++c) {
            if ((unsigned char)*c < 0x20 || *c == '\\') continue;
            if (*c == '"') nm[k++] = '\\';
            nm[k++] = *c;
        }
        nm[k] = '\0';
        w += (size_t)snprintf(out + w, cap - w,
                              "%s\n    { \"vid\": %d, \"pid\": %d, \"name\": \"%s\"",
                              i ? "," : "", o->vid, o->pid, nm);
        for (int f = 0; f < 22; ++f)
            w += (size_t)snprintf(out + w, cap - w, ",%s\"%s\": %d",
                                  (f % 6 == 0) ? "\n      " : " ",
                                  PAD_FIELDS[f].name, o->*PAD_FIELDS[f].f);
        w += (size_t)snprintf(out + w, cap - w, " }");
        if (w >= cap) { free(out); return 0; }
    }
    snprintf(out + w, cap - w, "%s]", g_padlayout_n ? "\n  " : "");
    return out;
}

/* "#RRGGBB" to 0xFFRRGGBB. Returns dflt for anything that is not exactly six
   hex digits after an optional '#', so a half-typed colour is the default
   rather than a colour nobody chose. Case insensitive, because a player
   pasting a colour out of a picker gets either case. */
unsigned parse_hex_color(const char *s, unsigned dflt)
{
    unsigned v = 0;
    int n = 0;
    if (!s) return dflt;
    if (*s == '#') ++s;
    for (; *s; ++s, ++n) {
        int d;
        if (*s >= '0' && *s <= '9') d = *s - '0';
        else if (*s >= 'a' && *s <= 'f') d = *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'F') d = *s - 'A' + 10;
        else return dflt;
        if (n >= 6) return dflt;
        v = (v << 4) | (unsigned)d;
    }
    if (n != 6) return dflt;
    return 0xFF000000u | v;
}

/* ---- THE VOICE KEYS, read in ONE place ---------------------------------
   Called from load_once with the variables at their defaults and from
   reload_live with the variables holding whatever the last read produced, so
   both callers get the same "an absent key keeps what we have" rule for free.
   Returns 1 when any answer changed.

   VoiceMicDevice is read into a scratch buffer first: json_str leaves its
   output alone when a value will not fit, and a name too long for the buffer
   has to read as "no name given" (the default device) rather than as the
   previous name, which is what a bare read into g_voice_mic would have done
   on a value that overflowed. */
int read_voice_keys(const char *text)
{
    const int en = json_bool(text, "VoiceEnabled", g_voice_enabled);
    int vol = g_voice_volume;
    int near_r = g_voice_near;
    int far_r = g_voice_far;
    char mic[sizeof g_voice_mic];
    mic[0] = '\0';
    json_str(text, "VoiceMicDevice", mic, sizeof mic);
    {
        const int v = json_int(text, "VoiceVolume", -1);
        if (v >= 0) vol = v > 100 ? 100 : v;
    }
    {
        const int v = json_int(text, "VoiceNearRadius", -1);
        if (v >= 0) near_r = v;
    }
    {
        const int v = json_int(text, "VoiceFarRadius", -1);
        if (v >= 0) far_r = v;
    }
    /* A falloff with no span, or an inverted one, is not a choice a player
       made -- it is a typo or a launcher bug -- so both radii go back to the
       defaults together rather than one of them being quietly clamped to the
       other and producing a hard on/off cutoff nobody asked for. */
    if (far_r <= near_r) { near_r = 512; far_r = 3072; }

    int changed = 0;
    if (en != g_voice_enabled) { g_voice_enabled = en; changed = 1; }
    if (vol != g_voice_volume) { g_voice_volume = vol; changed = 1; }
    if (near_r != g_voice_near) { g_voice_near = near_r; changed = 1; }
    if (far_r != g_voice_far) { g_voice_far = far_r; changed = 1; }
    if (strcmp(mic, g_voice_mic) != 0) {
        strncpy(g_voice_mic, mic, sizeof g_voice_mic - 1);
        g_voice_mic[sizeof g_voice_mic - 1] = '\0';
        changed = 1;
    }
    return changed;
}

void load_once(void)
{
    if (g_loaded) return;
    g_loaded = 1;
    g_swap_camera_turn = 0;
    g_run_mode = 0;
    g_run_key = 0x10;
    g_run_pad = 0x4000;
    g_camera_mode = 0;
    for (int i = 0; i < 14; ++i) g_key[i] = KEY_BIND[i].dflt;
    for (int i = 0; i < 6; ++i) g_pad[i] = PAD_BIND[i].dflt;
    g_gap_on = 1;
    g_gap_fill = 1;
    g_gap_color = 0xFF000000u;
    g_gap_peek = 0;
    g_gapless_minigames = 0;
    g_lovesme_character = 0;
    g_mouse_capture = 0;
    g_custom_palette = 0;
    g_name_tags = 1;
    g_adventure_ghosts = 0;
    for (int i = 0; i < 4; ++i) g_char_palette[i][0] = '\0';
    g_yoshi_row = -1;
    g_padlayout_n = 0;
    g_voice_enabled = 0;
    g_voice_volume = 80;
    g_voice_mic[0] = '\0';
    g_voice_near = 512;
    g_voice_far = 3072;

    char path[1024];
    if (!find_settings(path, sizeof path)) return;
    char *text = slurp(path);
    if (!text) return;
    if (looks_like_json_object(text)) {
        g_swap_camera_turn = json_bool(text, "SwapCameraTurnDirection", 0);
        /* the mode by NAME, and by number as well: the launcher serializes a
           C# enum as an integer unless it is told otherwise, and a player
           editing by hand will write the word */
        char mode[16];
        if (json_str(text, "RunMode", mode, sizeof mode)) {
            int matched = 0;
            for (int i = 0; i < 3; ++i)
                if (strlen(mode) == strlen(RUN_MODE_KEY[i]) &&
                    ieq(mode, RUN_MODE_KEY[i], strlen(mode))) {
                    g_run_mode = i;
                    matched = 1;
                }
            if (!matched) {
                const int n = json_int(text, "RunMode", 0);
                if (n >= 0 && n <= 2) g_run_mode = n;
            }
        }
        /* a binding outside the code space is a typo, not a choice */
        {
            const int k = json_int(text, "RunButtonKey", 0x10);
            if (k >= 0 && k <= 0xff) g_run_key = k;
            const int p = json_int(text, "RunButtonPad", 0x4000);
            if (p >= 0 && p <= HOST_PAD_MASK_MAX) g_run_pad = p;
        }
        /* the camera mode, by name and by number like RunMode */
        {
            char mode[16];
            if (json_str(text, "CameraMode", mode, sizeof mode)) {
                int matched = 0;
                for (int i = 0; i < 3; ++i)
                    if (strlen(mode) == strlen(CAMERA_MODE_KEY[i]) &&
                        ieq(mode, CAMERA_MODE_KEY[i], strlen(mode))) {
                        g_camera_mode = i;
                        matched = 1;
                    }
                if (!matched) {
                    const int n = json_int(text, "CameraMode", 0);
                    if (n >= 0 && n <= 2) g_camera_mode = n;
                }
            }
        }
        /* the bindings, each read against its OWN default so a file that
           moves jump and says nothing about the rest is honoured for jump.
           Out of the code space is a typo, not a choice, like the run pair. */
        int keyrun_ok = 0, padrun_ok = 0;   /* KeyRun / PadRun parsed IN RANGE */
        for (int i = 0; i < 14; ++i) {
            const int k = json_int(text, KEY_BIND[i].name, KEY_BIND[i].dflt);
            if (k >= 0 && k <= 0xff) {
                g_key[i] = k;
                if (i == 11 && json_value(text, KEY_BIND[i].name)) keyrun_ok = 1;
            }
        }
        for (int i = 0; i < 6; ++i) {
            const int p = json_int(text, PAD_BIND[i].name, PAD_BIND[i].dflt);
            if (p >= 0 && p <= HOST_PAD_MASK_MAX) {
                g_pad[i] = p;
                if (i == 3 && json_value(text, PAD_BIND[i].name)) padrun_ok = 1;
            }
        }
        /* THE ALIAS: KeyRun / PadRun win when the file names them WITH A VALUE
           THAT PARSED; when it does not, the old RunButtonKey / RunButtonPad
           answer stands, which is what the two reads above already left in
           g_run_key / g_run_pad. "Parsed", not "present": a KeyRun of 999 is a
           typo and must not beat a valid RunButtonKey beside it, which a
           presence test would let it do by handing over the default. After
           these two lines g_run_key and g_key[RUN] are one value. */
        if (keyrun_ok) g_run_key = g_key[11];
        else g_key[11] = g_run_key;
        if (padrun_ok) g_run_pad = g_pad[3];
        else g_pad[3] = g_run_pad;
        /* the screen gap. Each key is read against its OWN default, so a file
           that sets one of the four and none of the others is honoured for
           the one it set. */
        g_gap_on = json_bool(text, "MinigameGap", 1);
        g_gap_peek = json_bool(text, "GapPeek", 0);
        /* read beside the other four and defaulted on its own, so a file
           written before this key existed reads as a file that turned it
           off -- which is the ROM */
        g_gapless_minigames = json_bool(text, "GaplessMinigames", 0);
        /* defaulted ON, on its own line for the gap keys' reason: a file
           written before this key existed reads as a file that left it on,
           which is what a player who never opened the dialog expects. */
        g_name_tags = json_bool(text, "NameTags", 1);
        /* the adventure-ghost mode, read against its own default of OFF beside
           the keys above: a file written before this key existed reads as one
           that left it off, which is the ROM's solo game. */
        g_adventure_ghosts = json_bool(text, "AdventureGhosts", 0);
        {
            char who[24];
            if (json_str(text, "LovesMeCharacter", who, sizeof who))
                for (int i = 0; i < 6; ++i)
                    if (strlen(who) == strlen(LOVESME_KEY[i]) &&
                        ieq(who, LOVESME_KEY[i], strlen(who)))
                        g_lovesme_character = i + 1;
            /* "", an absent key and any other spelling are all Yoshi */
        }
        {
            /* out of range is the ROM, same shape as the spellings above */
            const int n = json_int(text, "CustomPalette", 0);
            if (n >= 1 && n <= 3) g_custom_palette = n;
        }
        /* the four per-character keys, each read against its own default of
           "" so a settings.json naming one and not the others is honoured
           for the one it names -- the same rule the gap keys follow */
        for (int i = 0; i < 4; ++i)
            json_str(text, PALETTE_KEY[i], g_char_palette[i],
                     sizeof g_char_palette[i]);
        /* PaletteYoshi's built-in rows, resolved once here so no other file
           has to carry the spellings. A "yoshi:" value this build does not
           know reads as the default and is emptied, so the file layer never
           goes looking for a file that cannot exist. */
        {
            char *y = g_char_palette[3];
            for (int i = 0; i < 4; ++i)
                if (strlen(y) == strlen(YOSHI_ROW_KEY[i]) &&
                    ieq(y, YOSHI_ROW_KEY[i], strlen(y)))
                    g_yoshi_row = i;
            if (g_yoshi_row < 0 && strlen(y) > 6 && ieq(y, "yoshi:", 6)) {
                fprintf(stderr, "[settings] PaletteYoshi %s is not a color "
                                "this build knows (yoshi:green, yoshi:red, "
                                "yoshi:blue, yoshi:yellow); Yoshi keeps the "
                                "ROM's colors\n", y);
                y[0] = '\0';
            }
        }
        {
            char mode[16];
            if (json_str(text, "GapFillMode", mode, sizeof mode)) {
                if (strlen(mode) == 5 && ieq(mode, "solid", 5)) g_gap_fill = 0;
                else if (strlen(mode) == 7 && ieq(mode, "ambient", 7))
                    g_gap_fill = 1;
                else if (strlen(mode) == 6 && ieq(mode, "custom", 6))
                    g_gap_fill = 2;
                /* anything else keeps the default, like every other reader */
            }
            char col[16];
            if (json_str(text, "GapColor", col, sizeof col))
                g_gap_color = parse_hex_color(col, 0xFF000000u);
        }
        {
            const int v = json_int(text, "Volume", -1);
            if (v >= 0) g_volume = v > 100 ? 100 : v;
        }
        /* read against its own default beside the gap keys, for the same
           reason they are: a settings.json written before this key existed
           reads exactly as one that turned it off, which is the old program */
        g_mouse_capture = json_bool(text, "MouseCapture", 0);
        /* the learned controller maps; absent is none, like every key */
        padlayouts_parse(text);
        /* lane VOICE: the same reader the live re-read uses, so the boot
           values and the reloaded values cannot come out of two different
           pieces of code that drift apart. Every key reads against the value
           already in the variable, which at boot is its default. */
        read_voice_keys(text);
    }
    free(text);

    /* Said out loud only when a setting is off its default, so an ordinary
       run's log is unchanged and a support log shows at a glance which
       non-default choices were in force. */
    if (g_swap_camera_turn)
        fprintf(stderr, "[settings] SwapCameraTurnDirection on (%s)\n", path);
    if (g_run_mode || g_run_key != 0x10 || g_run_pad != 0x4000)
        fprintf(stderr, "[settings] RunMode %s key 0x%02x pad 0x%04x (%s)\n",
                RUN_MODE_KEY[g_run_mode], (unsigned)g_run_key,
                (unsigned)g_run_pad, path);
    if (g_camera_mode)
        fprintf(stderr, "[settings] CameraMode %s (%s)\n",
                CAMERA_MODE_KEY[g_camera_mode], path);
    for (int i = 0; i < g_padlayout_n; ++i)
        fprintf(stderr, "[settings] PadLayouts: learned layout for %04x:%04x "
                        "%s (%s)\n", (unsigned)g_padlayouts[i].vid,
                (unsigned)g_padlayouts[i].pid, g_padlayouts[i].name, path);
    /* one line per binding the player moved, so a support log answers "what
       was jump bound to" without anyone opening the file */
    for (int i = 0; i < 14; ++i)
        if (g_key[i] != KEY_BIND[i].dflt)
            fprintf(stderr, "[settings] %s 0x%02x (default 0x%02x) (%s)\n",
                    KEY_BIND[i].name, (unsigned)g_key[i],
                    (unsigned)KEY_BIND[i].dflt, path);
    for (int i = 0; i < 6; ++i)
        if (g_pad[i] != PAD_BIND[i].dflt)
            fprintf(stderr, "[settings] %s 0x%04x (default 0x%04x) (%s)\n",
                    PAD_BIND[i].name, (unsigned)g_pad[i],
                    (unsigned)PAD_BIND[i].dflt, path);
    if (!g_gap_on || g_gap_fill != 1 || g_gap_color != 0xFF000000u ||
        g_gap_peek)
        fprintf(stderr, "[settings] MinigameGap %s, fill %s #%06x, peek %s "
                "(%s)\n", g_gap_on ? "on" : "OFF",
                g_gap_fill == 0   ? "solid"
                : g_gap_fill == 2 ? "custom"
                                  : "ambient",
                g_gap_color & 0xffffffu, g_gap_peek ? "ON" : "off", path);
    /* SAID ON ITS OWN LINE AND IN PLAIN WORDS, because this is the one key
       in the file that makes the program stop agreeing with the ROM. A
       support log that carries it should say so without anybody having to
       know what the key is called. */
    if (g_gapless_minigames)
        fprintf(stderr, "[settings] GaplessMinigames ON -- a supported "
                        "minigame's screen gap is removed from the SIMULATION, "
                        "not just the picture. Objects cross the seam sooner "
                        "than they do on a DS. This is a mod, not the game. "
                        "(%s)\n", path);
    /* The same plain-words rule as GaplessMinigames: a support log carrying
       this key should say what the player is looking at. */
    /* Off its default, so it is said; and said in plain words because a
       support log where the player reports "the game stole my mouse" should
       carry the reason on one line. */
    /* lane VOICE: said only when it is on, like every other non-default, and
       said in plain words because a support log for "the game is using my
       microphone" should carry the answer on one line. */
    if (g_voice_enabled)
        fprintf(stderr, "[settings] VoiceEnabled on -- proximity voice chat "
                "will open a recording device in an online match. Mic '%s', "
                "volume %d, audible from %d units out to %d (%s)\n",
                g_voice_mic[0] ? g_voice_mic : "(system default)",
                g_voice_volume, g_voice_near, g_voice_far, path);
    if (g_mouse_capture)
        fprintf(stderr, "[settings] MouseCapture on -- an adventure window "
                        "holds the pointer and bare mouse movement turns the "
                        "camera, with no right button held. Escape hands the "
                        "pointer back (it opens the debug menu). (%s)\n", path);
    /* Said on its own line and in plain words, because this key changes what
       the session IS -- a solo game that opens the wire and draws other
       players as ghosts -- rather than how a body already on the wire looks. */
    if (g_adventure_ghosts)
        fprintf(stderr, "[settings] AdventureGhosts on -- this is a solo game "
                        "that opens the network and draws other players in the "
                        "same level as see-through, walk-through ghosts. No "
                        "shared world. (%s)\n", path);
    if (g_lovesme_character)
        fprintf(stderr, "[settings] LovesMeCharacter %s -- the Loves "
                        "Me...? minigame's Yoshi is replaced at the file "
                        "layer. This is a mod, not the game. (%s)\n",
                LOVESME_KEY[g_lovesme_character - 1], path);
    /* Same plain-words rule again. */
    if (g_custom_palette)
        fprintf(stderr, "[settings] CustomPalette %d -- character colors "
                        "are rewritten at the file layer from "
                        "palettes/combo%d.pal. This is a mod, not the "
                        "game. (%s)\n",
                g_custom_palette, g_custom_palette, path);
    /* One line per key the player actually set, in the same plain words:
       who is recolored and where the colors come from. The built-in Yoshi
       rows say so, because "the ROM's own VS color" and "a file somebody
       authored" are different sentences to anyone reading a support log. */
    for (int i = 0; i < 4; ++i) {
        const char *v = g_char_palette[i];
        if (!*v) continue;
        if (i == 3 && g_yoshi_row >= 0)
            fprintf(stderr, "[settings] PaletteYoshi %s -- Yoshi wears one "
                            "of the game's own four-player colors in the "
                            "adventure. This is a mod, not the game. (%s)\n",
                    v, path);
        else
            fprintf(stderr, "[settings] %s %s -- %s's colors are rewritten "
                            "at the file layer from palettes/%s.pal. This "
                            "is a mod, not the game. (%s)\n",
                    PALETTE_KEY[i], v, PALETTE_WHO[i], v, path);
    }
}

/* ---- the live re-read -----------------------------------------------------
   What the file's write time was when it was last read, so a poll can tell
   "changed" from "looked at". Zero until the first poll finds the file. */
#ifdef _WIN32
unsigned long long g_watch_time;
unsigned long long g_watch_size;
#endif

/* Re-read ONLY the keys the header promises reload live: the four screen-gap
   keys, Volume, MouseCapture and lane VOICE's five. Returns 1 when an answer
   changed. Each key lands on the value it already has rather than its default
   when the file no longer names it, because "the launcher stopped writing a key" and "the
   player turned a key off" are different sentences and only the second one has
   a picture. */
int reload_live(const char *text)
{
    int changed = 0;
    const int gap = json_bool(text, "MinigameGap", g_gap_on);
    const int peek = json_bool(text, "GapPeek", g_gap_peek);
    const int mcap = json_bool(text, "MouseCapture", g_mouse_capture);
    /* reloads live for the same reason MouseCapture does: it is presentation,
       it costs nothing to change mid-run, and the launcher's dialog promises a
       restart only for the Mods panel. */
    const int tags = json_bool(text, "NameTags", g_name_tags);
    /* reloads live beside NameTags for the same reason: the ghost render and
       no-collision paths read it per frame, so the launcher's dialog can turn
       the mode on and off with the game already running. */
    const int adv = json_bool(text, "AdventureGhosts", g_adventure_ghosts);
    int fill = g_gap_fill;
    unsigned color = g_gap_color;
    int vol = g_volume;
    {
        char mode[16];
        if (json_str(text, "GapFillMode", mode, sizeof mode)) {
            if (strlen(mode) == 5 && ieq(mode, "solid", 5)) fill = 0;
            else if (strlen(mode) == 7 && ieq(mode, "ambient", 7)) fill = 1;
            else if (strlen(mode) == 6 && ieq(mode, "custom", 6)) fill = 2;
        }
        char col[16];
        if (json_str(text, "GapColor", col, sizeof col))
            color = parse_hex_color(col, g_gap_color);
    }
    {
        const int v = json_int(text, "Volume", -1);
        if (v >= 0) vol = v > 100 ? 100 : v;
    }
    /* lane VOICE: read UNCONDITIONALLY and before the compare below, because
       its keys own their own change detection. Folding them into the gap
       comparison would have meant a voice-only edit either changing nothing
       (if the read were inside the if) or being announced as a gap change. */
    const int voice_changed = read_voice_keys(text);
    if (voice_changed) {
        changed = 1;
        fprintf(stderr, "[settings] live re-read: voice %s, volume %d, mic "
                "'%s', radii %d..%d\n", g_voice_enabled ? "ON" : "off",
                g_voice_volume, g_voice_mic[0] ? g_voice_mic : "(default)",
                g_voice_near, g_voice_far);
    }
    if (gap != g_gap_on || peek != g_gap_peek || fill != g_gap_fill ||
        color != g_gap_color || vol != g_volume || mcap != g_mouse_capture ||
        tags != g_name_tags || adv != g_adventure_ghosts) {
        g_gap_on = gap;
        g_gap_peek = peek;
        g_gap_fill = fill;
        g_gap_color = color;
        g_volume = vol;
        g_mouse_capture = mcap;
        g_name_tags = tags;
        g_adventure_ghosts = adv;
        changed = 1;
        fprintf(stderr, "[settings] live re-read: MinigameGap %s, fill %s "
                "#%06x, peek %s, volume %d, mouse capture %s, "
                "name tags %s, adventure ghosts %s\n",
                g_gap_on ? "on" : "OFF",
                g_gap_fill == 0   ? "solid"
                : g_gap_fill == 2 ? "custom"
                                  : "ambient",
                g_gap_color & 0xffffffu, g_gap_peek ? "ON" : "off", g_volume,
                g_mouse_capture ? "ON" : "off", g_name_tags ? "on" : "OFF",
                g_adventure_ghosts ? "ON" : "off");
    }
    return changed;
}

}  /* namespace */

extern "C" int host_setting_swap_camera_turn(void)
{
    load_once();
    return g_swap_camera_turn;
}

extern "C" int host_camera_turn_sign(void)
{
    load_once();
    return g_swap_camera_turn ? 1 : -1;
}

/* RunMode: 0 button, 1 analog, 2 auto. See the block above load_once for
   what each one is and why the DS did not need any of them. */
extern "C" int host_setting_run_mode(void)
{
    load_once();
    return g_run_mode;
}

/* The keyboard virtual-key code and the XInput button mask bound to running.
   Zero on either one means that device has no run binding. */
extern "C" int host_setting_run_key(void)
{
    load_once();
    return g_run_key;
}

extern "C" int host_setting_run_pad(void)
{
    load_once();
    return g_run_pad;
}

/* CameraMode: 0 analog, 1 freecam, 2 ds. See the header. */
extern "C" int host_setting_camera_mode(void)
{
    load_once();
    return g_camera_mode;
}

/* The bindings. An index outside the table is 0, unbound, so a caller that
   grew an action before this file did gets "nothing" and not a wild read. */
extern "C" int host_setting_key(int action)
{
    load_once();
    if (action < 0 || action >= 14) return 0;
    return g_key[action];
}

extern "C" int host_setting_pad(int action)
{
    load_once();
    if (action < 0 || action >= 6) return 0;
    return g_pad[action];
}

extern "C" const char *host_setting_key_name(int action)
{
    if (action < 0 || action >= 14) return "";
    return KEY_BIND[action].name;
}

extern "C" const char *host_setting_pad_name(int action)
{
    if (action < 0 || action >= 6) return "";
    return PAD_BIND[action].name;
}

/* The four screen-gap keys. See the block above load_once for what each one
   means; hal/screen_gap.cpp is the only reader. */
extern "C" int host_setting_minigame_gap(void)
{
    load_once();
    return g_gap_on;
}

extern "C" int host_setting_gap_fill_mode(void)
{
    load_once();
    return g_gap_fill;
}

extern "C" unsigned host_setting_gap_color(void)
{
    load_once();
    return g_gap_color;
}

extern "C" int host_setting_gap_peek(void)
{
    load_once();
    return g_gap_peek;
}

/* GaplessMinigames. Not one of the four: this one changes the GAME, and only
   for a scene the table in hal/screen_gap.cpp names. See the block over
   g_gapless_minigames and port/hal/host_settings.h. */
extern "C" int host_setting_gapless_minigames(void)
{
    load_once();
    return g_gapless_minigames;
}

/* LovesMeCharacter: 0 Yoshi (the ROM), 1 Mario. Boot-latched like every Mods
   key; hal/fs_mods.cpp is the only reader. */
extern "C" int host_setting_lovesme_character(void)
{
    load_once();
    return g_lovesme_character;
}

/* CustomPalette: 0 the ROM's colors, 1..3 a combo file. Boot-latched like
   every Mods key; hal/fs_mods.cpp is the only reader. */
extern "C" int host_setting_custom_palette(void)
{
    load_once();
    return g_custom_palette;
}

/* The per-character picker. Returns the value AS WRITTEN, never null: "" is
   the ROM. An out-of-range character is "" as well, so a caller that grew a
   fifth character before this file did gets the ROM and not a wild read.
   Boot-latched like every Mods key; hal/fs_mods.cpp is the only reader. */
extern "C" const char *host_setting_character_palette(int character)
{
    load_once();
    if (character < 0 || character > 3) return "";
    return g_char_palette[character];
}

/* 1 when the player set any of the four. This is what makes CustomPalette
   stand down: see the header. */
extern "C" int host_setting_character_palette_any(void)
{
    load_once();
    for (int i = 0; i < 4; ++i)
        if (g_char_palette[i][0]) return 1;
    return 0;
}

/* Which of the ROM's four-player rows PaletteYoshi named, 0..3 in the order
   they are stacked in yoshi_all_16p_pl (green, red, blue, yellow), or -1
   when the key names a file instead. The spellings live in this file and
   the meaning of the row lives in hal/fs_mods.cpp; see YOSHI_ROW_KEY. */
extern "C" int host_setting_yoshi_builtin_row(void)
{
    load_once();
    return g_yoshi_row;
}

extern "C" int host_setting_volume(void)
{
    load_once();
    return g_volume;
}

/* MouseCapture: 1 when the window may hold the pointer. Only ever a
   PERMISSION -- the window decides whether to take it this frame, and the
   header lists the seven places where the answer is no however this reads. */
extern "C" int host_setting_mouse_capture(void)
{
    load_once();
    return g_mouse_capture;
}

/* NameTags, plus the debug env override every host overlay in this port has.
   SM64DS_NAME_TAGS=0 forces them off and any other value forces them on, so a
   proof run can pin the answer without editing a player's file; unset is the
   file's answer, live re-read included. Read once, like every other env in
   this file's neighbourhood. */
extern "C" int host_setting_name_tags(void)
{
    static int env = -2;
    if (env == -2) {
        const char *e = getenv("SM64DS_NAME_TAGS");
        env = e ? ((e[0] == '0' && e[1] == 0) ? 0 : 1) : -1;
    }
    if (env >= 0) return env;
    load_once();
    return g_name_tags;
}

/* AdventureGhosts, plus the debug env override every host toggle in this file
   has. SM64DS_ADVENTURE=0 forces the mode off and any other value forces it
   on, so a proof run can pin the answer without editing a player's file; unset
   is the file's answer, live re-read included. Read once, like every other env
   in this file's neighbourhood. */
extern "C" int host_setting_adventure_ghosts(void)
{
    static int env = -2;
    if (env == -2) {
        const char *e = getenv("SM64DS_ADVENTURE");
        env = e ? ((e[0] == '0' && e[1] == 0) ? 0 : 1) : -1;
    }
    if (env >= 0) return env;
    load_once();
    return g_adventure_ghosts;
}

/* The C++ predicate the header promises. One reader, so the ghost render and
   no-collision paths cannot drift from the accessor's answer. */
namespace port {
bool adventure_ghost_mode()
{
    return host_setting_adventure_ghosts() != 0;
}
}  // namespace port

/* ---- THE VOICE ACCESSORS, lane VOICE ---------------------------------- */
extern "C" int host_setting_voice_enabled(void)
{
    load_once();
    return g_voice_enabled;
}

extern "C" int host_setting_voice_volume(void)
{
    load_once();
    return g_voice_volume;
}

/* Never null. "" means the system's default recording device, which is what
   an absent key, an empty key and a key too long for the buffer all mean. */
extern "C" const char *host_setting_voice_mic_device(void)
{
    load_once();
    return g_voice_mic;
}

extern "C" int host_setting_voice_near_radius(void)
{
    load_once();
    return g_voice_near;
}

extern "C" int host_setting_voice_far_radius(void)
{
    load_once();
    return g_voice_far;
}

extern "C" int host_settings_gen(void)
{
    return g_setgen;
}

/* See the header. The steady-state cost is one counter compare; the file's
   write time is asked for every 30th call, and the file is read only when
   that time (or the size) moved. A read that fails or parses as garbage
   changes NOTHING -- the next poll simply tries again -- so a torn write
   can delay a change but never invent one. */
extern "C" int host_settings_poll(void)
{
#ifdef _WIN32
    static int tick;
    if (++tick < 30) return 0;
    tick = 0;
    load_once();

    char path[1024];
    if (!find_settings(path, sizeof path)) return 0;
    WIN32_FILE_ATTRIBUTE_DATA fa;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &fa)) return 0;
    const unsigned long long t =
        ((unsigned long long)fa.ftLastWriteTime.dwHighDateTime << 32) |
        fa.ftLastWriteTime.dwLowDateTime;
    const unsigned long long sz =
        ((unsigned long long)fa.nFileSizeHigh << 32) | fa.nFileSizeLow;
    if (t == g_watch_time && sz == g_watch_size) return 0;
    g_watch_time = t;
    g_watch_size = sz;

    char *text = slurp(path);
    if (!text) return 0;
    int changed = 0;
    if (looks_like_json_object(text)) changed = reload_live(text);
    free(text);
    if (changed) ++g_setgen;
    return changed;
#else
    return 0;
#endif
}

/* Take the three values and PERSIST them, so a choice made in the debug menu
   is still there after a restart -- which is the whole reason this is a
   settings key and not a runtime toggle. Returns 1 when the file on disk now
   says so.

   The in-memory values move whether or not the write lands: a player who
   changed the mode is owed the change for the rest of this run even if the
   folder is read-only, and the failure is on stderr rather than in their
   face. Every key this program did not write is carried across untouched
   (see json_set). */
/* run mg16 lane MP2: A KNOWN, DELIBERATE LIMITATION WITH TWO INSTANCES.
   This is a READ-MODIFY-WRITE over a file both copies of the game share: it
   reloads, edits three keys and rewrites the WHOLE document. So if P1 changes
   its volume and then P2 saves anything, P2 rewrites the file from the state it
   loaded and P1's change is gone -- LAST WRITER WINS, silently.
   That is left as it is on purpose. Giving each instance its own settings.json
   would mean the launcher's own keys stop being shared, which is the wrong
   trade for a file whose whole job is to carry launcher settings into the game.
   The TEMP-FILE interleave that used to sit underneath this is a different
   thing and IS fixed, in write_text above.
   hal/instance_tag.h's survey carries the same note for a reader who arrives
   from the other direction. */
/* The shared write: settings.json reloaded, `n` keys set in order, the whole
   document published through write_text. Every key this program did not
   write is carried across untouched (see json_set). Says why on stderr when
   the write fails; the in-memory values are the caller's to have moved
   already. */
namespace {
int save_keys(const char *const *keys, const char *const *vals, int n,
              const char *what)
{
    char path[1024];
    if (!settings_write_path(path, sizeof path)) return 0;

    char *doc = slurp(path);
    if (doc && !looks_like_json_object(doc)) {
        /* a file that will not parse is one the loader is already ignoring,
           so replacing it loses nothing a reader was going to honour */
        free(doc);
        doc = 0;
    }
    if (!doc) {
        doc = (char *)malloc(4);
        if (!doc) return 0;
        memcpy(doc, "{\n}", 4);
    }
    int ok = 1;
    for (int i = 0; i < n && ok; ++i) {
        char *next = json_set(doc, keys[i], vals[i]);
        free(doc);
        doc = next;
        if (!doc) ok = 0;
    }
    if (ok) ok = write_text(path, doc);
    free(doc);
    if (!ok)
        fprintf(stderr, "[settings] could not write %s -- the %s is set for "
                        "this run only\n", path, what);
    return ok;
}
}  /* namespace */

extern "C" int host_setting_save_run(int mode, int key, int pad)
{
    load_once();
    if (mode < 0 || mode > 2) mode = 0;
    if (key < 0 || key > 0xff) key = 0;
    if (pad < 0 || pad > HOST_PAD_MASK_MAX) pad = 0;
    g_run_mode = mode;
    g_run_key = key;
    g_run_pad = pad;
    g_key[11] = key;
    g_pad[3] = pad;

    /* BOTH spellings of each binding, so a launcher that reads the old name
       and one that reads the new name both see the choice, and the loader's
       "KeyRun wins when present" rule cannot resurrect a stale old value. */
    char vmode[24], vkey[24], vpad[24];
    snprintf(vmode, sizeof vmode, "\"%s\"", RUN_MODE_KEY[mode]);
    snprintf(vkey, sizeof vkey, "%d", key);
    snprintf(vpad, sizeof vpad, "%d", pad);
    const char *const keys[5] = { "RunMode", "RunButtonKey", "KeyRun",
                                  "RunButtonPad", "PadRun" };
    const char *const vals[5] = { vmode, vkey, vkey, vpad, vpad };
    return save_keys(keys, vals, 5, "run mode");
}

/* The camera row's twin of the above: the live mode moves whether or not the
   write lands, and the file carries it to the next boot. */
extern "C" int host_setting_save_camera_mode(int mode)
{
    load_once();
    if (mode < 0 || mode > 2) mode = 0;
    g_camera_mode = mode;
    char v[24];
    snprintf(v, sizeof v, "\"%s\"", CAMERA_MODE_KEY[mode]);
    const char *const keys[1] = { "CameraMode" };
    const char *const vals[1] = { v };
    return save_keys(keys, vals, 1, "camera mode");
}

/* ---- PadLayouts -----------------------------------------------------------
   See the header. The table is what load_once parsed plus whatever the learn
   flow saved this run. */
extern "C" void host_pad_layout_default(HostPadLayout *out)
{
    padlayout_default(out);
}

extern "C" int host_setting_pad_layout(int vid, int pid, HostPadLayout *out)
{
    load_once();
    for (int i = 0; i < g_padlayout_n; ++i)
        if (g_padlayouts[i].vid == vid && g_padlayouts[i].pid == pid) {
            if (out) *out = g_padlayouts[i];
            return 1;
        }
    return 0;
}

extern "C" int host_setting_pad_layout_count(void)
{
    load_once();
    return g_padlayout_n;
}

extern "C" int host_setting_pad_layout_at(int i, HostPadLayout *out)
{
    load_once();
    if (i < 0 || i >= g_padlayout_n) return 0;
    if (out) *out = g_padlayouts[i];
    return 1;
}

extern "C" int host_setting_save_pad_layout(const HostPadLayout *layout)
{
    load_once();
    if (!layout || layout->vid < 1 || layout->vid > 0xffff ||
        layout->pid < 1 || layout->pid > 0xffff)
        return 0;
    int slot = g_padlayout_n;
    for (int i = 0; i < g_padlayout_n; ++i)
        if (g_padlayouts[i].vid == layout->vid && g_padlayouts[i].pid == layout->pid)
            slot = i;
    if (slot >= HOST_PAD_LAYOUT_MAX) {
        fprintf(stderr, "[settings] PadLayouts is full (%d pads); the layout "
                        "is set for this run only\n", HOST_PAD_LAYOUT_MAX);
        return 0;
    }
    /* clamp through the same table the parser uses, so what the file gets
       is what a reader will accept */
    HostPadLayout o = *layout;
    HostPadLayout d;
    padlayout_default(&d);
    for (int f = 0; f < 22; ++f) {
        const int v = o.*PAD_FIELDS[f].f;
        const int is_sign = PAD_FIELDS[f].lo == -1 && PAD_FIELDS[f].hi == 1;
        if (v < PAD_FIELDS[f].lo || v > PAD_FIELDS[f].hi || (is_sign && v == 0))
            o.*PAD_FIELDS[f].f = d.*PAD_FIELDS[f].f;
    }
    o.name[sizeof o.name - 1] = '\0';
    g_padlayouts[slot] = o;
    if (slot == g_padlayout_n) ++g_padlayout_n;

    char *arr = padlayouts_text();
    if (!arr) return 0;
    const char *const keys[1] = { "PadLayouts" };
    const char *const vals[1] = { arr };
    const int ok = save_keys(keys, vals, 1, "pad layout");
    free(arr);
    return ok;
}
