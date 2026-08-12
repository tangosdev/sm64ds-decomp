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

/* Find `key` used as an object KEY and read the JSON true/false after it.
   Returns dflt unless a real boolean is there.

   Strings that are values rather than keys are stepped over, so a credit
   name that happens to contain the key's spelling cannot be mistaken for
   the key itself. */
int json_bool(const char *s, const char *key, int dflt)
{
    const size_t klen = strlen(key);
    for (const char *p = s; *p; ++p) {
        if (*p != '"') continue;
        const char *name = p + 1;
        const char *end = name;
        while (*end && *end != '"' && *end != '\\') ++end;
        if (*end != '"') {           /* escape or unterminated: stop here */
            if (!*end) break;
            p = end;
            continue;
        }
        const int is_key = (size_t)(end - name) == klen && ieq(name, key, klen);
        const char *q = end + 1;
        while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
        if (*q != ':') { p = end; continue; }   /* a string value, not a key */
        ++q;
        while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
        if (is_key) {
            /* length before the compare: ieq reads a fixed count, so a
               value shorter than the literal must never reach it */
            const size_t vlen = strlen(q);
            if (vlen >= 4 && ieq(q, "true", 4)) return 1;
            if (vlen >= 5 && ieq(q, "false", 5)) return 0;
            return dflt;             /* present but not a boolean */
        }
        p = q - 1;                   /* carry on from the value */
    }
    return dflt;
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

int g_loaded;
int g_swap_camera_turn;              /* default 0 */

void load_once(void)
{
    if (g_loaded) return;
    g_loaded = 1;
    g_swap_camera_turn = 0;

    char path[1024];
    if (!find_settings(path, sizeof path)) return;
    char *text = slurp(path);
    if (!text) return;
    if (looks_like_json_object(text))
        g_swap_camera_turn = json_bool(text, "SwapCameraTurnDirection", 0);
    free(text);

    /* Said out loud only when a setting is off its default, so an ordinary
       run's log is unchanged and a support log shows at a glance which
       non-default choices were in force. */
    if (g_swap_camera_turn)
        fprintf(stderr, "[settings] SwapCameraTurnDirection on (%s)\n", path);
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
