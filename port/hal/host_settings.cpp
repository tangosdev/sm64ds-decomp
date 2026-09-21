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

/* Publish one environment variable for a child of this process to read with
   getenv. The one use is the VsLuigiInfection bridge (load_once): the launcher
   writes the mode into settings.json and this hands it to the game-side module
   that arms off SM64DS_VS_LUIGI_INFECTION, so the two lanes share one flag and
   need no header between them. _putenv_s / setenv both take a copy, so the
   pointers here need not outlive the call. */
void port_setenv(const char *name, const char *value)
{
#ifdef _WIN32
    _putenv_s(name, value);
#else
    setenv(name, value, 1);
#endif
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

/* THE ONE PLACE THE ASPECT BAND IS SPELLED OUT. Both readers of the key -- the
   settings file and the SM64DS_ASPECT override -- come through here, so the file
   and the environment cannot disagree about what 9.0 means.

     not a positive number   -> 0, the explicit native 4:3 sentinel. Covers
                                absent, unparseable, 0 itself, negatives and NaN
                                (the !(a > 0) spelling catches NaN; the test
                                written the other way round would not).
     positive                -> clamped into [1.0, 4.0]. 1.0 is square; below
                                1.0 is a portrait picture the HUD has no layout
                                for.

                                THE CEILING WAS 3.0 AND 3.0 WAS WRONG. It was
                                picked as "wider than any shipping monitor",
                                and 32:9 monitors exist: 32:9 is 3.5555556, so
                                the old ceiling took a super-ultrawide panel's
                                own ratio and quietly letterboxed it to 3.0.
                                4.0 is still a picture -- the DS's vertical
                                field is a slot long before the arithmetic
                                stops working -- and the arithmetic's own limit
                                is higher still and lives elsewhere: the
                                stacked sub-screen presentation scales by the
                                INTEGER active_h / 192 (ntr/ppu_sub.cpp), which
                                must stay at least 1, so no ceiling above
                                1024/192 = 5.33 is safe at all. 4.0 covers
                                32:9 with room to spare and leaves that scale
                                at 1. */
double aspect_sanitise(double a)
{
    if (!(a > 0.0)) return 0.0;
    if (a < 1.0) return 1.0;
    if (a > 4.0) return 4.0;
    return a;
}

/* Find `key` used as an object KEY and read a DECIMAL FRACTION after it. The
   same contract as json_int one function up -- dflt unless a number is really
   there -- but the value is a ratio, so it cannot be an int. Only reader is the
   Aspect key. strtod is locale-sensitive in principle; this process never calls
   setlocale, so the C locale's '.' is the separator, which is what a settings
   file written by the launcher contains. */
double json_num(const char *s, const char *key, double dflt)
{
    const char *v = json_value(s, key);
    if (!v) return dflt;
    if (*v == '"') ++v;              /* "1.7777778" is as good as 1.7777778 */
    {
        const char *p = v;
        if (*p == '-' || *p == '+') ++p;
        /* a leading '.' is input a player might plausibly type */
        if ((*p < '0' || *p > '9') && *p != '.') return dflt;
    }
    {
        char *end = 0;
        const double d = strtod(v, &end);
        if (end == v) return dflt;   /* nothing consumed: unparseable */
        return d;
    }
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

     RunMode         "button" | "analog" (default) | "auto"
     RunButtonKey    Win32 virtual-key code for the keyboard binding.
                     Default 0x10, which is shift -- what the window has
                     always used. 0 means no keyboard binding at all.
     RunButtonPad    XInput button mask for the pad binding. Default 0x4000,
                     which is X on an Xbox layout -- again what the window
                     has always used. 0 means no pad binding.

   A missing file, a missing key or a value that will not parse is the
   default. The bindings and the keys are unchanged from what this window has
   always used; the MODE now defaults to analog on Tango's order. A keyboard
   has no stick to push, so with no pad (or the stick in its dead zone) it
   falls back to button mode: WASD walk, the bound run key holds the run bit,
   exactly as before. */
int g_run_mode;                      /* default RUN_ANALOG (1) */
int g_run_key;                       /* default 0x10, VK_SHIFT */
int g_run_pad;                       /* default 0x4000, pad X */

const char *const RUN_MODE_KEY[3] = { "button", "analog", "auto" };

/* ---- CameraMode -----------------------------------------------------------
   0 analog, 1 freecam, 2 ds -- tests/walk_window.cpp's CAM_ numbering. The
   default is ds, on Tango's order, because that is the cartridge's own
   stepped rotate and it is the mode the bumpers turn in; analog and freecam
   stay one F1 press (or one menu row) away. The header carries the rest.
   Read by name and, like RunMode, by number too. */
int g_camera_mode;                   /* default 2, ds */
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
int g_net_mode = 1;   /* NetMode: 0 lockstep, 1 rollback (port/rollback); rollback is the default again (0.3.7: the 0.3.6 lockstep hotfix is reverted); sessions past kRollbackMaxPlayers still fall back to lockstep regardless */

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

/* Aspect: the presentation ratio, WIDTH DIVIDED BY HEIGHT. 0 (the default) is
   the explicit native sentinel -- the DS's own 4:3 at the 512x384 window the
   port has always opened. 1.7777778 is 16:9. It is a NUMBER and not a
   Widescreen boolean for one reason that matters and one that follows from it:
   a boolean cannot express ultrawide, and a number means an odd monitor needs
   no new mode name, only its own ratio.

   ACCEPTED: 0 for native, or a ratio CLAMPED TO [1.0, 4.0]. Absent,
   unparseable, negative or otherwise not a positive number all read as 0, so a
   file written before this key existed -- or half-edited by hand -- lands on the
   shipped 4:3 look like every other key here. A positive number outside the band
   is clamped rather than rejected: 9.0 becomes 4.0, which is a picture, where
   rejecting it would be a surprise. The ceiling is 4.0 and not 3.0 because
   32:9 is 3.5555556 and those monitors are real; see aspect_sanitise for why
   4.0 and not higher.

   BOOT-LATCHED, not read live: the framebuffer aspect is chosen once, at boot,
   and threaded through the whole render path (ntr::configure_aspect), so unlike
   NameTags/AdventureGhosts a mid-run change cannot take. The only reader is
   walk_window's boot, which calls ntr::configure_aspect(host_setting_aspect()). */
double g_aspect = 0.0;

/* VsLuigiInfection and VsLuigiInfectionSeconds. Default off / ROM clock, the
   normal VS match. See the header: this key is the launcher's channel to the
   game-side Luigi Infection module, which arms off SM64DS_VS_LUIGI_INFECTION;
   load_once bridges the file to that variable when the mode is on and the
   player has not set it. */
int g_vs_luigi_infection = 0;
int g_vs_luigi_seconds = 0;

/* ---- PROXIMITY VOICE CHAT, lane VOICE ------------------------------------
   Six keys, all of them host preferences and none of them a mod: nothing
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
     VoiceMicDevice   "" (default), and the literal "auto", mean AUTO-PICK
                      rather than "the Windows default device". The capture
                      backend opens each recording device in turn for a
                      fraction of a second, measures the peak sample, and
                      takes the first one that is actually producing audio;
                      if every device measures silent it falls back to the
                      Windows default and says so. The default moved from
                      "Windows default device" to "auto" because a device can
                      open perfectly and capture nothing -- muted in Windows,
                      blocked by the privacy gate, or simply the wrong device
                      left as the system default -- and there is no way to
                      tell that apart from a working microphone without
                      recording from it. That case is a silent voice channel
                      with no error line anywhere, which is the worst failure
                      shape this feature has. The literal "default" (or
                      "system") forces the Windows default device with no
                      scan. Any other value is matched, case-insensitively and
                      as a SUBSTRING, against the names winmm reports for the
                      machine's recording devices; the first match wins, the
                      scan is skipped, and no match falls back to the default
                      device with one line on stderr. Substring rather than
                      exact because the names winmm reports are truncated to
                      31 characters and a launcher listing them cannot always
                      show a player the whole thing. RELOADS LIVE: a changed
                      name reopens the device.
     VoiceMicIndex    -1 (default) means "no index given" and leaves
                      VoiceMicDevice in charge. 0 or above is a winmm device
                      id used VERBATIM -- no scan, no name match, no silent
                      fall back to the default if it will not open -- and it
                      OUTRANKS VoiceMicDevice when both are set. It exists for
                      the machine whose two devices report the same truncated
                      31-character name, which a substring match cannot tell
                      apart. RELOADS LIVE: a changed index reopens the device.
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
char g_voice_mic[96];                /* "" = auto-pick, see above */
int  g_voice_mic_index = -1;         /* -1 = no index given; >= 0 is verbatim */
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
    /* -1 is BOTH the default and the "absent key" answer, so a file without
       the key and a file that spells it -1 land in the same place: no index
       given, VoiceMicDevice decides. Any negative value reads as -1 rather
       than being carried through, because a negative device id is not a
       choice anybody made and winmm has no meaning for one. */
    int mic_idx = json_int(text, "VoiceMicIndex", g_voice_mic_index);
    if (mic_idx < 0) mic_idx = -1;
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
    if (mic_idx != g_voice_mic_index) { g_voice_mic_index = mic_idx; changed = 1; }
    if (strcmp(mic, g_voice_mic) != 0) {
        strncpy(g_voice_mic, mic, sizeof g_voice_mic - 1);
        g_voice_mic[sizeof g_voice_mic - 1] = '\0';
        changed = 1;
    }
    return changed;
}

/* ---- FrameRate: THE ONE PLACE THE PRESENTATION BAND IS SPELLED OUT -------
   Both readers of the key -- settings.json and the SM64DS_FRAME_RATE override
   -- come through here, so the file and the environment cannot disagree about
   what 1000 or 45 means.

     not a positive number  -> 0, the explicit native sentinel: one picture per
                               game tick, the port's shipped behaviour. Covers
                               absent, unparseable, 0 itself and negatives.
     1..59                  -> 0 as well. 60 is the FLOOR because 60 is the
                               fastest the ROM's own clock ever runs
                               (data_0208ee44 == 1, every minigame), so a
                               smaller number would be asking the port to
                               present less often than the game ticks. That is
                               not a presentation choice, it is a slower game,
                               and this key does not do that.
     60..240                -> that many pictures a second.
     above 240              -> clamped to 240, the Aspect rule: a number that
                               is a picture beats an error. */
int frame_rate_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n < 60) return 0;
    if (n > 240) return 240;
    return n;
}

/* The primary display's refresh rate, once, for the key's "display" spelling.
   user32 is HAND-LOADED rather than imported, the rule this whole port follows
   (port/hal/pad_backend.cpp and hal/asset_root_refuse.cpp do the same): a
   static import table maps over 0x02000000 and the ROM's own address space
   lives there. A display that reports 0 or 1 (the "driver default" answer
   EnumDisplaySettings is allowed to give) is no answer, so it reads as 0 and
   the sanitiser turns that into native. */
int display_refresh_hz(void)
{
#ifdef _WIN32
    typedef BOOL (WINAPI *EnumDisplaySettingsA_t)(LPCSTR, DWORD, DEVMODEA *);
    static int hz = -1;
    if (hz >= 0) return hz;
    hz = 0;
    if (HMODULE u = LoadLibraryA("user32.dll")) {
        EnumDisplaySettingsA_t f =
            (EnumDisplaySettingsA_t)GetProcAddress(u, "EnumDisplaySettingsA");
        if (f) {
            DEVMODEA dm;
            memset(&dm, 0, sizeof dm);
            dm.dmSize = sizeof dm;
            if (f(0, ENUM_CURRENT_SETTINGS, &dm) && dm.dmDisplayFrequency > 1)
                hz = (int)dm.dmDisplayFrequency;
        }
    }
    return hz;
#else
    return 0;
#endif
}

/* FrameRate: the presentation rate in pictures a second, 0 for native (one
   picture per game tick). BOOT-LATCHED like g_aspect and for the same kind of
   reason: the presentation clock's shape is settled at the pacer's first turn
   and a mid-run change would be a second code path nobody tests. The only
   reader is walk_window's pacer. See the header for the whole contract. */
int g_frame_rate = 0;

/* ---- THE THREE PICTURE-QUALITY KEYS' BANDS (run hd1) ---------------------
   One sanitiser each, and both readers of every key -- settings.json and the
   environment override -- come through it, so the file and the environment
   cannot disagree about what 9 or -1 means. The shape is FrameRate's exactly.

     RenderScale   not a positive number -> 0, the explicit default sentinel:
                   the multiplier the port picks today. Covers absent,
                   unparseable, 0 itself and negatives. 1..4 are themselves.
                   Above 4 clamps to 4 rather than being refused, the Aspect
                   rule: a number that is a picture beats an error. The
                   ceiling is 4 because 4 x 192 is 768 and 4 x 256 is 1024,
                   which is the largest tier the render path has ever been
                   compiled at (ntr/ppu.h's NTR_HIRES), so it is the largest
                   one with any evidence behind it.
     SmoothModels  the same, clamped into 0..3, which is the range the
                   subdivision itself is defined over (ntr/smooth.h). */
int render_scale_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n > 4) return 4;
    return n;
}

int smooth_models_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n > 3) return 3;
    return n;
}

/* ---- THE TWO PICTURE-SMOOTHING KEYS' BANDS (run hd2) --------------------
   The same shape again, one sanitiser each, both readers through it.

     TextureFilter  0 nearest (the sampler the port has always used), 1
                    bilinear, 2 trilinear. Absent, unparseable, 0 itself and
                    negative all land on 0; above 2 clamps to 2 rather than
                    being refused, the Aspect rule. The ceiling is 2 because
                    trilinear is the last mode the raster has a chain for:
                    anisotropic filtering would need a per-pixel footprint
                    the sampler does not compute.
     AntiAliasing   0 off, 1 edge smoothing. Same band, ceiling 1, because 1
                    is the only mode measured on this renderer. */
int texture_filter_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n > 2) return 2;
    return n;
}

int anti_aliasing_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n > 1) return 1;
    return n;
}

/* ---- THE THREE PRESENT KEYS' BANDS (run hd2, lane GPU1) -----------------
   The same shape once more, one sanitiser each, both readers through it.

     PresentBackend  0 the ordinary Windows path the port has always used
                     (StretchDIBits), 1 Direct3D 11. Absent, unparseable, 0
                     itself and negative all land on 0; above 1 clamps to 1
                     rather than being refused, the Aspect rule. The ceiling
                     is 1 because Direct3D 11 is the only other path that
                     exists.
     PresentFilter   0 nearest (the whole-pixel drop-and-duplicate the port
                     has always presented with), 1 smooth, 2 sharp. Ceiling 2
                     because sharp is the last mode the present shader has.
                     READ ONLY WHEN THE BACKEND IS 1; the Windows path has
                     its own filter knob.
     AntiAliasing's band shape exactly for VSync: 0 off, 1 on, ceiling 1.
     READ ONLY WHEN THE BACKEND IS 1, because the Windows path has no vsync
     of any kind to switch on. */
int present_backend_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n > 1) return 1;
    return n;
}

int present_filter_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n > 2) return 2;
    return n;
}

int vsync_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n > 1) return 1;
    return n;
}

/* ---- THE RENDERER KEY'S BAND (run hd2, lane GPU2) -----------------------
     Renderer   0 the software rasteriser the port has always drawn the 3D
                picture with, which is the default and the only byte-exact
                reference; 1 Direct3D 11, which draws the opaque 3D pass on
                the graphics card and reads it back. Absent, unparseable, 0
                itself and negative all land on 0; above 1 clamps to 1 rather
                than being refused, the Aspect rule. The ceiling is 1 because
                Direct3D 11 is the only other renderer that exists.
     It is INDEPENDENT of PresentBackend: either can be on without the other,
     and with both on the process still makes exactly one device. */
int renderer_sanitise(int n)
{
    if (n <= 0) return 0;
    if (n > 1) return 1;
    return n;
}

/* The three keys' stored values. BOOT-LATCHED like g_aspect and g_frame_rate
   and for the same kind of reason: the render size is threaded into the
   framebuffer, the pack is opened once and the subdivision level sizes
   per-polygon work, so all three are settled before the first frame and a
   mid-run change would be a second code path nobody tests. */
int g_render_scale = 0;
int g_hd_textures = 0;
int g_smooth_models = 0;

/* run hd2's two, latched beside them and for the same kind of reason: the
   filter mode decides whether the texture cache builds a mip chain at the
   first bind of each texture, and the smoothing pass sizes a scratch buffer
   the width of the picture, so both are settled before the first frame. */
int g_texture_filter = 0;
int g_anti_aliasing = 0;

/* run hd2 lane GPU1's three, latched beside them and for the same kind of
   reason: which backend presents the picture decides which library is loaded
   and which device exists, and the filter sizes nothing but is read at the
   same moment, so all three are settled before the first picture is handed
   over and a mid-run change would be a second code path nobody tests. */
int g_present_backend = 0;
int g_present_filter = 0;
int g_vsync = 0;

/* ---- THE IMPROVED MINIMAP'S TWO KEYS ---------------------------------------

   MinimapScale is a multiplier on the panel the port draws TODAY (half a DS
   screen, so 128x96), which is what "scaled from how it is" means: 1 is the
   current picture, byte for byte.

   IT IS A FREE NUMBER NOW, not one of six rows, because the owner asked to
   drag the map's corner and scale it against the window by hand. The launcher
   keeps its 1 / 1.25 / 1.5 / 2 / 3 / 4 picker and every one of those six is
   still exact; a number between them is just as legal, and the picker's own
   round trip shows the nearest row for one the player dragged to.

   THE ONE THING A SIZE HAS TO BE is a whole number of pixels in both axes.
   The map is drawn from a 256x192 source and keeps its 4:3 shape, so its
   drawn width k must be a multiple of four for 3k/4 to be whole. Every size
   is therefore quantised onto that grid -- steps of four pixels of width,
   three of height, one thirty-second of a multiplier -- and the six picker
   rows land on it exactly: 128, 160, 192, 256, 384 and 512.

   The ratio handed to the compose and to the stylus inverse is k/256, which
   for those six reduces to the 1/2, 5/8, 3/4, 1/1, 3/2 and 2/1 they used
   before and draws the identical picture. */
int g_improved_minimap = 1;   /* ABSENT MEANS ON: the owner's order */

/* SaveMenuOnTop: while the level-clear save menu is up, the two DS screens
   trade places -- the menu becomes the big picture and the course-clear tally
   drops into the corner. ABSENT MEANS ON, the second key in this file with
   that default and for the same reason the improved minimap has it: it is a
   mod the owner asked for and recommended on. 0 is the picture the port drew
   before it existed. */
int g_save_menu_on_top = 1;   /* ABSENT MEANS ON: the owner's recommendation */
double g_minimap_scale = 1.0; /* the multiplier, 1 or more */
/* A HAND HAS MOVED THE SIZE THIS RUN. Set by the drag's own write and by
   nothing else; read in host_setting_minimap_scale_value, where the block
   above that function says what it is for. */
int g_minimap_scale_dragged;

/* THE MAP'S DRAWN WIDTH for a multiplier, on the grid above. Absent, zero,
   negative and unparseable all read as 1, the Aspect rule: a number that is a
   picture beats an error. The ceiling is a sanity bound only -- the real
   limit is "the whole panel still fits in the picture" and only the drawing
   layer knows the picture. */
int minimap_scale_k(double v)
{
    if (!(v > 0.0)) v = 1.0;
    int k = (int)(v * 128.0 + 0.5);
    k = (k + 2) & ~3;
    if (k < 128) k = 128;
    if (k > 4096) k = 4096;
    return k;
}

/* Put a multiplier on the grid, and say through `snapped` whether it had to
   move, so the one caller that wants to say so can say it once rather than
   every reader printing a line. */
double minimap_scale_sanitise(double v, int *snapped)
{
    const double q = minimap_scale_k(v) / 128.0;
    if (snapped) *snapped = (v != q) ? 1 : 0;
    return q;
}

/* run hd2 lane GPU2's one, latched beside them and for the same kind of
   reason: which rasteriser draws the frame decides which library is loaded,
   which device exists and which buffers are allocated on the card, so it is
   settled before the first frame and a mid-run change would be a second code
   path nobody tests. */
int g_renderer = 0;

void load_once(void)
{
    if (g_loaded) return;
    g_loaded = 1;
    g_swap_camera_turn = 0;
    g_run_mode = 1;      /* RunMode analog, on Tango's order */
    g_run_key = 0x10;
    g_run_pad = 0x4000;
    g_camera_mode = 2;   /* CameraMode ds, on Tango's order */
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
    g_aspect = 0.0;
    g_vs_luigi_infection = 0;
    g_vs_luigi_seconds = 0;
    for (int i = 0; i < 4; ++i) g_char_palette[i][0] = '\0';
    g_yoshi_row = -1;
    g_padlayout_n = 0;
    g_voice_enabled = 0;
    g_voice_volume = 80;
    g_voice_mic[0] = '\0';
    g_voice_mic_index = -1;
    g_voice_near = 512;
    g_voice_far = 3072;
    /* NetMode: rollback by default again (0.3.7, 2026-09-04; reverts the 0.3.6
       hotfix that fell back to lockstep). Rollback predicts each peer and
       re-simulates the world on a wrong guess -- a button/stick change from a
       remote peer forces a rollback that frame -- while lockstep never
       predicts and so never re-sims. Set HERE, with the other defaults, so a
       missing file and a file that will not parse both land on it; the parse
       below only ever moves it to lockstep. */
    g_net_mode = 1;
    /* FrameRate: native, one picture per game tick, read here with the other
       defaults so a missing file and a file that will not parse both land on
       the behaviour the port shipped with. */
    g_frame_rate = 0;
    /* The three picture-quality keys, defaulted here beside FrameRate for its
       reason: a missing file and a file that will not parse both have to land
       on the picture the port shipped with, and these are the values that say
       so -- the render multiplier the port picks for itself, the ROM's own
       textures and the ROM's own geometry. */
    g_render_scale = 0;
    g_hd_textures = 0;
    g_smooth_models = 0;
    /* run hd2's two, here for the same reason: nearest sampling and no
       smoothing pass are the picture the port shipped with, so a missing file
       and a file that will not parse both have to land on them. */
    g_texture_filter = 0;
    g_anti_aliasing = 0;
    /* run hd2 lane GPU1's three, here for the same reason: the ordinary
       Windows present path, its own nearest scaler and no vsync are what the
       port has always shipped, so a missing file and a file that will not
       parse both have to land on them. */
    g_present_backend = 0;
    g_renderer = 0;
    g_present_filter = 0;
    g_vsync = 0;

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
                const int n = json_int(text, "RunMode", 1);
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
                    const int n = json_int(text, "CameraMode", 2);
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
        /* the presentation aspect, read against its own default of 0 (native)
           so a file written before this key existed reads as the 4:3 window.
           Sanitised HERE rather than at the accessor, so the stored value is
           always one the render path can size a framebuffer from. Boot-latched:
           walk_window's boot is the only reader and it reads once. */
        {
            double a = json_num(text, "Aspect", 0.0);
            /* LEGACY: the boolean this key replaced. Only consulted when Aspect
               itself is absent, so a file carrying both is decided by the newer
               key and nobody has to migrate anything by hand. */
            if (!json_value(text, "Aspect") && json_bool(text, "Widescreen", 0))
                a = 16.0 / 9.0;
            g_aspect = aspect_sanitise(a);
        }
        /* VsLuigiInfection: the Luigi Infection VS mode, read against its own
           default of OFF beside the mode keys above, so a file written before
           the key existed reads as a normal VS match. VsLuigiInfectionSeconds
           is the optional test clock in whole seconds; a non-positive or absent
           value is 0, the ROM's own match clock. */
        g_vs_luigi_infection = json_bool(text, "VsLuigiInfection", 0);
        {
            const int s = json_int(text, "VsLuigiInfectionSeconds", 0);
            g_vs_luigi_seconds = s > 0 ? s : 0;
        }
        /* THE BRIDGE to the game-side module. hal/luigi_infection.cpp (its own
           lane) arms off the environment variable SM64DS_VS_LUIGI_INFECTION and
           reads SM64DS_VS_LUIGI_TIME for its test clock. When this file turns
           the mode on and the player has NOT already exported the variable,
           publish it, so the launcher's settings.json is one channel to the same
           flag the environment toggles. Env-wins, the Aspect contract: an
           environment value already set (a proof run) is left untouched. */
        if (g_vs_luigi_infection && !getenv("SM64DS_VS_LUIGI_INFECTION")) {
            port_setenv("SM64DS_VS_LUIGI_INFECTION", "1");
            if (g_vs_luigi_seconds > 0 && !getenv("SM64DS_VS_LUIGI_TIME")) {
                char secbuf[16];
                snprintf(secbuf, sizeof secbuf, "%d", g_vs_luigi_seconds);
                port_setenv("SM64DS_VS_LUIGI_TIME", secbuf);
            }
            fprintf(stderr, "[settings] VsLuigiInfection on: published "
                    "SM64DS_VS_LUIGI_INFECTION=1%s for the game side\n",
                    g_vs_luigi_seconds > 0 ? " (+SM64DS_VS_LUIGI_TIME)" : "");
        }
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
        /* NetMode (port/rollback): "rollback" (the default again as of 0.3.7,
           and what an absent key, an absent file and an unparseable file all
           read as) or "lockstep" (opt-in). Only the word "lockstep" moves it;
           anything else keeps the default, like every other reader. Read once
           at load; the transport asks at install, SM64DS_NETMODE overrides
           there, and a seated session wider than kRollbackMaxPlayers
           (host_settings.h) runs lockstep regardless. */
        {
            char nm[16];
            if (json_str(text, "NetMode", nm, sizeof nm) &&
                strlen(nm) == 8 && ieq(nm, "lockstep", 8))
                g_net_mode = 0;
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
        /* FrameRate: the presentation rate, read against its own default of 0
           (native) so a file written before this key existed reads as one
           picture per game tick. Either a whole number or the word "display",
           which is the primary display's refresh rate read once here.
           Sanitised HERE rather than at the accessor, the Aspect shape, so the
           stored value is always one the pacer can keep. */
        {
            char fr[16];
            int n;
            if (json_str(text, "FrameRate", fr, sizeof fr) &&
                strlen(fr) == 7 && ieq(fr, "display", 7))
                n = display_refresh_hz();
            else
                n = json_int(text, "FrameRate", 0);
            g_frame_rate = frame_rate_sanitise(n);
        }
        /* The three picture-quality keys, each read against its own default
           beside FrameRate and sanitised HERE rather than at the accessor --
           the Aspect shape -- so the stored value is always one the render
           path can size a framebuffer, open a pack or subdivide a polygon
           from. A file written before these keys existed reads as one that
           left all three off, which is the shipped picture. */
        g_render_scale = render_scale_sanitise(json_int(text, "RenderScale", 0));
        /* THE IMPROVED MINIMAP, and the default is the odd one in this file:
           ABSENT IS ON. Every other feature key here defaults off so a file
           written before the key existed reads as the shipped picture; this
           one is on by the owner's explicit order, so a file with no
           ImprovedMinimap line gets the improved map. Both spellings of the
           toggle, the RunMode rule: the launcher serialises a C# bool and a
           player editing by hand may write 1. */
        g_improved_minimap = (json_int(text, "ImprovedMinimap", 1) != 0 &&
                              json_bool(text, "ImprovedMinimap", 1) != 0) ? 1 : 0;
        /* THE SAVE MENU ON THE TOP SCREEN, the same absent-is-on shape and
           both spellings of the toggle, because the launcher serialises a C#
           bool and a player editing by hand may write 1. */
        g_save_menu_on_top = (json_int(text, "SaveMenuOnTop", 1) != 0 &&
                              json_bool(text, "SaveMenuOnTop", 1) != 0) ? 1 : 0;
        {
            int snapped = 0;
            const double want = json_num(text, "MinimapScale", 1.0);
            g_minimap_scale = minimap_scale_sanitise(want, &snapped);
            if (snapped)
                fprintf(stderr, "[settings] MinimapScale %g is not a whole "
                        "number of pixels in both axes -- using %g, the "
                        "nearest that is\n", want, g_minimap_scale);
        }
        /* Both spellings of a toggle, the RunMode rule: the launcher
           serialises a C# bool as true/false and a player editing by hand may
           write 1. Either says on; absent and anything else say off. */
        g_hd_textures = (json_int(text, "HdTextures", 0) != 0 ||
                         json_bool(text, "HdTextures", 0) != 0) ? 1 : 0;
        g_smooth_models =
            smooth_models_sanitise(json_int(text, "SmoothModels", 0));
        /* run hd2's two, read the same way and sanitised here rather than at
           the accessor, so the stored value is always one the sampler can
           choose a tap count from and the smoothing pass can size itself
           against. A file written before these keys existed reads as one that
           left both off, which is the shipped picture. */
        g_texture_filter =
            texture_filter_sanitise(json_int(text, "TextureFilter", 0));
        g_anti_aliasing =
            anti_aliasing_sanitise(json_int(text, "AntiAliasing", 0));
        /* run hd2 lane GPU1's three, read the same way and sanitised here
           rather than at the accessor, so the stored value is always one the
           present path can pick a backend, a sampler and a wait from. A file
           written before these keys existed reads as one that left all three
           off, which is the present path the port shipped with. Both
           spellings of the two toggles, the RunMode rule: the launcher
           serialises a C# bool as true/false and a player editing by hand may
           write 1. */
        g_present_backend = present_backend_sanitise(
            (json_int(text, "PresentBackend", 0) != 0 ||
             json_bool(text, "PresentBackend", 0) != 0) ? 1 : 0);
        g_present_filter =
            present_filter_sanitise(json_int(text, "PresentFilter", 0));
        g_vsync = vsync_sanitise((json_int(text, "VSync", 0) != 0 ||
                                  json_bool(text, "VSync", 0) != 0) ? 1 : 0);
        /* run hd2 lane GPU2's one, read the same way and sanitised here for
           the same reason. A file written before this key existed reads as one
           that left it off, which is the software rasteriser the port has
           always drawn with. Both spellings, the RunMode rule. */
        g_renderer = renderer_sanitise(
            (json_int(text, "Renderer", 0) != 0 ||
             json_bool(text, "Renderer", 0) != 0) ? 1 : 0);
    }
    free(text);

    /* Said out loud only when a setting is off its default, so an ordinary
       run's log is unchanged and a support log shows at a glance which
       non-default choices were in force. */
    if (g_swap_camera_turn)
        fprintf(stderr, "[settings] SwapCameraTurnDirection on (%s)\n", path);
    /* Off its default, so it is said once: this copy keeps the level-clear
       save menu on the corner panel instead of composing it onto the big
       picture. */
    if (!g_save_menu_on_top)
        fprintf(stderr, "[settings] SaveMenuOnTop off -- the save menu "
                "after a star stays on the corner panel (%s)\n", path);
    /* Off its default (rollback again as of 0.3.7), so it is said, and said
       in plain words: a support log for "online play feels laggy" should
       carry on one line that this copy opted back into lockstep, which waits
       for every player's input each frame instead of predicting. */
    if (!g_net_mode)
        fprintf(stderr, "[settings] NetMode lockstep -- online play waits for "
                        "every player's input each frame instead of the "
                        "default rollback (predicts each peer and rewinds on "
                        "a wrong guess). (%s)\n", path);
    if (g_run_mode != 1 || g_run_key != 0x10 || g_run_pad != 0x4000)
        fprintf(stderr, "[settings] RunMode %s key 0x%02x pad 0x%04x (%s)\n",
                RUN_MODE_KEY[g_run_mode], (unsigned)g_run_key,
                (unsigned)g_run_pad, path);
    if (g_camera_mode != 2)
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
                "index %d, volume %d, audible from %d units out to %d (%s)\n",
                g_voice_mic[0] ? g_voice_mic : "(auto-pick)",
                g_voice_mic_index, g_voice_volume, g_voice_near, g_voice_far,
                path);
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
    /* Said on its own line and in plain words, the GaplessMinigames rule: a
       support log for "it looks smoother than a DS" should carry the reason.
       And it says what it does NOT do, because the honest sentence at this rung
       is that the picture repeats. */
    if (g_frame_rate)
        fprintf(stderr, "[settings] FrameRate %d -- the finished picture is "
                        "handed to the display %d times a second instead of "
                        "once per game tick. The game still ticks at its own "
                        "rate and nothing is interpolated yet, so the same "
                        "picture repeats. This is a mod, not the game. "
                        "(%s)\n", g_frame_rate, g_frame_rate, path);
    /* The three picture-quality keys, one plain line each and only when the
       key is off its default, so an ordinary run's log is unchanged and a
       support log for "it looks different from the video" names the reason on
       one line. Each says what it changes and that it is a host setting and
       not the game. */
    if (g_render_scale)
        fprintf(stderr, "[settings] RenderScale %d -- the 3D picture is drawn "
                        "at %d host rows per DS row (%d rows; the width "
                        "follows this run's Aspect) and presented into the "
                        "same window. The game is untouched; this is how "
                        "sharp the picture is and nothing else. (%s)\n",
                g_render_scale, g_render_scale, g_render_scale * 192, path);
    if (g_hd_textures)
        fprintf(stderr, "[settings] HdTextures on -- replacement texture "
                        "images are loaded from %s where the pack has one, and "
                        "the ROM's own texture is used everywhere else. This "
                        "is a mod, not the game. (%s)\n",
                host_setting_hd_textures_dir(), path);
    if (g_smooth_models)
        fprintf(stderr, "[settings] SmoothModels %d -- the game's models are "
                        "subdivided %d level(s) before they are drawn, so the "
                        "silhouettes are rounder than the ROM's. This is a "
                        "mod, not the game. (%s)\n",
                g_smooth_models, g_smooth_models, path);
    /* run hd2's two, one plain line each and only off their default, the rule
       every key above follows. */
    if (g_texture_filter)
        fprintf(stderr, "[settings] TextureFilter %d -- textures are sampled "
                        "%s instead of one texel per pixel, so surfaces read "
                        "smoother and the texel grid stops showing up close. "
                        "This is a mod, not the game. (%s)\n",
                g_texture_filter,
                g_texture_filter >= 2
                    ? "trilinear (four texels blended, at the two texture "
                      "sizes nearest the surface's distance)"
                    : "bilinear (the four texels around the sample point, "
                      "blended)",
                path);
    if (g_anti_aliasing)
        fprintf(stderr, "[settings] AntiAliasing %d -- the edges of the 3D "
                        "picture are smoothed after it is drawn and before "
                        "anything 2D is put over it, so text and the HUD are "
                        "untouched. This is a mod, not the game. (%s)\n",
                g_anti_aliasing, path);
    /* run hd2 lane GPU1's three, one plain line each and only off their
       default, the rule every key above follows. The filter and the wait are
       said on the backend's line rather than on lines of their own, because
       with the backend off they are settings nothing reads and a support log
       that listed them as if they were in force would be lying. */
    if (g_present_backend)
        fprintf(stderr, "[settings] PresentBackend 1 -- the finished picture is "
                        "handed to the screen through the graphics card "
                        "(Direct3D 11) instead of the ordinary Windows drawing "
                        "call. Scaling %s, VSync %s. The picture itself is "
                        "drawn exactly as before; this is only how it reaches "
                        "the screen, and it falls back to the old way if the "
                        "card will not have it. (%s)\n",
                g_present_filter >= 2 ? "sharp"
                                      : (g_present_filter == 1 ? "smooth"
                                                               : "nearest"),
                g_vsync ? "on" : "off", path);
    else if (g_present_filter || g_vsync)
        fprintf(stderr, "[settings] PresentFilter %d and VSync %d are set but "
                        "PresentBackend is 0, so nothing reads them: the "
                        "ordinary Windows present path has its own scaler and "
                        "no vsync. (%s)\n", g_present_filter, g_vsync, path);
    /* run hd2 lane GPU2's one, one plain line and only off its default. It is
       a line of its own rather than a clause on the backend's, because the two
       settings are independent: either can be on without the other. */
    if (g_renderer)
        fprintf(stderr, "[settings] Renderer 1 -- the solid part of the 3D "
                        "picture is drawn by the graphics card instead of by "
                        "the processor and handed straight back, so the "
                        "see-through polygons, the shadows and everything "
                        "after them carry on exactly as before. It is not "
                        "pixel for pixel the same picture as the ordinary "
                        "renderer, and it falls back to it if the card will "
                        "not have it. (%s)\n", path);
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
                "'%s', index %d, radii %d..%d\n", g_voice_enabled ? "ON" : "off",
                g_voice_volume, g_voice_mic[0] ? g_voice_mic : "(auto-pick)",
                g_voice_mic_index, g_voice_near, g_voice_far);
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

/* NetMode: 0 lockstep (opt-in), 1 rollback (the default again, 0.3.7). See the parse above. */
extern "C" int host_setting_net_mode(void)
{
    load_once();
    return g_net_mode;
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

/* Aspect: width divided by height, 0 for the native 4:3 window. Same shape as
   host_setting_run_mode / host_setting_camera_mode above -- load_once, then the
   stored value -- with two environment overrides in front of it, because a proof
   run has to capture both aspects off ONE build without editing a player's
   settings file.

     SM64DS_ASPECT       the ratio directly, through the same sanitiser the file
                         goes through: 0 native, 1.7777778 16:9, 9.0 clamps to
                         3.0, junk reads as 0.
     SM64DS_WIDESCREEN   the LEGACY boolean, kept because it is one line and
                         because the proof scripts written before the key became
                         a number spell it that way. 0 forces native, anything
                         else forces 16:9. Only consulted when SM64DS_ASPECT is
                         unset, so the newer variable wins if both are set.

   Read once at boot -- the aspect is latched into the framebuffer and cannot
   change mid-run. */
extern "C" double host_setting_aspect(void)
{
    static int env_read = 0;
    static double env = -1.0;        /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_ASPECT");
        if (e && *e) {
            char *end = 0;
            const double d = strtod(e, &end);
            /* an unparseable override is still an override: it says "native",
               the same answer an unparseable file value gives */
            env = (end != e) ? aspect_sanitise(d) : 0.0;
        } else if ((e = getenv("SM64DS_WIDESCREEN")) != 0 && *e) {
            env = (e[0] == '0' && e[1] == 0) ? 0.0 : 16.0 / 9.0;
        }
    }
    if (env >= 0.0) return env;
    load_once();
    return g_aspect;
}

/* VsLuigiInfection: 1 when the Luigi Infection VS mode is on, else 0. Same
   shape as host_setting_adventure_ghosts -- an environment override in front of
   the file -- with the grammar hal/luigi_infection.cpp's own reader uses:
   SM64DS_VS_LUIGI_INFECTION unset is the file's answer, empty or "0" forces it
   off, any other value forces it on. So a proof run pins the mode without a
   settings.json, and the file is the launcher's channel to the same flag. Note
   load_once ALSO publishes SM64DS_VS_LUIGI_INFECTION into the environment when
   the file turns the mode on and it was unset; this accessor is the read side
   of the same answer, for a host-layer caller that wants the bool directly. */
extern "C" int host_setting_vs_luigi_infection(void)
{
    static int env = -2;
    if (env == -2) {
        const char *e = getenv("SM64DS_VS_LUIGI_INFECTION");
        env = e ? ((e[0] == 0 || (e[0] == '0' && e[1] == 0)) ? 0 : 1) : -1;
    }
    if (env >= 0) return env;
    load_once();
    return g_vs_luigi_infection;
}

/* VsLuigiInfectionSeconds: the optional test clock in whole seconds, 0 for the
   ROM's own match clock. SM64DS_VS_LUIGI_TIME overrides the file the same way,
   and a non-positive override or file value is 0. */
extern "C" int host_setting_vs_luigi_infection_seconds(void)
{
    static int env_read = 0;
    static int env = -1;             /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_VS_LUIGI_TIME");
        if (e && *e) {
            const int v = atoi(e);
            env = v > 0 ? v : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_vs_luigi_seconds;
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

extern "C" int host_setting_voice_mic_index(void)
{
    load_once();
    return g_voice_mic_index;
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

/* FrameRate: the presentation rate in pictures a second, 0 for native. Same
   shape as host_setting_aspect above -- an environment override in front of
   load_once and the stored value -- with the same grammar on both channels,
   the word "display" included.

     SM64DS_FRAME_RATE   the rate directly, through the same sanitiser the file
                         goes through: 0 or anything under 60 is native, 144 is
                         144, 1000 clamps to 240, "display" is the primary
                         display's refresh rate, junk reads as 0.

   Read once at boot -- the pacer latches the answer on its first turn and the
   presentation clock cannot change shape mid-run. */
extern "C" int host_setting_frame_rate(void)
{
    static int env_read = 0;
    static int env = -1;             /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_FRAME_RATE");
        if (e && *e) {
            if (strlen(e) == 7 && ieq(e, "display", 7)) {
                env = frame_rate_sanitise(display_refresh_hz());
            } else {
                char *end = 0;
                const long v = strtol(e, &end, 10);
                /* an unparseable override is still an override: it says
                   "native", the same answer an unparseable file value gives */
                env = (end != e) ? frame_rate_sanitise((int)v) : 0;
            }
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_frame_rate;
}

/* ---- THE THREE PICTURE-QUALITY KEYS' ACCESSORS (run hd1) ----------------
   Every one is host_setting_frame_rate's shape exactly: an environment
   override read once, in front of load_once and the stored value, through
   the same sanitiser the file goes through, so a proof run can pin any of
   them off ONE build without editing a player's settings file and the two
   channels cannot disagree about what a value means. See the header for each
   key's contract. */

/* RenderScale: host rows per DS row, 0 for the multiplier the port picks
   today. SM64DS_RENDER_SCALE overrides; junk reads as 0, which is the answer
   an unparseable file value gives. */
extern "C" int host_setting_render_scale(void)
{
    static int env_read = 0;
    static int env = -1;             /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_RENDER_SCALE");
        if (e && *e) {
            char *end = 0;
            const long v = strtol(e, &end, 10);
            env = (end != e) ? render_scale_sanitise((int)v) : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_render_scale;
}

/* ---- THE IMPROVED MINIMAP'S TWO GETTERS ------------------------------------

   THE PIN IS HERE, IN THE GETTER, rather than in the frame loop, and that is
   the whole reason this pair is not two more lines of boilerplate. The option
   changes what the bottom-screen panel DRAWS. Every recorded baseline in this
   tree -- the five level-1 capture hashes, hd1's six key-absent captures, the
   opening gate, every sweep row -- is a picture taken on one of exactly two
   routes: a window selftest (SM64DS_WINDOW_SELFTEST) or a scene run
   (SM64DS_SCENE_FRAMES). A feature that moved those hashes would look like a
   hundred regressions and be none of them. Both frame loops and every proof
   tool in port/tools go through this one function, so pinning it once here
   covers all of them, and tests/walk_window.cpp needs no line of its own.

   THE ENVIRONMENT STILL DISPOSES, in both directions and ahead of the pin: a
   run that means to look at the improved map sets SM64DS_IMPROVED_MINIMAP=1
   and gets it, selftest or not, which is how this lane's own captures were
   taken. That is the same precedence SM64DS_DUAL_SCREEN has over the layout
   proposal in hal/sub_screen.cpp. */
extern "C" int host_setting_improved_minimap(void)
{
    static int env = -2;
    if (env == -2) {
        const char *e = getenv("SM64DS_IMPROVED_MINIMAP");
        env = e ? ((e[0] == 0 || (e[0] == '0' && e[1] == 0)) ? 0 : 1) : -1;
    }
    if (env >= 0) return env;
    if (getenv("SM64DS_WINDOW_SELFTEST") || getenv("SM64DS_SCENE_FRAMES"))
        return 0;
    load_once();
    return g_improved_minimap;
}

/* THE PIN IS A STARTING SIZE, NOT A LOCK ON THE PLAYER'S HAND.
 *
 * SM64DS_MINIMAP_SCALE still outranks the file and still holds for a whole run
 * that never touches the map, which is every scripted run there is: a sweep, a
 * capture, a proof and a gate all ask for a size and none of them has a mouse.
 * That is the reason the override exists and it does not change.
 *
 * What changes is what happens after a HAND has moved it. This getter used to
 * answer the pin ahead of g_minimap_scale unconditionally, and the drag's only
 * write is to g_minimap_scale -- so with the pin set the grab worked, the hold
 * worked, the release worked, and every one of them moved a number that
 * nothing read. The owner's own preview shortcut exports the pin, so on his
 * screen the yellow square took the press, printed that it had, printed a size
 * on release and never once resized the map. Both scripted drags that proved
 * this feature passed the size in settings.json instead, where the pin is not
 * in play, which is the whole of why they were green.
 *
 * So: the pin answers until a drag happens, and the drag answers afterwards.
 * One value, one hand, for the rest of the run. */
/* ---- THE SAVE MENU ON THE TOP SCREEN ---------------------------------------

   The same three-layer shape the improved minimap's getter has, and for the
   same three reasons.

   ABSENT IS ON. It is a mod the owner asked for and recommended on, so a
   settings file written before the key existed gets the composed picture.

   IT IS PINNED OFF ON EVERY COMPARATOR ROUTE. A window selftest and a scene
   sweep row are the two shapes every recorded baseline capture in this tree
   is taken in. This option changes what a frame LOOKS LIKE, so left free it
   would move recorded hashes on any route that reaches a level-clear screen,
   for a reason that has nothing to do with the code under test. Pinning it
   here, in the getter, covers both frame loops and every proof tool in
   port/tools at once.

   THE ENVIRONMENT STILL DISPOSES, in both directions and ahead of the pin: a
   run that means to look at the composed picture sets
   SM64DS_SAVE_MENU_ON_TOP=1 and gets it, selftest or not, which is how every
   capture of it was taken. */
extern "C" int host_setting_save_menu_on_top(void)
{
    static int env = -2;
    if (env == -2) {
        const char *e = getenv("SM64DS_SAVE_MENU_ON_TOP");
        env = e ? ((e[0] == 0 || (e[0] == '0' && e[1] == 0)) ? 0 : 1) : -1;
    }
    if (env >= 0) return env;
    if (getenv("SM64DS_WINDOW_SELFTEST") || getenv("SM64DS_SCENE_FRAMES"))
        return 0;
    load_once();
    return g_save_menu_on_top;
}

extern "C" double host_setting_minimap_scale_value(void)
{
    static int env_read = 0;
    static double env = -1.0;        /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_MINIMAP_SCALE");
        if (e && *e) {
            char *end = 0;
            const double v = strtod(e, &end);
            env = (end != e) ? minimap_scale_sanitise(v, 0) : 1.0;
        }
    }
    if (env > 0.0 && !g_minimap_scale_dragged) return env;
    load_once();
    return g_minimap_scale;
}

/* The chosen size as the exact rational the drawing and the touch inverse
   share. ONE function, so a size can never mean two things in two files --
   which is the rule the corner panel's geometry was already keeping with its
   single integer divisor and has to keep now that the divisor is a fraction
   the player can drag. */
extern "C" void host_setting_minimap_scale_ratio(int *num, int *den)
{
    const int k = minimap_scale_k(host_setting_minimap_scale_value());
    if (num) *num = k;
    if (den) *den = 256;
}

/* THE DRAG'S TWO WRITES.
 *
 * _live moves the size for this run and nothing else: it is called on every
 * frame of a drag, and a settings file rewritten sixty times a second would
 * be a file the launcher is reading while it is half written.
 *
 * _save is the mouse-up: the same move, and then the number on disk, so the
 * size survives a restart and the launcher's picker opens on it. It goes
 * through save_keys like the debug menu's run and camera rows, which reloads
 * the document and carries every key this program did not write across
 * untouched. SM64DS_MINIMAP_SCALE keeps a run that never drags pinned exactly
 * where it asked to be -- which is every scripted run -- and stands aside for
 * a hand that has actually moved the map; the block above the getter is the
 * argument for that.
 *
 * BOTH WRITES GO THROUGH ONE FUNCTION, and that is not tidiness: the defect
 * this closes was a size written in one place and read from another, so a
 * second writer that forgot to say a hand had moved it would put the map
 * straight back to where the owner found it.
 *
 * %.6g is enough to print any number on the grid exactly (the grid is
 * thirty-seconds, and 4096/128 = 32 is the ceiling), so a value written here
 * reads back as the same value. */
namespace {
void minimap_scale_move_live(double s)
{
    load_once();
    g_minimap_scale = minimap_scale_sanitise(s, 0);
    g_minimap_scale_dragged = 1;
}
}

extern "C" void host_setting_minimap_scale_set_live(double s)
{
    minimap_scale_move_live(s);
}

extern "C" int host_setting_save_minimap_scale(double s)
{
    minimap_scale_move_live(s);
    char v[32];
    snprintf(v, sizeof v, "%.6g", g_minimap_scale);
    const char *const keys[1] = { "MinimapScale" };
    const char *const vals[1] = { v };
    return save_keys(keys, vals, 1, "minimap size");
}

/* HdTextures: 1 when the replacement pack is on. SM64DS_HD_TEXTURES has the
   mod keys' grammar rather than a number's -- unset is the file's answer,
   empty or "0" forces it off, anything else forces it on -- because that is
   the spelling every other on/off override in this file uses and a player
   reading a proof recipe should not have to learn a second one. */
extern "C" int host_setting_hd_textures(void)
{
    static int env = -2;
    if (env == -2) {
        const char *e = getenv("SM64DS_HD_TEXTURES");
        env = e ? ((e[0] == 0 || (e[0] == '0' && e[1] == 0)) ? 0 : 1) : -1;
    }
    if (env >= 0) return env;
    load_once();
    return g_hd_textures;
}

/* WHERE THE PACK WOULD BE LOADED FROM, whether or not it is switched on --
   the on/off question is host_setting_hd_textures above and this one is
   only "which directory", so a log line can name the path while the key is
   off and a refusal can say where it looked.

   The candidates, in the order every other asset reader in this port uses
   (hal/gap_art.cpp, hal/fs_mods.cpp):

     SM64DS_HD_TEXTURES_DIR   the directory outright, for a pack somewhere
                              else entirely. Whatever it says, used as given.
     SM64DS_ASSET_ROOT        "<root>/textures_hd". In a player's kit the
                              launcher sets the asset root to the bundle
                              directory, so this is the pack folder sitting
                              beside the exe and settings.json.
     neither                  "textures_hd", relative, which is the working
                              directory -- the same last resort find_settings
                              falls back to for settings.json itself.

   Built once into a static buffer and never null, so a caller can print it
   unconditionally. Does NOT call load_once: nothing here comes from the
   file, and the log line inside load_once calls this. */
extern "C" const char *host_setting_hd_textures_dir(void)
{
    static int built = 0;
    static char dir[1024];
    if (!built) {
        built = 1;
        const char *over = getenv("SM64DS_HD_TEXTURES_DIR");
        if (over && *over) {
            snprintf(dir, sizeof dir, "%s", over);
        } else {
            const char *root = getenv("SM64DS_ASSET_ROOT");
            if (root && *root && strlen(root) + 16 < sizeof dir)
                snprintf(dir, sizeof dir, "%s/textures_hd", root);
            else
                snprintf(dir, sizeof dir, "textures_hd");
        }
    }
    return dir;
}

/* WHERE THE IMPROVED MAP'S PANEL ARTWORK WOULD BE READ FROM, whether or not
   anything is there. Exactly the shape host_setting_hd_textures_dir has above,
   and for the same reason: an asset folder beside the game data, named here so
   a log line can say where the game looked.

     SM64DS_MINIMAP_DIR    the folder outright, used as given. A test points
                           this at a read-only folder somewhere else.
     SM64DS_ASSET_ROOT     "<root>/minimap". In a player's kit the launcher
                           sets the asset root to the bundle directory, so
                           this is a folder sitting beside the exe, exactly
                           where textures_hd sits.
     neither               "minimap", relative to the working directory.

   THE PICTURES ARE NOT PART OF THIS PROGRAM. Nothing here ships, embeds or
   copies them: the code reads a folder, and whether a folder with those two
   files in it travels with a download is a packaging decision made outside
   the code. With no folder the panel is composed at run time from the
   player's own game data, which is what hal/sub_screen.cpp did before this
   and still does. Built once, never null. */
extern "C" const char *host_setting_minimap_dir(void)
{
    static int built = 0;
    static char dir[1024];
    if (!built) {
        built = 1;
        const char *over = getenv("SM64DS_MINIMAP_DIR");
        if (over && *over) {
            snprintf(dir, sizeof dir, "%s", over);
        } else {
            const char *root = getenv("SM64DS_ASSET_ROOT");
            if (root && *root && strlen(root) + 12 < sizeof dir)
                snprintf(dir, sizeof dir, "%s/minimap", root);
            else
                snprintf(dir, sizeof dir, "minimap");
        }
    }
    return dir;
}

/* SmoothModels: the subdivision level, 0 for the ROM's own geometry.
   SM64DS_SMOOTH_MODELS overrides; junk reads as 0. */
extern "C" int host_setting_smooth_models(void)
{
    static int env_read = 0;
    static int env = -1;             /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_SMOOTH_MODELS");
        if (e && *e) {
            char *end = 0;
            const long v = strtol(e, &end, 10);
            env = (end != e) ? smooth_models_sanitise((int)v) : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_smooth_models;
}

/* ---- run hd2's two accessors, host_setting_render_scale's shape exactly --
   an environment override read once, in front of load_once and the stored
   value, through the same sanitiser the file goes through, so a proof run can
   pin either off ONE build without editing a player's settings file. */

/* TextureFilter: 0 nearest, 1 bilinear, 2 trilinear.
   SM64DS_TEXTURE_FILTER overrides; junk reads as 0. */
extern "C" int host_setting_texture_filter(void)
{
    static int env_read = 0;
    static int env = -1;             /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_TEXTURE_FILTER");
        if (e && *e) {
            char *end = 0;
            const long v = strtol(e, &end, 10);
            env = (end != e) ? texture_filter_sanitise((int)v) : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_texture_filter;
}

/* AntiAliasing: 0 off, 1 edge smoothing.
   SM64DS_ANTI_ALIASING overrides; junk reads as 0. */
extern "C" int host_setting_anti_aliasing(void)
{
    static int env_read = 0;
    static int env = -1;
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_ANTI_ALIASING");
        if (e && *e) {
            char *end = 0;
            const long v = strtol(e, &end, 10);
            env = (end != e) ? anti_aliasing_sanitise((int)v) : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_anti_aliasing;
}

/* ---- run hd2 lane GPU1's three accessors, the same shape once more -------
   an environment override read once, in front of load_once and the stored
   value, through the same sanitiser the file goes through, so a proof run can
   pin any of them off ONE build without editing a player's settings file.

   SM64DS_PRESENT_FILTER_D3D rather than SM64DS_PRESENT_FILTER: that name has
   belonged to the ordinary Windows path's halftone knob since before this key
   existed (tests/walk_window.cpp reads it), and two settings answering to one
   variable is how a proof run comes to mean something it did not say. */

/* PresentBackend: 0 the ordinary Windows path, 1 Direct3D 11. */
extern "C" int host_setting_present_backend(void)
{
    static int env_read = 0;
    static int env = -1;             /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_PRESENT_BACKEND");
        if (e && *e) {
            char *end = 0;
            const long v = strtol(e, &end, 10);
            env = (end != e) ? present_backend_sanitise((int)v) : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_present_backend;
}

/* PresentFilter: 0 nearest, 1 smooth, 2 sharp. Only read when the backend
   is 1; this accessor answers either way and the backend is the one that
   decides whether the answer matters. */
extern "C" int host_setting_present_filter(void)
{
    static int env_read = 0;
    static int env = -1;
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_PRESENT_FILTER_D3D");
        if (e && *e) {
            char *end = 0;
            const long v = strtol(e, &end, 10);
            env = (end != e) ? present_filter_sanitise((int)v) : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_present_filter;
}

/* Renderer: 0 the software rasteriser, 1 Direct3D 11. INDEPENDENT of
   PresentBackend: either can be on without the other, and with both on the
   process still makes exactly one device (hal/gpu_device.cpp). */
extern "C" int host_setting_renderer(void)
{
    static int env_read = 0;
    static int env = -1;             /* <0 means "the environment said nothing" */
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_RENDERER");
        if (e && *e) {
            char *end = 0;
            const long v = strtol(e, &end, 10);
            env = (end != e) ? renderer_sanitise((int)v) : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_renderer;
}

/* VSync: 0 off, 1 on. Only read when the backend is 1, for the same reason. */
extern "C" int host_setting_vsync(void)
{
    static int env_read = 0;
    static int env = -1;
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("SM64DS_VSYNC");
        if (e && *e) {
            char *end = 0;
            const long v = strtol(e, &end, 10);
            env = (end != e) ? vsync_sanitise((int)v) : 0;
        }
    }
    if (env >= 0) return env;
    load_once();
    return g_vsync;
}
