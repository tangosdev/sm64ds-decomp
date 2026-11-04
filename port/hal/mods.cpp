/* hal/mods.cpp: mods that are folders, the list, and the files/ layer
   (run mods1, lane MODFILES1). The contract is runs/mods1/in/MODS_CONTRACT.md;
   this file is its game side for files/. textures_hd/ is applied by
   ntr/hdtex.cpp and palettes/ by hal/fs_mods.cpp, both through the list below.

   THE LIST (mods.h). Boot-latched: EnabledMods is read once. An id is skipped,
   with one stderr line saying why, when it is not [a-z0-9-] of 1..64
   characters, is a Windows reserved device name, repeats an earlier id, has no
   mods/<id>/ folder, or has a mod.json that is missing, unreadable, not a JSON
   object, has no "id" or an "id" other than the folder's, or a "kind" other
   than "files" or "code". Never a crash: nothing below aborts.

   THE FILES LAYER. A mod's files/<rom path> replaces the ROM file with that
   path for every read. The path is looked up exactly (case-insensitively for
   the loose files, whose catalog is hal/fs.cpp's files.tsv; archive members by
   their NARC's own name table, the way hal/fs.cpp's port_fs_interior_id does).
   A path that names no ROM file is ignored with a line. Two mods replacing the
   same file: the later in EnabledMods wins, and a line says so.

   It rides the hooks hal/fs.cpp already has: the FILTER (rewrites a file's
   decompressed master as it is served; a returned buffer REPLACES the stock
   one, any size) and the CLAIMS (the ids the card must pre-serve, because the
   game targets read through the ROM's own loaders and the NARC the ROM mounted;
   an archive member's NARC is rebuilt around the new member by hal/fs.cpp's
   existing code, which is why no size limit applies here either).

   WHY THIS LINK IS LAST. hal/fs_mods.cpp installs its link from an ordinary
   initializer and hal/stage_geom.cpp chains onto it from .CRT$XCV; this one is
   .CRT$XCW, after both, and calls what it chained to FIRST and then replaces
   the buffer. A mod's file sits ON TOP of the port's own mods for the same
   file, for two reasons. The port's mods (the palette settings, Loves Me, the
   stage-geometry folder) patch bytes at offsets they found in the STOCK file,
   so running them over a different file would write colours into the wrong
   place; and a player who installed a mod that replaces a file chose that file
   on purpose, which outranks a global setting. The port's mods have already
   run on the stock bytes and those bytes are simply dropped.

   A mod file that starts with "LZ77" (a file copied straight out of the
   extracted game is stored that way when the cartridge stores it that way) is
   decompressed here: the filter's contract is the DECOMPRESSED master, and
   hal/fs.cpp re-wraps the result the way the stock file is held.

   KNOWN GAP, RECORDED NOT FIXED: save states. A state saved with one mod set
   and loaded with another can mix resident files: the state holds the files
   the game had loaded (a model, an animation) as they were when it was saved,
   while the files the game loads after the restore come from the mod set the
   running game booted with. A model from one set can then meet an animation
   from the other. A future fix is a mods signature (the ordered list of ids
   with each mod's version) in the save-state header, refused or warned about
   on a load that does not match. Nothing here writes or checks one yet. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <unordered_map>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <psapi.h>

#include "mods.h"
#include "char_registry.h"
#include "ntr/hdtex.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern "C" {
int host_setting_adventure_ghosts(void);   /* hal/host_settings.cpp: settings.json AdventureGhosts, SM64DS_ADVENTURE */
int host_setting_enabled_mods(char *bundle, size_t bcap, char *arr, size_t acap);  /* 1 got, 0 none, -1 too long */
int host_json_is_object(const char *text);
int host_json_string(const char *text, const char *key, char *out, size_t cap);
const char *port_fs_catalog_path(unsigned file_id);   /* hal/fs.cpp */
unsigned port_fs_interior_id_nocase(const char *want); /* hal/fs.cpp */
unsigned port_fs_member_claim_cap(void);              /* hal/fs.cpp */
extern unsigned (*port_fs_mod_map)(unsigned fileID);
extern u32 (*port_fs_mod_filter)(unsigned fileID, u8 **data, u32 size);
extern void (*port_fs_mod_claims)(void (*claim)(unsigned fileID));
}

/* A multiplayer session is any of: the
   launcher's lobby / VS match env (comms role and relay knobs, the VS-match
   knobs), SM64DS_PARTY set to a nonzero number (read the way comms_sync.cpp
   reads it), or the Adventure hub (settings.json AdventureGhosts, or the
   SM64DS_ADVENTURE override), which is the shared adventure with other players.
   The ONE definition: the mod list below asks it and, in a multiplayer session,
   keeps only the mods tagged Online Compatible that are cosmetic by content
   (online_ok below); the plugin loader only sees that list, which never holds
   a code mod. host_setting_adventure_ghosts() runs the settings
   loader (load_once), which reads the file itself and never calls back into
   mods, and host_setting_enabled_mods() reads the file on its own, so there is
   no loop, and load_once's one-shot guard makes any second call a no-op. */
extern "C" int port_mods_multiplayer(void)
{
    static const char *const K[] = {
        "SM64DS_COMMS_ROLE", "SM64DS_COMMS_RELAY", "SM64DS_COMMS_HOST", "SM64DS_COMMS_CODE",
        "SM64DS_COMMS_SLOT", "SM64DS_VS_MAP", "SM64DS_VS_MODE", "SM64DS_VS_PLAYERS", 0 };
    for (int i = 0; K[i]; ++i) {
        const char *v = getenv(K[i]);
        if (v && *v) return 1;
    }
    const char *party = getenv("SM64DS_PARTY");
    if (party && atoi(party) != 0) return 1;
    return host_setting_adventure_ghosts() != 0;
}

namespace {

/* the latch's and the files layer's log lines (lane SEAMART1). Straight to stderr, except while the relatch at the title builds the
   new list: then they wait in g_log_hold, and the relatch prints them only once the switch is accepted. A refused relatch prints its
   one refusal line and nothing else, where it used to print the whole list a second time (REVSEAMMODS1 note e). */
std::string *g_log_hold;
void mlog(const char *fmt, ...)
{
    char b[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    if (g_log_hold) *g_log_hold += b;
    else fputs(b, stderr);
}

struct Mod {
    std::string id, folder, kind, game, min_game;
    int api;
    bool part;   /* a switchable sub-folder of the mod above it (see add_parts) */
    bool online; /* mod.json "onlineCompatible": true (the literal true, nothing else) */
};

std::vector<Mod> g_mods;
std::string g_bundle;
int g_latched;

bool reserved_name(const std::string &id)
{
    static const char *const R[] = { "con", "prn", "aux", "nul", 0 };
    for (int i = 0; R[i]; ++i)
        if (id == R[i]) return true;
    if (id.size() == 4 && (id.compare(0, 3, "com") == 0 || id.compare(0, 3, "lpt") == 0)
        && id[3] >= '0' && id[3] <= '9')
        return true;
    return false;
}

bool valid_id(const std::string &id, const char **why)
{
    if (id.empty() || id.size() > 64) { *why = "id is empty or longer than 64 characters"; return false; }
    for (size_t i = 0; i < id.size(); ++i) {
        const char c = id[i];
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-')) {
            *why = "id has a character outside a-z 0-9 and -";
            return false;
        }
    }
    if (reserved_name(id)) { *why = "id is a reserved Windows name"; return false; }
    return true;
}

char *slurp_small(const std::string &path)
{
    FILE *f = fopen(path.c_str(), "rb");
    long n;
    char *b;
    size_t got;
    if (!f) return 0;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
    n = ftell(f);
    if (n < 0 || n > (1 << 20)) { fclose(f); return 0; }
    fseek(f, 0, SEEK_SET);
    b = (char *)malloc((size_t)n + 1);
    if (!b) { fclose(f); return 0; }
    got = fread(b, 1, (size_t)n, f);
    fclose(f);
    b[got] = 0;
    return b;
}

bool dir_exists(const std::string &p)
{
    const DWORD a = GetFileAttributesA(p.c_str());
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
}


/* ---- switchable parts (lane MODPACK1) -----------------------------------
   A files mod may declare, in its settings.json (the launcher's list of
   settings), a bool or choice setting that carries a "files" entry: a folder
   inside the mod, relative, written with / between names. That folder is a
   PART. It is laid out exactly like a mod folder (files/, sound/, textures_hd/,
   palettes/) and is applied only while the setting says so.

     bool    {"key":"x","type":"bool","default":true,"files":"parts/x"}
             applies parts/x when the player's value (settings.values.json) is
             true, else when it is absent and the default is true.
     choice  {"key":"q","type":"choice","choices":["hd","ds"],"default":"hd",
              "files":"parts/q"}
             applies the folder named by the value, parts/q/<value>.

   A part joins the mod list as its own entry straight AFTER its mod (so it
   outranks the mod and sits below the next mod in EnabledMods), kind "files",
   id "<mod>:<part folder>". Everything that walks the list (the files layer,
   textures_hd, palettes, sound/ including single-wave .wav files) therefore
   treats it like any mod, with no other code. Read once at boot, with the
   list. A part folder must stay inside the mod: no "..", no drive or stream
   colon, no leading slash, only letters, digits, - _ . and / . */
bool part_path_ok(const std::string &rel)
{
    if (rel.empty() || rel.size() > 120 || rel[0] == '/' || rel[0] == '.') return false;
    if (rel.find("//") != std::string::npos || rel[rel.size() - 1] == '/') return false;
    for (size_t i = 0; i < rel.size(); ++i) {
        const char c = rel[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '/'))
            return false;
    }
    if (rel.find("..") != std::string::npos) return false;
    return true;
}

void add_parts(const Mod &parent, std::vector<Mod> &list, std::string &summary)
{
    char *spec = slurp_small(parent.folder + "/settings.json");
    if (!spec) return;
    char *vals = slurp_small(parent.folder + "/settings.values.json");
    int nparts = 0;
    /* the top-level objects of the settings array */
    int depth = 0;
    const char *objstart = 0;
    for (const char *p = spec; *p; ++p) {
        if (*p == '"') {
            ++p;
            while (*p && *p != '"') p += (*p == '\\' && p[1]) ? 2 : 1;
            if (!*p) break;
            continue;
        }
        if (*p == '[' || *p == '{') {
            if (*p == '{' && depth == 1) objstart = p;
            ++depth;
        } else if (*p == ']' || *p == '}') {
            --depth;
            if (*p == '}' && depth == 1 && objstart) {
                const std::string obj(objstart, (size_t)(p + 1 - objstart));
                objstart = 0;
                char key[96] = "", type[16] = "", files[160] = "", def[96] = "", val[96] = "";
                host_json_string(obj.c_str(), "key", key, sizeof key);
                host_json_string(obj.c_str(), "type", type, sizeof type);
                if (!host_json_string(obj.c_str(), "files", files, sizeof files) || !files[0]) continue;
                host_json_string(obj.c_str(), "default", def, sizeof def);
                const bool have = vals && key[0] && host_json_string(vals, key, val, sizeof val);
                const char *src = have ? "player's value" : "default";
                const std::string cur = have ? val : def;
                if (!key[0]) continue;
                if (nparts >= 16) { mlog("[mods] %s: more than 16 parts, the rest are ignored\n", parent.id.c_str()); break; }
                if (!part_path_ok(files)) {
                    mlog("[mods] %s: setting %s: part folder \"%s\" is not a plain folder inside the mod, ignored\n",
                            parent.id.c_str(), key, files);
                    continue;
                }
                std::string rel = files;
                bool on;
                if (!strcmp(type, "bool")) {
                    on = cur == "true";
                } else if (!strcmp(type, "choice")) {
                    on = !cur.empty() && part_path_ok(cur) && cur.find('/') == std::string::npos;
                    if (on) rel += "/" + cur;
                } else {
                    mlog("[mods] %s: setting %s: \"files\" needs a bool or choice setting, ignored\n", parent.id.c_str(), key);
                    continue;
                }
                ++nparts;
                if (!on) {
                    mlog("[mods] %s: setting %s = %s (%s): part %s is off\n", parent.id.c_str(), key,
                            cur.empty() ? "-" : cur.c_str(), src, rel.c_str());
                    continue;
                }
                const std::string dir = parent.folder + "/" + rel;
                if (!dir_exists(dir)) {
                    mlog("[mods] %s: setting %s = %s (%s): part folder %s does not exist, nothing applied\n",
                            parent.id.c_str(), key, cur.c_str(), src, rel.c_str());
                    continue;
                }
                Mod pm;
                pm.id = parent.id + ":" + rel;
                pm.folder = dir;
                pm.kind = "files";
                pm.api = 0;
                pm.part = true;
                pm.online = false;
                list.push_back(pm);
                summary += " + part " + rel;
                mlog("[mods] %s: setting %s = %s (%s): part %s is on\n", parent.id.c_str(), key,
                        cur.c_str(), src, rel.c_str());
            }
        }
    }
    free(spec);
    if (vals) free(vals);
}

/* A files mod with none of files/, sound/, textures_hd/, palettes/, hd_ui/ (in itself
   or in an applied part) replaces nothing: say so, once. The usual cause is a
   zip that was unpacked with its contents inside one extra folder. */
bool has_content(const std::string &folder)
{
    /* CHARPALG1: the two mods whose settings the game reads itself
       (hal/fs_mods.cpp, THE TWO CATALOG MODS) do their work with no folder */
    const size_t slash = folder.find_last_of("/\\");
    const std::string leaf = slash == std::string::npos ? folder : folder.substr(slash + 1);
    if (leaf == "character-palettes" || leaf == "loves-me-swap") return true;
    static const char *const D[] = { "/files", "/sound", "/textures_hd", "/palettes", "/character", "/hd_ui", 0 };
    for (int i = 0; D[i]; ++i)
        if (dir_exists(folder + D[i])) return true;
    return false;
}

std::string g_summary_loaded, g_summary_skipped;
int g_nskipped;

void skip(const std::string &id, const char *why)
{
    mlog("[mods] skipped %s: %s\n", id.c_str(), why);
    if (g_nskipped++) g_summary_skipped += ", ";
    g_summary_skipped += id + " (" + why + ")";
}

/* ---- Online Compatible (run after093, lane MODONLINE1) ------------------
   mod.json "onlineCompatible": true is the author's claim that the mod is
   cosmetic only: it changes what THIS player sees and hears and nothing the
   other players' games have to agree with. In a multiplayer session a mod
   stays on only when it carries the tag AND its content proves it (online_ok);
   every other mod is off there, as before.

   The tag is the JSON literal true at the top level of mod.json, the key
   spelled exactly; absent, false, "true" (a string), 1 or anything else is no
   tag. json_top_value is host_settings.cpp's top-level scan, case-sensitive. */
const char *json_skip_ws(const char *q)
{
    while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
    return q;
}

const char *json_top_value(const char *s, const char *key)
{
    const size_t klen = strlen(key);
    int depth = 0;
    for (const char *p = s; *p; ++p) {
        if (*p == '{' || *p == '[') { ++depth; continue; }
        if (*p == '}' || *p == ']') { --depth; continue; }
        if (*p != '"') continue;
        const char *name = p + 1, *end = name;
        while (*end && *end != '"') end += (*end == '\\' && end[1]) ? 2 : 1;
        if (*end != '"') return 0;
        p = end;
        if (depth != 1) continue;
        const char *q = json_skip_ws(end + 1);
        if (*q != ':') continue;
        if ((size_t)(end - name) == klen && strncmp(name, key, klen) == 0)
            return json_skip_ws(q + 1);
    }
    return 0;
}

bool json_top_true(const char *s, const char *key)
{
    const char *q = json_top_value(s, key);
    if (!q || strncmp(q, "true", 4) != 0) return false;
    q = json_skip_ws(q + 4);
    return *q == ',' || *q == '}';
}

/* THE CONTENT CHECK: the tag is a claim, the game verifies it. A folder is
   cosmetic when every folder directly inside it is textures_hd/ (HD
   textures), hd_ui/ (UI art), sound/ (sounds), palettes/ (colour tables) or
   media/ (the launcher's pictures and videos of the mod, which the game never
   reads; run after093, lane CHARPALG1).
   Anything else is not: files/ replaces raw game data (models, collision,
   levels, scripts), character/ is a playable character, plugin/ is code, and
   a folder this check does not know (parts/ included) is refused rather than
   guessed at. Loose files beside mod.json (a readme, settings.json) are not
   applied by the game and do not count. Names are compared the way Windows
   opens them, ignoring case. True, or false with the first failing folder's
   name in *bad. */
bool cosmetic_folder(const std::string &folder, std::string *bad)
{
    static const char *const OK[] = { "textures_hd", "hd_ui", "sound", "palettes", "media", 0 };
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((folder + "/*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return true;
    bool ok = true;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
        const char *n = fd.cFileName;
        if (!strcmp(n, ".") || !strcmp(n, "..")) continue;
        bool hit = false;
        for (int i = 0; OK[i] && !hit; ++i) hit = _stricmp(n, OK[i]) == 0;
        if (!hit) { *bad = n; ok = false; }
    } while (ok && FindNextFileA(h, &fd));
    FindClose(h);
    return ok;
}

/* Whether mod m, with its applied parts list[first, last), may stay on in a
   multiplayer session. For a TAGGED mod that is refused, *why says why. */
bool online_ok(const Mod &m, const std::vector<Mod> &list, size_t first, size_t last,
               const std::string &json, std::string *why)
{
    std::string bad;
    if (!m.online) return false;
    if (m.kind != "files") { *why = "code mod"; return false; }
    if (!cosmetic_folder(m.folder, &bad)) { *why = "ships " + bad + "/"; return false; }
    for (size_t q = first; q < last; ++q)
        if (!cosmetic_folder(list[q].folder, &bad)) {
            *why = "part " + list[q].id.substr(m.id.size() + 1) + " ships " + bad + "/";
            return false;
        }
    /* a mod.json "character" block registers a playable character
       (hal/char_registry.cpp) wherever its files sit */
    if (json_top_value(json.c_str(), "character")) { *why = "declares a character"; return false; }
    return true;
}

/* port_mods_relatch_at_title (below) runs the latch a second time: with
   g_relatching set it skips the character registry's offer (a character block
   on either side of a relatch is refused, so an unchanged one is already
   registered), and g_list_override, when set, is the EnabledMods array text to
   use instead of settings.json's (the dev knob SM64DS_MODS_RELATCH_AT). */
int g_relatching;
const char *g_list_override;

void latch(void)
{
    char bundle[1024];
    static char arr[65536];   /* about 900 ids of 64 characters; static to keep the stack small */
    const char *kill;
    if (g_latched) return;
    g_latched = 1;

    kill = getenv("SM64DS_MODS");
    if (kill && strcmp(kill, "0") == 0) {
        mlog("[mods] SM64DS_MODS=0: every mod is off for this run\n");
        return;
    }
    /* A multiplayer session keeps only the mods online_ok passes, in their
       order, and says so in ONE line once the list is read. */
    const bool mp = port_mods_multiplayer() != 0;
    int have = host_setting_enabled_mods(bundle, sizeof bundle, arr, sizeof arr);
    if (g_list_override && have >= 0 && strlen(g_list_override) < sizeof arr) {
        strcpy(arr, g_list_override);
        have = 1;
    }
    g_bundle = bundle;
    if (have < 0) {
        mlog("[mods] EnabledMods too long (over %u characters); no mods are on this run (bundle %s)\n",
                (unsigned)(sizeof arr - 1), bundle);
        if (mp) mlog("[mods] multiplayer match: 0 mod(s) kept (Online Compatible): -; 0 off: -\n");
        return;
    }
    if (!have) {
        /* No EnabledMods: nothing on. Said once so a support log shows it. */
        mlog("[mods] none enabled (bundle %s)\n", bundle);
        if (mp) mlog("[mods] multiplayer match: 0 mod(s) kept (Online Compatible): -; 0 off: -\n");
        return;
    }
    std::string mp_kept, mp_off;
    unsigned mp_nkept = 0, mp_noff = 0;

    /* Only string elements directly in the array count. Numbers, true, false
       and null are passed over; a nested array or object is passed over whole,
       with every string inside it. */
    std::vector<std::string> ids;
    {
        int depth = 0;
        for (const char *p = arr; *p; ++p) {
            if (*p == '[' || *p == '{') { ++depth; continue; }
            if (*p == ']' || *p == '}') { --depth; continue; }
            if (*p != '"') continue;
            const char *e = ++p;
            while (*e && *e != '"') e += (*e == '\\' && e[1]) ? 2 : 1;
            if (*e != '"') break;
            if (depth == 1)
                ids.push_back(std::string(p, (size_t)(e - p)));
            p = e;
        }
    }

    for (size_t i = 0; i < ids.size(); ++i) {
        const std::string &id = ids[i];
        const char *why = 0;
        if (!valid_id(id, &why)) {
            /* a bad id is never put into a path */
            skip(id.size() > 40 ? id.substr(0, 40) + "..." : id, why);
            continue;
        }
        bool dup = false;
        for (size_t k = 0; k < g_mods.size(); ++k)
            if (g_mods[k].id == id) dup = true;
        if (dup) { skip(id, "listed twice (the first entry counts)"); continue; }

        const std::string folder = g_bundle + "/mods/" + id;
        if (!dir_exists(folder)) { skip(id, "no mods/<id> folder"); continue; }
        char *text = slurp_small(folder + "/mod.json");
        if (!text) { skip(id, "mod.json missing or unreadable"); continue; }
        if (!host_json_is_object(text)) { free(text); skip(id, "mod.json is not a JSON object"); continue; }
        char jid[96] = "", kind[16] = "", game[48] = "", api[16] = "", mingame[48] = "";
        host_json_string(text, "id", jid, sizeof jid);
        host_json_string(text, "kind", kind, sizeof kind);
        host_json_string(text, "game", game, sizeof game);
        host_json_string(text, "api", api, sizeof api);
        host_json_string(text, "minGameVersion", mingame, sizeof mingame);
        const std::string json_copy(text);
        free(text);
        if (!jid[0]) { skip(id, "mod.json has no id"); continue; }
        if (id != jid) { skip(id, "mod.json id is not the folder name"); continue; }
        if (strcmp(kind, "files") && strcmp(kind, "code")) { skip(id, "mod.json kind is not files or code"); continue; }

        Mod m;
        m.id = id;
        m.folder = folder;
        m.kind = kind;
        m.game = game;
        m.min_game = mingame;
        m.api = atoi(api);
        m.part = false;
        m.online = json_top_true(json_copy.c_str(), "onlineCompatible");
        if (mp) {
            /* the parts are read first, so the check sees every folder this
               mod would apply; a refused mod takes its parts with it */
            const size_t at = g_mods.size();
            std::string parts_summary, why;
            g_mods.push_back(m);
            add_parts(m, g_mods, parts_summary);
            if (!online_ok(m, g_mods, at + 1, g_mods.size(), json_copy, &why)) {
                g_mods.erase(g_mods.begin() + (std::ptrdiff_t)at, g_mods.end());
                if (!why.empty())
                    mlog("[mods] %s: Online Compatible refused: %s\n", id.c_str(), why.c_str());
                if (mp_noff++) mp_off += ", ";
                mp_off += id;
                continue;
            }
            if (mp_nkept++) mp_kept += ", ";
            mp_kept += id;
            if (!g_summary_loaded.empty()) g_summary_loaded += ", ";
            g_summary_loaded += id + " (" + kind + ")" + parts_summary;
            bool any = has_content(folder);
            for (size_t q = at + 1; q < g_mods.size() && !any; ++q) any = has_content(g_mods[q].folder);
            if (!any)
                mlog("[mods] WARNING: %s replaces nothing: it has no files/, sound/, textures_hd/ or palettes/ folder "
                        "(a zip unpacked with everything inside one extra folder looks like this)\n", id.c_str());
            continue;
        }
        if (!g_summary_loaded.empty()) g_summary_loaded += ", ";
        g_mods.push_back(m);
        g_summary_loaded += id + " (" + kind + ")";
        if (!g_relatching)
            port_char_offer(id.c_str(), folder.c_str(), json_copy.c_str());   /* CHARSLOT1: a "character" block registers dormant */
        const size_t before = g_mods.size();
        add_parts(m, g_mods, g_summary_loaded);
        if (!strcmp(kind, "files")) {
            bool any = has_content(folder);
            for (size_t q = before; q < g_mods.size() && !any; ++q) any = has_content(g_mods[q].folder);
            if (!any)
                mlog("[mods] WARNING: %s replaces nothing: it has no files/, sound/, textures_hd/ or palettes/ folder "
                        "(a zip unpacked with everything inside one extra folder looks like this)\n", id.c_str());
        }
    }
    if (mp)
        mlog("[mods] multiplayer match: %u mod(s) kept (Online Compatible): %s; %u off: %s\n",
                mp_nkept, mp_nkept ? mp_kept.c_str() : "-", mp_noff, mp_noff ? mp_off.c_str() : "-");
    mlog("[mods] %u loaded: %s | %d skipped%s%s\n", (unsigned)g_mods.size(),
            g_mods.empty() ? "-" : g_summary_loaded.c_str(), g_nskipped,
            g_nskipped ? ": " : "", g_summary_skipped.c_str());
    fflush(stderr);
}

/* ---- files/ ------------------------------------------------------------ */

std::unordered_map<unsigned, std::string> g_override;   /* file id -> path on disk */
int g_override_built;

std::string lower(const std::string &s)
{
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i) r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}

void walk(const std::string &root, const std::string &rel, std::vector<std::string> &out, int depth)
{
    WIN32_FIND_DATAA fd;
    const std::string pat = root + (rel.empty() ? "" : "/" + rel) + "/*";
    HANDLE h = FindFirstFileA(pat.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        const std::string n = fd.cFileName;
        if (n == "." || n == "..") continue;
        const std::string r = rel.empty() ? n : rel + "/" + n;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            /* a junction or symlink inside a mod folder could point anywhere
               on the disk; the mod's own files are real files and folders */
            mlog("[mods] %s/%s is a link, skipped\n", root.c_str(), r.c_str());
            continue;
        }
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (depth < 12) walk(root, r, out, depth + 1);
        } else {
            out.push_back(r);
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

void build_overrides(void)
{
    if (g_override_built) return;
    g_override_built = 1;
    latch();
    if (g_mods.empty()) {
        port_char_active();   /* CHARSAVE1: resolves a saved character pick, so a missing mod is said once in the log */
        return;
    }

    std::unordered_map<std::string, unsigned> loose;
    for (unsigned id = 0; id < 4096; ++id) {
        const char *cp = port_fs_catalog_path(id);
        if (cp) loose[lower(cp)] = id;
    }
    std::unordered_map<unsigned, std::string> owner;   /* id -> mod id, for the "wins" line */
    unsigned total_members = 0;
    int warned = 0;
    for (size_t m = 0; m < g_mods.size(); ++m) {
        const std::string root = g_mods[m].folder + "/files";
        std::vector<std::string> rels;
        walk(root, "", rels, 0);
        unsigned used = 0, ignored = 0, members = 0;
        for (size_t k = 0; k < rels.size(); ++k) {
            unsigned id = 0;
            std::unordered_map<std::string, unsigned>::const_iterator it = loose.find(lower(rels[k]));
            if (it != loose.end()) id = it->second;
            else id = port_fs_interior_id_nocase(rels[k].c_str());
            if (!id) {
                mlog("[mods] %s: files/%s is not a ROM file, ignored\n",
                        g_mods[m].id.c_str(), rels[k].c_str());
                ++ignored;
                continue;
            }
            std::unordered_map<unsigned, std::string>::const_iterator o = owner.find(id);
            if (o != owner.end())
                mlog("[mods] files/%s is replaced by %s and by %s; %s wins (later in the list)\n",
                        rels[k].c_str(), o->second.c_str(), g_mods[m].id.c_str(), g_mods[m].id.c_str());
            if (id >= 0x8000 && !owner.count(id)) ++members;
            owner[id] = g_mods[m].id;
            g_override[id] = root + "/" + rels[k];
            ++used;
        }
        if (used || ignored)
            mlog("[mods] %s: files/ replaces %u ROM file(s)%s\n", g_mods[m].id.c_str(), used,
                    ignored ? " (some ignored, see above)" : "");
        total_members += members;
        if (!warned && total_members > port_fs_member_claim_cap()) {
            /* hal/fs.cpp holds this many archive members at once; the rest
               keep the cartridge's bytes. One line, naming the mod whose files
               went over, instead of one line per member. */
            mlog("[mods] WARNING: %u archive members are replaced in all, more than the %u the game can hold; "
                    "%s is the mod that went over, and its members past the limit keep the cartridge's bytes\n",
                    total_members, port_fs_member_claim_cap(), g_mods[m].id.c_str());
            warned = 1;
        }
    }
    /* CHARSLOT1: the active character skin (SM64DS_CHAR_SKIN, hal/char_registry.h)
       replaces the base character's body and heads, ON TOP of every files/ entry:
       choosing a character is a deliberate act. Not counted against the member cap. */
    for (int k = 0;; ++k) {
        const char *rom = 0, *disk = 0;
        if (!port_char_skin_file(k, &rom, &disk)) break;
        unsigned id = 0;
        std::unordered_map<std::string, unsigned>::const_iterator it = loose.find(lower(rom));
        if (it != loose.end()) id = it->second;
        else id = port_fs_interior_id_nocase(rom);
        if (!id) { mlog("[chars] %s is not a ROM file in this dump; skin file skipped\n", rom); continue; }
        g_override[id] = disk;
        mlog("[chars] skin file %s serves ROM file id %u (%s)\n", disk, id, rom);
    }
    mlog("[mods] files layer: %u ROM file id(s) overridden in all\n", (unsigned)g_override.size());
    fflush(stderr);
}

/* The checked LZ77 (type 0x10) decoder for a mod's file. DecompressLZ16 trusts
   its input and has no bounds, which is right for the cartridge's own files and
   wrong for a file anyone can ship. This one needs the type byte 0x10, caps the
   declared size at 64 MB, stops with a failure when the input runs out, and
   rejects a back-reference that reaches before the start of the output.
   `in` is the stream after the "LZ77" tag (the 0x10 header word first).
   Returns a malloc'd buffer and its length, or 0. */
const u32 LZ_MAX = 64u << 20;

u8 *lz77_checked(const u8 *in, size_t n, u32 *len)
{
    if (n < 4 || in[0] != 0x10) return 0;
    const u32 dec = in[1] | in[2] << 8 | (u32)in[3] << 16;
    if (dec > LZ_MAX) return 0;
    u8 *out = (u8 *)malloc(dec ? dec : 1);
    if (!out) return 0;
    size_t ip = 4;
    u32 op = 0;
    while (op < dec) {
        if (ip >= n) goto bad;
        {
            const u8 flags = in[ip++];
            for (int bit = 0; bit < 8 && op < dec; ++bit) {
                if (flags & (0x80 >> bit)) {
                    if (ip + 2 > n) goto bad;
                    const u32 b1 = in[ip], b2 = in[ip + 1];
                    ip += 2;
                    const u32 cnt = (b1 >> 4) + 3;
                    const u32 disp = ((b1 & 15) << 8 | b2) + 1;
                    if (disp > op) goto bad;
                    for (u32 k = 0; k < cnt && op < dec; ++k, ++op)
                        out[op] = out[op - disp];
                } else {
                    if (ip >= n) goto bad;
                    out[op++] = in[ip++];
                }
            }
        }
    }
    *len = dec;
    return out;
bad:
    free(out);
    return 0;
}

/* The override's DECOMPRESSED bytes, malloc'd; 0 when unreadable. */
u8 *read_override(const std::string &path, u32 *len)
{
    FILE *f = fopen(path.c_str(), "rb");
    long n;
    u8 *raw, *out;
    if (!f) return 0;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
    n = ftell(f);
    if (n < 0) { fclose(f); return 0; }
    fseek(f, 0, SEEK_SET);
    raw = (u8 *)malloc(n ? (size_t)n : 1);
    if (!raw) { fclose(f); return 0; }
    if ((long)fread(raw, 1, (size_t)n, f) != n) { fclose(f); free(raw); return 0; }
    fclose(f);
    if (n > 8 && memcmp(raw, "LZ77", 4) == 0) {
        out = lz77_checked(raw + 4, (size_t)n - 4, len);
        free(raw);
        return out;   /* 0 for a malformed stream: the caller keeps the stock file */
    }
    *len = (u32)n;
    return raw;
}

u32 (*g_prev_filter)(unsigned, u8 **, u32);
void (*g_prev_claims)(void (*)(unsigned));
std::unordered_map<unsigned, int> g_said;

u32 mods_filter(unsigned fileID, u8 **data, u32 size)
{
    if (g_prev_filter)
        size = g_prev_filter(fileID, data, size);
    build_overrides();
    std::unordered_map<unsigned, std::string>::const_iterator it = g_override.find(fileID);
    if (it == g_override.end())
        return size;
    u32 len = 0;
    u8 *b = read_override(it->second, &len);
    if (!b) {
        if (!g_said[fileID]++)
            fprintf(stderr, "[mods] %s could not be read or decoded (bad LZ77?); file id %u keeps the stock file\n",
                    it->second.c_str(), fileID);
        return size;
    }
    if (*data) free(*data);
    *data = b;
    if (!g_said[fileID]++)
        fprintf(stderr, "[mods] serving %s for file id %u (%u bytes in place of %u)\n",
                it->second.c_str(), fileID, len, size);
    return len;
}

void mods_claims(void (*claim)(unsigned fileID))
{
    build_overrides();
    for (std::unordered_map<unsigned, std::string>::const_iterator it = g_override.begin();
         it != g_override.end(); ++it)
        claim(it->first);
    if (g_prev_claims)
        g_prev_claims(claim);
}

/* textures_hd: the i-th directory, highest priority (latest mod) first. */
int mods_tex_dir(int i, char *out, unsigned cap)
{
    latch();
    const int n = (int)g_mods.size();
    if (i < 0 || i >= n) return 0;
    const std::string d = g_mods[n - 1 - i].folder + "/textures_hd";
    if (d.size() + 1 > cap) return 0;
    memcpy(out, d.c_str(), d.size() + 1);
    return 1;
}

/* Runs after every ordinary initializer, hal/fs_mods.cpp's and (.CRT$XCV)
   hal/stage_geom.cpp's included. */
void __cdecl mods_install(void)
{
    g_prev_filter = port_fs_mod_filter;
    port_fs_mod_filter = mods_filter;
    g_prev_claims = port_fs_mod_claims;
    port_fs_mod_claims = mods_claims;
    ntr::hdtex_mod_dir_hook = mods_tex_dir;
}

}  /* namespace */

extern "C" {
int port_mods_count(void) { latch(); return (int)g_mods.size(); }
const char *port_mods_id(int i) { latch(); return i >= 0 && i < (int)g_mods.size() ? g_mods[i].id.c_str() : 0; }
const char *port_mods_folder(int i) { latch(); return i >= 0 && i < (int)g_mods.size() ? g_mods[i].folder.c_str() : 0; }
const char *port_mods_kind(int i) { latch(); return i >= 0 && i < (int)g_mods.size() ? g_mods[i].kind.c_str() : 0; }
const char *port_mods_game(int i) { latch(); return i >= 0 && i < (int)g_mods.size() ? g_mods[i].game.c_str() : 0; }
const char *port_mods_min_game(int i) { latch(); return i >= 0 && i < (int)g_mods.size() ? g_mods[i].min_game.c_str() : 0; }
int port_mods_api(int i) { latch(); return i >= 0 && i < (int)g_mods.size() ? g_mods[i].api : 0; }
const char *port_mods_bundle(void) { latch(); return g_bundle.c_str(); }
}

#pragma section(".CRT$XCW", read)
extern "C" __declspec(allocate(".CRT$XCW"))
    void (__cdecl *port_mods_ctor)(void) = mods_install;
#if defined(_M_IX86)
#pragma comment(linker, "/include:_port_mods_ctor")
#else
#pragma comment(linker, "/include:port_mods_ctor")
#endif

/* THE HD UI FILE LOOKUP (run hunt8, lane HDMETER1; shared by every piece of HD UI art a mod can replace).
   A mod's hd_ui/<rel> is a file the port draws instead of its own art: the "HD UI Textures" mod carries them. It sits NEXT TO files/,
   not inside it, because files/ means "this replaces that ROM file" and would answer "names no ROM file" for art the ROM does not have.
   <rel> is a plain relative path ("hud/power_meter/meter_health3_with_label.png"): letters, digits, - _ . and /, no "..", no leading
   slash, no drive colon. Returns 1 and fills out with the full path of the file from the LAST enabled mod that has it (a later mod wins,
   a part folder counts as its own entry), 0 when no enabled mod provides it, which is every run in a lobby or match without an Online
   Compatible mod that has it (only those stay in the list there) and every run with no such mod: the caller then draws the port's own art, unchanged. The list is boot-latched, so this is
   a plain directory probe per call; callers cache what they decode. Only the game targets link this file. */
extern "C" int port_mods_ui_file(const char *rel, char *out, unsigned cap)
{
    if (!rel || !out || !cap) return 0;
    out[0] = 0;
    if (!rel[0] || rel[0] == '/' || strstr(rel, "..") || strchr(rel, ':') || strchr(rel, '\\') || strlen(rel) > 200) return 0;
    for (int i = port_mods_count() - 1; i >= 0; --i) {
        const char *dir = port_mods_folder(i);
        if (!dir) continue;
        const std::string path = std::string(dir) + "/hd_ui/" + rel;
        const DWORD a = GetFileAttributesA(path.c_str());
        if (a == INVALID_FILE_ATTRIBUTES || (a & FILE_ATTRIBUTE_DIRECTORY)) continue;
        if (path.size() >= cap) return 0;
        memcpy(out, path.c_str(), path.size() + 1);
        return 1;
    }
    return 0;
}

/* run hunt8, lane UIMOD1: hal/ui_art.cpp's lookup answers through this file when it is linked (the game targets); the smoke targets leave
   the hook null. Called from the menu art worker thread too: the list is boot-latched and the probe is a read-only attribute query. */
extern "C" int (*port_ui_file_hook)(const char *rel, char *out, unsigned cap);
namespace {
struct UiFileHookSet { UiFileHookSet() { port_ui_file_hook = port_mods_ui_file; } } g_ui_file_hook_set;
}

/* ==== THE RE-LATCH AT THE TITLE (run after093, lane SEAMMODS1; one-process plan M3a) ===========================================
   port_mods_relatch_at_title switches the mods FILE LAYER in place, at the title, with no level loaded: the list is read again
   (EnabledMods, and the multiplayer rule through port_mods_multiplayer as it stands now), the files/ overrides are built again,
   the card image serves the new list (hal/fs_names.cpp port_nitrofs_patch_relatch, after hal/fs.cpp and hal/fs_mods.cpp forget
   what the last latch emitted and read), the textures_hd index is built again and gx.cpp's decode cache dropped with the GPU
   generation it feeds. What is held at the title cannot be switched here, so the call REFUSES, changing nothing, and the caller
   restarts as before:
     - a code mod on either side (a plugin DLL cannot be unloaded);
     - a change in which mods ship hd_ui/ while the settings page or the character door is open (their pictures are on screen);
       otherwise the seven menu art caches that decode hd_ui/ once are reset below (lane SEAMART1) and the next draw decodes the
       new list's pictures;
     - a character (mod.json "character" or character/);
     - a files/ change to a file the game read before the title or while the title is live (the title holds its own files).
   The game says which in one line. 1 = switched, 0 = refused. Game thread only. */
extern "C" int port_scene_live_id(void);                         /* hal/scene_boot.cpp: 1 = SCENE_TITLE, -1 none */
extern "C" unsigned long port_fs_mod_relatch_reset(void);        /* hal/fs.cpp */
extern "C" void port_fs_cache_size(unsigned long *files, unsigned long *bytes);
extern "C" void port_fs_mods_relatch_reset(void);                /* hal/fs_mods.cpp */
extern "C" void port_nitrofs_patch_relatch(void);                /* hal/fs_names.cpp */
extern "C" void (*port_nitrofs_census_hook)(unsigned file_id);   /* hal/fs_names.cpp */
extern "C" void (*port_sdat_frame_hook)(void);                   /* hal/sdat/consumer.cpp */
namespace ntr { int hdtex_reindex(void); void gx_invalidate_textures(); }
extern "C" int sdat_reseat_mods(const char *reason);           /* hal/sdat/sdat.cpp (SEAMSND1) */
/* THE HD UI ART CACHES (lane SEAMART1, one-process plan M3c): every file that decodes hd_ui/ art keeps it for the life of the process; each
   has a reset that drops what it decoded and re-arms its first-use load, so the next draw reads the new list. Two of them decode on a worker
   thread: the quiesce calls wait for that worker to be idle before the list changes under it. All of these files link with this one (the
   two game targets). */
extern "C" int port_menu_hostimg_hd_ui_quiesce(void);           /* hal/menu_hostimg.cpp: title, pause, game over, star select art */
extern "C" int port_settings_page_hd_ui_quiesce(void);          /* hal/settings_page.cpp */
extern "C" int port_menu_hostimg_hd_ui_reset(void);
extern "C" int port_settings_page_hd_ui_reset(void);
extern "C" int port_menu_pause_hd_ui_reset(void);               /* hal/menu_recipe_pause.cpp: planks or the ROM's plates */
extern "C" int port_sub_screen_hd_ui_reset(void);               /* hal/sub_screen.cpp: the minimap panel */
extern "C" int port_ui_text_hd_ui_reset(void);                  /* hal/ui_text.cpp: the mod font */
extern "C" int port_hd_meter_hd_ui_reset(void);                 /* hal/hd_meter.cpp: the HD power meter */
extern "C" int port_char_door_hd_ui_reset(void);                /* hal/char_door_skin.cpp: the character door's skin */
extern "C" int settings_page_open(void);
extern "C" int port_char_door_open(void);

namespace {

/* WHAT THE GAME HAS READ, BY FAT ID AND WHEN. Every card block the ROM reads names the files it covers (hal/fs_names.cpp); the
   phase is the boot (before the title was ever live), the title (scene 1 live) or later (a level or another scene). It is what the
   held-file refusal reads, so it runs in every build, the kit included (lane SEAMART2): the relatch's callers (the in-place leave of an
   online world, the join) are kit code, and a kit with no record would accept every files/ change. One byte per FAT id and an OR per
   card block. Only the per-id block counters, the SM64DS_FS_CENSUS dump and the knobs below are dev only. The first card read after the
   title says the record's two counts once (boot, title), in every build: the kit's proof that its record is the dev build's. */
enum { RD_BOOT = 1, RD_TITLE = 2, RD_LATER = 4, FAT_IDS = 4096 };
unsigned char g_read_phase[FAT_IDS];
volatile int g_title_seen;
volatile int g_record_said;
#ifndef PORT_ROM_CLEAN
unsigned g_read_count[FAT_IDS];
#endif

void record_say(void)
{
    unsigned nb = 0, nt = 0;
    for (unsigned i = 0; i < FAT_IDS; ++i) {
        if (g_read_phase[i] & RD_BOOT) ++nb;
        if (g_read_phase[i] & RD_TITLE) ++nt;
    }
    fprintf(stderr, "[mods] file record at the end of the title: %u file id(s) read at boot, %u while the title was live\n", nb, nt);
    fflush(stderr);
}

void census_note(unsigned id)
{
    if (id >= FAT_IDS) return;
    const int sc = port_scene_live_id();
    if (sc == 1) g_title_seen = 1;
    else if (g_title_seen && !g_record_said) { g_record_said = 1; record_say(); }
    g_read_phase[id] |= sc == 1 ? RD_TITLE : (g_title_seen ? RD_LATER : RD_BOOT);
#ifndef PORT_ROM_CLEAN
    ++g_read_count[id];
#endif
}

struct NoteSet { NoteSet() { port_nitrofs_census_hook = census_note; } } g_note_set;

/* SEAMHUB1 (SEAMMODS1 note c): a relatch at the title AFTER A LEVEL (the in-place leave of an online world) cannot tell which of the
   files a level read the game still keeps across the return to the title, so on that call every one of them counts as held, a loose
   file or an archive member alike (lane SEAMART2). port_mods_relatch_after_level raises it for the one call. */
int g_after_level;

#ifndef PORT_ROM_CLEAN
/* SM64DS_MODS_FORCE_BUSY=menu|settings (dev only): the relatch treats that art worker as not idle, the case a decode longer than 2 s
   makes, so the refusal below can be shown on demand. */
const char *g_force_busy;
#endif

int held_mask(void) { return RD_BOOT | RD_TITLE | (g_after_level ? RD_LATER : 0); }

/* ARCHIVE MEMBERS (ids >= 0x8000, lane SEAMART1): a card block read inside a NARC names the NARC's own FAT id, so a member override is
   held when the archive it lives in was read at boot or while the title is live (or, after a level, in a level). The level boot,
   dScStage_c::InitResources (src/_ZN10dScStage_c13InitResourcesEv.cpp:255-263), remounts c2d and the language text archive
   (LoadTextNarcs: archive 6 = c2d, then one of 8..12 = cee cef ceg cei ces by language), arc0 (LoadArchive(0)), ar1 on level 1
   and the VS archive of its level, after the title's UnloadArchives dropped every mount; LoadArchive mounts only (header and FAT)
   and members are read on demand. So the mount is not
   what holds a member: what the boot DECODED out of an archive does. c2d (2D art) and cee (the text and font) are decoded at boot
   (about 133 and 94 card blocks) into heaps that outlive the title, so their members are held; cef shows one boot block and is
   refused the same way (an over-refusal: an English game decodes nothing from it). arc0 is the exception: its boot reads are its
   header and FAT only, and the next level boot reads its members off the card again, so a member override there reaches that level
   whole (REVSEAMMODS1's arc0 rows: on -> off and off -> on == fresh).
   port_archive_map is the ROM's mount table (index = LoadArchive's idx; [base, end) member ids; the NARC's path). */
struct port_arc_entry { unsigned short base, end; const char *narc; };
extern "C" struct port_arc_entry port_archive_map[13];
enum { ARC_COUNT = 13, ARC_PER_LEVEL = 0 };

unsigned narc_fat_id(int a)
{
    static unsigned id[ARC_COUNT];
    static bool done;
    if (!done) {
        done = true;
        for (unsigned i = 0; i < FAT_IDS; ++i) {
            const char *cp = port_fs_catalog_path(i);
            if (!cp) continue;
            for (int k = 0; k < ARC_COUNT; ++k)
                if (port_archive_map[k].narc && !strcmp(cp, port_archive_map[k].narc)) id[k] = i;
        }
    }
    return a >= 0 && a < ARC_COUNT ? id[a] : 0;
}

int archive_of(unsigned id)
{
    for (int k = 0; k < ARC_COUNT; ++k)
        if (id >= port_archive_map[k].base && id < port_archive_map[k].end) return k;
    return -1;
}

bool held_at_title(unsigned id)
{
    if (id < FAT_IDS) return (g_read_phase[id] & held_mask()) != 0;
    const int a = archive_of(id);
    if (a < 0 || a == ARC_PER_LEVEL) return false;
    const unsigned narc = narc_fat_id(a);
    return narc && (g_read_phase[narc] & held_mask()) != 0;
}

/* how a held id was read, for the refusal line: "at boot" / "at the title" / "in a level", through its archive for a member */
const char *held_when(unsigned id)
{
    const unsigned f = id < FAT_IDS ? id : narc_fat_id(archive_of(id));
    if (f >= FAT_IDS) return "at the title";
    return (g_read_phase[f] & RD_BOOT) ? "at boot" : (g_read_phase[f] & RD_TITLE) ? "at the title" : "in a level";
}

/* the path of a FAT id, or of a member: "<narc> member <n>" */
std::string held_path(unsigned id)
{
    if (id < FAT_IDS) {
        const char *cp = port_fs_catalog_path(id);
        return cp ? cp : "?";
    }
    const int a = archive_of(id);
    if (a < 0) return "an archive member";
    char b[200];
    snprintf(b, sizeof b, "%s member %u", port_archive_map[a].narc ? port_archive_map[a].narc : "?", id - port_archive_map[a].base);
    return b;
}

/* the ids of the mods in l that ship folder sub (a part counts as its own entry), joined in order */
std::string ids_with(const std::vector<Mod> &l, const char *sub)
{
    std::string r;
    for (size_t i = 0; i < l.size(); ++i)
        if (dir_exists(l[i].folder + sub)) r += (r.empty() ? "" : ",") + l[i].id;
    return r;
}

std::string ids_with_character(const std::vector<Mod> &l)
{
    std::string r;
    for (size_t i = 0; i < l.size(); ++i) {
        bool c = dir_exists(l[i].folder + "/character");
        if (!c && !l[i].part) {
            char *t = slurp_small(l[i].folder + "/mod.json");
            if (t) { c = json_top_value(t, "character") != 0; free(t); }
        }
        if (c) r += (r.empty() ? "" : ",") + l[i].id;
    }
    return r;
}

std::string list_text(const std::vector<Mod> &l)
{
    std::string r;
    for (size_t i = 0; i < l.size(); ++i) r += (i ? ", " : "") + l[i].id;
    return r.empty() ? "-" : r;
}

std::vector<Mod> g_mods_prev;   /* the list before the last relatch, kept one relatch long for a reader mid-call */

int g_force_held = 0;   /* SEAMCLEAN1: the title-time restart failed; accept a file a level read (see port_mods_relatch_forced) */

int relatch(const char *reason, const char *list_json)
{
    char why[600] = "";
    const int sc = port_scene_live_id();
    if (!reason) reason = "?";
    if (sc != 1) {
        fprintf(stderr, "[mods] relatch at the title refused (%s): the title is not the live scene (scene %d); the caller restarts\n", reason, sc);
        fflush(stderr);
        return 0;
    }
    /* the art workers read the mod list (port_mods_ui_file) while they decode: let them finish before it changes. A worker that is
       still busy after the bounded wait (about 2 s) may be reading g_mods right now, so the list is not touched at all: refused before
       the first rebuild (lane SEAMART2; the old list, overrides and pictures stay as they are). */
    int menu_idle = port_menu_hostimg_hd_ui_quiesce(), page_idle = port_settings_page_hd_ui_quiesce();
#ifndef PORT_ROM_CLEAN
    if (g_force_busy && !strcmp(g_force_busy, "menu")) menu_idle = 0;
    if (g_force_busy && !strcmp(g_force_busy, "settings")) page_idle = 0;
#endif
    if (!menu_idle || !page_idle) {
        const char *who = !menu_idle ? "menu art" : "settings page";
        fprintf(stderr, "[seamless] in place refused: the %s decode worker is still busy; restarting\n", who);
        fprintf(stderr, "[mods] relatch at the title refused (%s): the %s decode worker did not go idle within 2 s; nothing changed "
                "(the list was not rebuilt), the caller restarts\n", reason, who);
        fflush(stderr);
        return 0;
    }
    latch();
    build_overrides();

    std::vector<Mod> old_mods = g_mods;
    std::unordered_map<unsigned, std::string> old_override = g_override;
    const std::string old_loaded = g_summary_loaded, old_skipped = g_summary_skipped, old_bundle = g_bundle;
    const int old_nskipped = g_nskipped;

    g_mods.clear();
    g_summary_loaded.clear();
    g_summary_skipped.clear();
    g_nskipped = 0;
    g_latched = 0;
    g_relatching = 1;
    g_list_override = list_json;
    std::string held_log;          /* the new list's lines: printed below only if the switch is accepted (once, not twice) */
    g_log_hold = &held_log;
    latch();
    g_list_override = 0;
    g_override.clear();
    g_override_built = 0;
    build_overrides();
    g_log_hold = 0;
    g_relatching = 0;

    for (size_t i = 0; i < old_mods.size() && !why[0]; ++i)
        if (old_mods[i].kind == "code") snprintf(why, sizeof why, "code mod %s is on now", old_mods[i].id.c_str());
    for (size_t i = 0; i < g_mods.size() && !why[0]; ++i)
        if (g_mods[i].kind == "code") snprintf(why, sizeof why, "code mod %s would be on", g_mods[i].id.c_str());
    /* sound/ is not on this list any more (lane SEAMSND1): a change in the mods shipping it re-seats the sound archive below,
       hal/sdat/sdat.cpp sdat_reseat_mods, with the output device kept open. Nor is hd_ui/ (lane SEAMART1): a change in the mods
       shipping it resets the seven art caches below, unless a page drawing that art is open now (a busy art worker refused above). */
    /* SEAMCLEAN1, DECISION (review item 9): the hd_ui art caches reset when the SET of mods shipping hd_ui/ changes (this comparison
       is by mod id). A mod that keeps its id but changes its hd_ui files on disk between two relatches keeps its old decoded pictures
       until the process ends or the set changes. That is documented, not hashed: the mods folder is read at boot and a player edits
       a mod with the game closed; hashing every hd_ui file at each relatch would cost a full read of the art on every online
       leave and join, for a case no flow of the game creates. */
    const std::string ui_old = ids_with(old_mods, "/hd_ui"), ui_new = ids_with(g_mods, "/hd_ui");
    if (!why[0] && ui_old != ui_new) {
        const char *busy = settings_page_open() ? "the settings page is open" : port_char_door_open() ? "the character door is open" : 0;
        if (busy)
            snprintf(why, sizeof why, "the mods shipping hd_ui/ change (%s -> %s) and %s", ui_old.empty() ? "-" : ui_old.c_str(),
                     ui_new.empty() ? "-" : ui_new.c_str(), busy);
    }
    if (!why[0]) {
        const std::string a = ids_with_character(old_mods), b = ids_with_character(g_mods);
        if (a != b)
            snprintf(why, sizeof why, "the character mods change (%s -> %s)", a.empty() ? "-" : a.c_str(), b.empty() ? "-" : b.c_str());
    }
    unsigned changed = 0;
    if (!why[0]) {
        std::unordered_map<unsigned, std::string> all = old_override;
        for (std::unordered_map<unsigned, std::string>::const_iterator it = g_override.begin(); it != g_override.end(); ++it)
            all[it->first];
        for (std::unordered_map<unsigned, std::string>::const_iterator it = all.begin(); it != all.end() && !why[0]; ++it) {
            std::unordered_map<unsigned, std::string>::const_iterator o = old_override.find(it->first), n = g_override.find(it->first);
            const std::string ov = o == old_override.end() ? "" : o->second, nv = n == g_override.end() ? "" : n->second;
            if (ov == nv) continue;
            ++changed;
            if (held_at_title(it->first)) {
                if (g_force_held) {
                    fprintf(stderr, "[mods] forced: file id %u (%s) was read %s; the new list serves it from its next read\n", it->first,
                            held_path(it->first).c_str(), held_when(it->first));
                    continue;
                }
                snprintf(why, sizeof why, "files/ changes file id %u (%s), which the game read %s and still holds", it->first,
                         held_path(it->first).c_str(), held_when(it->first));
            }
        }
    }
    if (why[0]) {
        g_mods = old_mods;
        g_override = old_override;
        g_summary_loaded = old_loaded;
        g_summary_skipped = old_skipped;
        g_bundle = old_bundle;
        g_nskipped = old_nskipped;
        g_latched = 1;
        g_override_built = 1;
        fprintf(stderr, "[mods] relatch at the title refused (%s): %s; nothing changed, the caller restarts\n", reason, why);
        fflush(stderr);
        return 0;
    }
    fputs(held_log.c_str(), stderr);

    const std::string snd_old = ids_with(old_mods, "/sound"), snd_new = ids_with(g_mods, "/sound");
    g_said.clear();
    const unsigned long dropped = port_fs_mod_relatch_reset();
    port_fs_mods_relatch_reset();
    port_nitrofs_patch_relatch();
    const int tex = ntr::hdtex_reindex();
    if (ui_old != ui_new) {
        const int a = port_menu_hostimg_hd_ui_reset(), b = port_settings_page_hd_ui_reset(), c = port_menu_pause_hd_ui_reset(),
                  d = port_sub_screen_hd_ui_reset(), e = port_ui_text_hd_ui_reset(), f = port_hd_meter_hd_ui_reset(),
                  g = port_char_door_hd_ui_reset();
        fprintf(stderr, "[mods] hd_ui art reset (%s -> %s): menu pictures %d, settings pictures %d, pause planks %d, minimap panel %d, "
                "mod font %d, power meter %d, door skin %d dropped; each loads again from the new list on its next draw\n",
                ui_old.empty() ? "-" : ui_old.c_str(), ui_new.empty() ? "-" : ui_new.c_str(), a, b, c, d, e, f, g);
    }
    ntr::gx_invalidate_textures();     /* after the art resets: the GPU drops every texture the old pictures fed */
    if (snd_old != snd_new) {
        char sr[160];
        snprintf(sr, sizeof sr, "%s; sound mods %s -> %s", reason, snd_old.empty() ? "-" : snd_old.c_str(),
                 snd_new.empty() ? "-" : snd_new.c_str());
        sdat_reseat_mods(sr);
    }
    g_mods_prev.swap(old_mods);
    PROCESS_MEMORY_COUNTERS pmc;
    memset(&pmc, 0, sizeof pmc);
    K32GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof pmc);
    fprintf(stderr, "[mods] relatched at the title (%s): %u mod(s) %s (was %s); %u ROM file id(s) overridden (was %u, %u changed); "
            "fs cache %lu byte(s) dropped; textures_hd %d name(s) indexed; working set %lu KB\n", reason, (unsigned)g_mods.size(),
            list_text(g_mods).c_str(), list_text(g_mods_prev).c_str(), (unsigned)g_override.size(), (unsigned)old_override.size(), changed,
            dropped, tex, (unsigned long)(pmc.WorkingSetSize / 1024));
    fflush(stderr);
    return 1;
}

/* ==== THE SKIN IN PLACE (run after093, lane SEAMSND1; one-process plan M3b) ====================================================
   A skin is files and nothing else: char_registry's list of the base character's body and head files, served on top of every
   files/ entry (build_overrides above), plus the HUD pictures hal/char_hud.cpp decodes for it. The cartridge reads a character's
   files when a level loads it (Player::InitResources, under Stage::InitResources), so a skin can be switched without a restart
   whenever no level holds the old one: the files layer is rebuilt for the new skin and the card image serves it, the same three
   steps as the relatch above with the mod list unchanged. port_char_reskin_now does that at once (the file pick at the title,
   the star select's pick once its course is chosen and the old level torn down); port_char_reskin_next_level checks the same
   things inside a level and marks the pick, and the switch happens at the next level change, at the point the ROM's
   Scene::BeforeCleanupResources runs (hal/sdat/consumer.cpp sd_sound_level_reap, after the old level's actors are gone and
   before the new level boots). Refused (0, *why says why, nothing changed; the caller restarts as before) when:
     - this is a multiplayer session (the base character is forced there);
     - the old or the new skin comes from a code mod (a plugin cannot be unloaded);
     - a skin file is one the game read at boot or at the title and still holds.
   Away from the title the archives are unmounted with the title's own UnloadArchives first (see reskin_now): the body files are
   arc0 members, and the ROM mounts an archive once. */
extern "C" void port_char_hud_reskin_reset(void);   /* hal/char_hud.cpp */

int g_reskin_pending_level;   /* 1 = the pending skin waits for the next level change */

int reskin_check(int idx, int away_from_title, char *why, unsigned cap)
{
    why[0] = 0;
    latch();
    build_overrides();
    if (port_mods_multiplayer()) {
        snprintf(why, cap, "a multiplayer session forces the base character");
        return 0;
    }
    const int cur = port_char_active();
    const int ends[2] = { cur, idx };
    for (int e = 0; e < 2; ++e) {
        const char *mid = port_char_mod_of(ends[e]);
        if (!mid) continue;
        for (size_t i = 0; i < g_mods.size(); ++i)
            if (g_mods[i].id == mid && g_mods[i].kind == "code") {
                snprintf(why, cap, "the %s skin comes from code mod %s, and a plugin cannot be unloaded", e ? "new" : "running", mid);
                return 0;
            }
    }
    (void)away_from_title;
    return 1;
}

/* The archive table the ROM's LoadArchive / UnloadArchives walk (src/LoadArchive.c, src/UnloadArchives.c): 13 records of five words,
   the first the mounted archive (0 = not mounted). */
extern "C" int data_0208ecf4[];
extern "C" void UnloadArchives(void);

unsigned archives_mounted(void)
{
    unsigned n = 0;
    for (int i = 0; i < 13; ++i)
        if (data_0208ecf4[i * 5]) ++n;
    return n;
}

int reskin_now(int idx, const char *reason, int away_from_title, int dry, char *why, unsigned cap)
{
    if (!reason) reason = "?";
    if (!reskin_check(idx, away_from_title, why, cap)) {
        fprintf(stderr, "[chars] skin in place refused (%s): %s\n", reason, why);
        fflush(stderr);
        return 0;
    }
    const std::unordered_map<unsigned, std::string> old_override = g_override;
    const int was = port_char_select(idx);
    g_override.clear();
    g_override_built = 0;
    build_overrides();
    unsigned changed = 0;
    std::unordered_map<unsigned, std::string> all = old_override;
    for (std::unordered_map<unsigned, std::string>::const_iterator it = g_override.begin(); it != g_override.end(); ++it)
        all[it->first];
    for (std::unordered_map<unsigned, std::string>::const_iterator it = all.begin(); it != all.end() && !why[0]; ++it) {
        std::unordered_map<unsigned, std::string>::const_iterator o = old_override.find(it->first), n = g_override.find(it->first);
        const std::string ov = o == old_override.end() ? "" : o->second, nv = n == g_override.end() ? "" : n->second;
        if (ov == nv) continue;
        ++changed;
        if (held_at_title(it->first))
            snprintf(why, cap, "skin file id %u (%s) was read %s and is still held", it->first, held_path(it->first).c_str(),
                     held_when(it->first));
    }
    if (why[0] || dry) {
        port_char_select(was);
        g_override = old_override;
        g_override_built = 1;
        if (!why[0]) return 1;   /* the dry run of a pick inside a level: it can switch at the next level change */
        fprintf(stderr, "[chars] skin in place refused (%s): %s\n", reason, why);
        fflush(stderr);
        return 0;
    }
    /* THE PLAYER MODELS ARE ARCHIVE MEMBERS (arc0), and the ROM mounts an archive once: LoadArchive returns at once while its record
       is set, and only the title's UnloadArchives (src/game/stages/d_s_title.cpp) clears the records. At the title they are already
       clear. Away from it (a level change, the star select), the old level is torn down and the next one has not booted, so the
       records are cleared here with the title's own call, and the next Stage::InitResources mounts arc0 again off the card, where
       the new skin's members are served. Without this the level after the switch read the old mount: the skin did not show. */
    const unsigned mounted = archives_mounted();
    if (mounted) UnloadArchives();
    g_said.clear();
    port_fs_mod_relatch_reset();
    port_nitrofs_patch_relatch();
    port_char_hud_reskin_reset();
    g_reskin_pending_level = 0;
    if (mounted)
        fprintf(stderr, "[chars] %u archive(s) were mounted: unmounted with the title's own UnloadArchives, so the next level mounts "
                "them again with the new skin\n", mounted);
    PROCESS_MEMORY_COUNTERS pmc;
    memset(&pmc, 0, sizeof pmc);
    K32GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof pmc);
    fprintf(stderr, "[chars] skin switched in place (%s): %s -> %s, %u ROM file id(s) changed, no restart; working set %lu KB\n",
            reason, was >= 0 ? port_char_key(was) : "plain", idx >= 0 ? port_char_key(idx) : "plain", changed,
            (unsigned long)(pmc.WorkingSetSize / 1024));
    fflush(stderr);
    return 1;
}

/* hal/sdat/consumer.cpp's level reap, seated below: the old level's actors are gone, the next level has not booted */
void reskin_at_level_change(void)
{
    if (!g_reskin_pending_level) return;
    const int idx = port_char_skin_pending();
    g_reskin_pending_level = 0;
    if (idx == -2) return;
    char why[400];
    if (!reskin_now(idx, "the level change after a pick inside a level", 1, 0, why, sizeof why))
        fprintf(stderr, "[chars] the pending skin was not applied (%s); the running one stays\n", why);
}

}  /* namespace */

extern "C" void (*port_level_reap_hook)(void);   /* hal/sdat/consumer.cpp */

namespace {
struct ReapSet { ReapSet() { port_level_reap_hook = reskin_at_level_change; } } g_reap_set;

#ifndef PORT_ROM_CLEAN
/* ---- dev knobs, compiled out of the kit --------------------------------------------------------------------------------------
   SM64DS_MODS_RELATCH_AT=<tick>[,<id>+<id>...][;<tick>[,...]]...  at the title's <tick>th frame (counted from the first frame the
   title is the live scene) call the relatch; with a list, that list stands in for settings.json's EnabledMods ("-" = no mods).
   A tick written <tick>L calls it as port_mods_relatch_after_level (files a level read count as held).
   SM64DS_MODS_MARK_LATER=<id>[,<id>...]  marks those FAT ids read in a level, as if a level had read them before this title (no
   in-place return from a level to the title on this branch); with an L step it proves the after-level rule on a dev build.
   SM64DS_FS_CENSUS=<tick>  at that title frame (default 200) and at exit, print every FAT id the game has read, with when
   (boot / title / later) and how often, and the fs cache size. */
struct RelatchStep { int tick; bool has_list; bool after_level; std::string json; };
std::vector<RelatchStep> g_steps;
int g_census_tick = -1;
unsigned g_title_ticks;

void census_dump(const char *when)
{
    unsigned long files = 0, bytes = 0;
    port_fs_cache_size(&files, &bytes);
    unsigned nb = 0, nt = 0, nl = 0;
    for (unsigned i = 0; i < FAT_IDS; ++i) {
        if (g_read_phase[i] & RD_BOOT) ++nb;
        if (g_read_phase[i] & RD_TITLE) ++nt;
        if (g_read_phase[i] & RD_LATER) ++nl;
    }
    fprintf(stderr, "[census] %s: %u id(s) read at boot, %u at the title, %u later; fs cache %lu file(s) %lu byte(s); "
            "%u mod(s) on, %u id(s) overridden\n", when, nb, nt, nl, files, bytes, (unsigned)g_mods.size(), (unsigned)g_override.size());
    for (unsigned i = 0; i < FAT_IDS; ++i) {
        if (!g_read_phase[i]) continue;
        const char *cp = port_fs_catalog_path(i);
        fprintf(stderr, "[census]   id %4u %c%c%c blocks %6u %s%s\n", i, (g_read_phase[i] & RD_BOOT) ? 'B' : '-',
                (g_read_phase[i] & RD_TITLE) ? 'T' : '-', (g_read_phase[i] & RD_LATER) ? 'L' : '-', g_read_count[i], cp ? cp : "?",
                g_override.count(i) ? " (overridden)" : "");
    }
    fflush(stderr);
}

void census_at_exit(void) { census_dump("at exit"); }

void (*g_dev_prev_hook)(void) = 0;   /* SEAMCLEAN1 (item 32): the hook that was seated before this one, called first */
void dev_frame(void)
{
    if (g_dev_prev_hook) g_dev_prev_hook();
    if (port_scene_live_id() != 1) return;
    const unsigned t = g_title_ticks++;
    if ((int)t == g_census_tick) census_dump("at the title");
    for (size_t i = 0; i < g_steps.size(); ++i)
        if (g_steps[i].tick == (int)t) {
            char reason[64];
            snprintf(reason, sizeof reason, "SM64DS_MODS_RELATCH_AT title frame %u%s", t, g_steps[i].after_level ? " after a level" : "");
            if (g_steps[i].after_level) g_after_level = 1;
            const int r = relatch(reason, g_steps[i].has_list ? g_steps[i].json.c_str() : 0);
            g_after_level = 0;
            fprintf(stderr, "[mods] dev: relatch returned %d%s\n", r, r ? "" : " (a real caller restarts the game here)");
            fflush(stderr);
        }
}

std::vector<std::string> split(const std::string &s, char sep)
{
    std::vector<std::string> r;
    size_t at = 0;
    for (;;) {
        const size_t k = s.find(sep, at);
        r.push_back(s.substr(at, k == std::string::npos ? std::string::npos : k - at));
        if (k == std::string::npos) return r;
        at = k + 1;
    }
}

struct DevKnobs {
    DevKnobs()
    {
        const char *e = getenv("SM64DS_MODS_RELATCH_AT");
        if (e && *e) {
            const std::vector<std::string> steps = split(e, ';');
            for (size_t i = 0; i < steps.size(); ++i) {
                if (steps[i].empty()) continue;
                RelatchStep st;
                st.tick = atoi(steps[i].c_str());
                st.after_level = steps[i].find('L') != std::string::npos && steps[i].find('L') < steps[i].find(',');
                const size_t comma = steps[i].find(',');
                st.has_list = comma != std::string::npos;
                st.json = "[";
                if (st.has_list && steps[i].substr(comma + 1) != "-") {
                    const std::vector<std::string> ids = split(steps[i].substr(comma + 1), '+');
                    for (size_t k = 0; k < ids.size(); ++k)
                        if (!ids[k].empty()) st.json += (st.json.size() > 1 ? ",\"" : "\"") + ids[k] + "\"";
                }
                st.json += "]";
                g_steps.push_back(st);
            }
        }
        const char *ml = getenv("SM64DS_MODS_MARK_LATER");
        if (ml && *ml) {
            const std::vector<std::string> ids = split(ml, ',');
            for (size_t k = 0; k < ids.size(); ++k) {
                const unsigned id = (unsigned)strtoul(ids[k].c_str(), 0, 0);
                if (id < FAT_IDS) g_read_phase[id] |= RD_LATER;
            }
        }
        const char *fb = getenv("SM64DS_MODS_FORCE_BUSY");
        if (fb && *fb) g_force_busy = fb;
        const char *c = getenv("SM64DS_FS_CENSUS");
        if (c && *c && strcmp(c, "0")) {
            g_census_tick = atoi(c) > 1 ? atoi(c) : 200;
            atexit(census_at_exit);
        }
        if (!g_steps.empty() || g_census_tick >= 0)
        {
            g_dev_prev_hook = port_sdat_frame_hook;
            port_sdat_frame_hook = dev_frame;
        }
    }
} g_dev_knobs;
#endif

}  /* namespace */

extern "C" int port_mods_relatch_at_title(const char *reason) { return relatch(reason, 0); }

/* SEAMHUB1: the same relatch, for a title reached from a level (the in-place leave of an online world): files a level read count as
   held (note c above held_at_title). */
extern "C" int port_mods_relatch_after_level(const char *reason)
{
    g_after_level = 1;
    const int r = relatch(reason, 0);
    g_after_level = 0;
    return r;
}

/* SEAMCLEAN1: the last resort of an online leave whose relatch was refused and whose restart then failed to start: the player is on
   a title with the online mod list and no other way out, so the offline list goes on anyway, accepting files a level read (they show
   from their next read; the title holds no level). A code mod still refuses (a plugin cannot be unloaded). */
extern "C" int port_mods_relatch_forced(const char *reason)
{
    g_force_held = 1;
    const int r = port_mods_relatch_after_level(reason);
    g_force_held = 0;
    return r;
}

/* SEAMSND1: the skin in place (see THE SKIN IN PLACE above). port_char_reskin_now: switch now, where no level holds the old skin
   (away_from_title = 1 anywhere but the title). port_char_reskin_next_level: a pick inside a level; checks everything now (a dry run)
   and marks it for the next level change. 1 = done / marked; 0 = refused, *why says why, the caller restarts as before. */
extern "C" int port_char_reskin_now(int idx, const char *reason, int away_from_title, char *why, unsigned cap)
{
    return reskin_now(idx, reason, away_from_title, 0, why, cap);
}

extern "C" int port_char_reskin_next_level(int idx, char *why, unsigned cap)
{
    if (!reskin_now(idx, "a pick inside a level", 1, 1, why, cap)) return 0;
    port_char_skin_set_pending(idx);
    g_reskin_pending_level = 1;
    fprintf(stderr, "[chars] skin %s marked for the next level load (the cartridge loads the character with the level); this level "
            "keeps %s\n", idx >= 0 ? port_char_key(idx) : "plain", port_char_active() >= 0 ? port_char_key(port_char_active()) : "plain");
    fflush(stderr);
    return 1;
}
