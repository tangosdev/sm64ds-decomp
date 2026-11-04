/* hal/mod_values.h: reading a catalog mod's own settings (run after093, lane
   CHARPALG1). A mod folder may carry settings.json, the SPEC (a JSON array of
   setting objects: key, type, choices, default, and optional launcher-only
   fields such as showWhen), and settings.values.json, the player's values (a
   flat JSON object {key: value}, written by the launcher). The value of a key
   is the player's, else the spec's default.

   Pure and header-only so port/tools/mod_values_test.cpp can check it with
   no game around it. hal/fs_mods.cpp is the reader. Strings only: every
   value the game reads this way (a choice, a colour) is a JSON string.

   TOP LEVEL ONLY. A setting object can hold another object (showWhen
   {"key": "mario", "equals": "Custom"}) whose own "key" must never answer for
   the setting's, so every lookup here matches names at depth 1 of the object
   it is handed and nowhere deeper. Names compare exactly (case matters, as in
   the launcher's JSON). */
#ifndef PORT_HAL_MOD_VALUES_H
#define PORT_HAL_MOD_VALUES_H

#include <stddef.h>
#include <string.h>

namespace modval {

static inline const char *skip_ws(const char *q)
{
    while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
    return q;
}

/* One past the closing quote of the string whose opening quote is at s, or 0
   when it never closes. */
static inline const char *string_end(const char *s)
{
    const char *e = s + 1;
    while (*e && *e != '"') e += (*e == '\\' && e[1]) ? 2 : 1;
    return *e == '"' ? e + 1 : 0;
}

/* The string value of `key` at the top level of the object that starts at
   the first '{' of `obj`. 1 when found, a string, and it fits in cap (with
   its NUL); \" and \\ are unescaped, any other escape is kept as written. */
static inline int top_str(const char *obj, const char *key, char *out, size_t cap)
{
    const size_t klen = strlen(key);
    int depth = 0;
    if (!obj || !cap) return 0;
    for (const char *p = obj; *p; ++p) {
        if (*p == '{' || *p == '[') { ++depth; continue; }
        if (*p == '}' || *p == ']') {
            if (--depth <= 0) return 0;
            continue;
        }
        if (*p != '"') continue;
        const char *end = string_end(p);
        if (!end) return 0;
        const char *name = p + 1;
        const size_t nlen = (size_t)(end - 1 - name);
        p = end - 1;
        if (depth != 1) continue;
        const char *q = skip_ws(end);
        if (*q != ':') continue;               /* a value, not a name */
        if (nlen != klen || strncmp(name, key, klen) != 0) continue;
        q = skip_ws(q + 1);
        if (*q != '"') return 0;
        const char *vend = string_end(q);
        if (!vend) return 0;
        size_t n = 0;
        for (const char *v = q + 1; v < vend - 1; ++v) {
            if (*v == '\\' && (v[1] == '"' || v[1] == '\\')) ++v;
            if (n + 1 >= cap) return 0;
            out[n++] = *v;
        }
        out[n] = '\0';
        return 1;
    }
    return 0;
}

/* The "default" of the setting whose "key" is `key` in a spec array. 1 when
   that setting exists and has a string default that fits. */
static inline int spec_default(const char *spec, const char *key, char *out, size_t cap)
{
    int depth = 0;
    const char *obj = 0;
    if (!spec) return 0;
    for (const char *p = spec; *p; ++p) {
        if (*p == '"') {
            const char *end = string_end(p);
            if (!end) return 0;
            p = end - 1;
            continue;
        }
        if (*p == '[' || *p == '{') {
            if (*p == '{' && depth == 1) obj = p;
            ++depth;
        } else if (*p == ']' || *p == '}') {
            --depth;
            if (*p == '}' && depth == 1 && obj) {
                char k[96];
                if (top_str(obj, "key", k, sizeof k) && strcmp(k, key) == 0)
                    return top_str(obj, "default", out, cap);
                obj = 0;
            }
        }
    }
    return 0;
}

/* The value a setting holds: the player's (values, a flat object), else the
   spec's default. Returns 2 for the player's, 1 for the default, 0 for
   neither (out is then ""). */
static inline int value(const char *spec, const char *values, const char *key,
                        char *out, size_t cap)
{
    if (cap) out[0] = '\0';
    if (values && top_str(values, key, out, cap)) return 2;
    if (spec && spec_default(spec, key, out, cap)) return 1;
    if (cap) out[0] = '\0';
    return 0;
}

/* "RRGGBB" (exactly six hex digits, either case) as 0xRRGGBB; -1 otherwise. */
static inline long hex6(const char *s)
{
    long v = 0;
    if (!s) return -1;
    for (int i = 0; i < 6; ++i) {
        const char c = s[i];
        int d;
        if (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
        else return -1;
        v = v * 16 + d;
    }
    return s[6] == '\0' ? v : -1;
}

/* ---- character-palettes: what one character's choice means -------------
   The choices are "Original", for Yoshi "Green" "Red" "Blue" "Yellow" (the
   cartridge's four-player rows), the names of the mod's own palettes/<name>.pal,
   and "Custom". Anything else is read as a palette name, so a preset added
   in a later version of the mod works without a game update. */
enum { CP_ORIGINAL = 0, CP_ROW = 1, CP_FILE = 2, CP_CUSTOM = 3 };

static inline int eq_nocase(const char *a, const char *b)
{
    for (; *a && *b; ++a, ++b) {
        char x = *a, y = *b;
        if (x >= 'A' && x <= 'Z') x = (char)(x - 'A' + 'a');
        if (y >= 'A' && y <= 'Z') y = (char)(y - 'A' + 'a');
        if (x != y) return 0;
    }
    return *a == *b;
}

/* *row is set for CP_ROW (0 green, 1 red, 2 blue, 3 yellow, the order the
   rows are stacked in yoshi_all_16p_pl). Only Yoshi (yoshi != 0) has rows. */
static inline int choice_kind(const char *v, int yoshi, int *row)
{
    static const char *const kRow[4] = { "Green", "Red", "Blue", "Yellow" };
    *row = -1;
    if (!v || !*v || eq_nocase(v, "Original")) return CP_ORIGINAL;
    if (eq_nocase(v, "Custom")) return CP_CUSTOM;
    if (yoshi)
        for (int i = 0; i < 4; ++i)
            if (eq_nocase(v, kRow[i])) { *row = i; return CP_ROW; }
    return CP_FILE;
}

/* ---- loves-me-swap: the `character` choice ------------------------------
   The launcher's labels, in its order, which is also host_settings.cpp's
   LOVESME_KEY order (1-based). The old key's spellings are accepted too.
   0 = not a character this build knows (the ROM's Yoshi). */
static inline int lovesme_index(const char *v)
{
    static const char *const kLabel[6] = {
        "Mario", "Red Yoshi", "Blue Yoshi", "Yellow Yoshi", "Luigi", "Wario",
    };
    static const char *const kKey[6] = {
        "mario", "yoshi_red", "yoshi_blue", "yoshi_yellow", "luigi", "wario",
    };
    if (!v || !*v) return 0;
    for (int i = 0; i < 6; ++i)
        if (eq_nocase(v, kLabel[i]) || eq_nocase(v, kKey[i])) return i + 1;
    return 0;
}

}  // namespace modval

#endif  // PORT_HAL_MOD_VALUES_H
