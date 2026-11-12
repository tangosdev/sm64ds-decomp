/* hal/mod_plugins.cpp -- the plugin loader: code mods, API v1 (run mods1, lane
   MODPLUGIN1). The contract a mod author reads is mods_api/sm64ds_mod.h; this
   file is the game's half. hal/mods.cpp (MODFILES1) decides which mods are on;
   this file takes the code mods among them.

   WHEN IT RUNS. port_mod_plugins_load() is called from main() right after
   io_init and the comms install: the TLS callback in ntr/io.cpp has already
   claimed the DS address windows (a DLL loaded before that could land on them),
   and nothing has read a ROM file or registered an actor yet, so a plugin's
   on_file and replace_actor registrations are in place before they matter.

   WHO IS LOADED. A mod with kind "code", mod.json "api" within the range this
   game offers (SM64DS_MOD_API_MIN_VERSION to SM64DS_MOD_API_VERSION) and, when
   mod.json has "minGameVersion", a release (PORT_GAME_VERSION, the resource file's
   ProductVersion) at least that new, with exactly one DLL in mods/<id>/plugin/. Anything else is
   skipped and the boot summary line says why. SM64DS_MOD_PLUGINS=0 turns the
   plugins off alone (SM64DS_MODS=0 turns every mod off). A multiplayer session
   (the launcher's lobby / VS match env, see port_mods_multiplayer() in hal/mods.cpp, which empties the whole mod list) loads none: a
   plugin on one machine only would desync the match.

   WHERE EACH CALLBACK COMES FROM
     on_frame       hal/actor_registry.cpp port_actor_frame_end, the one point
                    both host loops (level, scene) reach once per frame after
                    the frame's actor work.
     on_level_load  the end of port_stage_a_boot (hal/level_boot.cpp), which
                    every level boot and every level change goes through.
     on_file        one more link on hal/fs.cpp's filter and claims chain,
                    installed after hal/mods.cpp's, so a plugin sees what the
                    files/ layer put there. Files the plugin names are CLAIMED,
                    so the card the ROM's own loaders read from serves the
                    edited bytes too; a file the plugin leaves unchanged moves
                    nothing (hal/fs.cpp drops a claim whose bytes equal stock).
     replace_actor  the spawn table (hal/actor_registry.cpp): the registry
                    rewrites SpawnInfo+0 for every class on every level boot,
                    so the choices are re-applied from the registry's hook
                    after each rewrite.

   ROLLBACK. Rollback re-simulation exists only in a multiplayer session
   (hal/rollback.cpp is armed by the netcode, SM64DS_NETMODE=rollback with a
   seated comms session) and no plugin is loaded there. The frame hook still
   skips a frame while g_port_rb_replaying is set, so a future single-player
   rewind could not run a plugin inside a re-simulation. F8/F9 save states
   restore the game's memory and not the plugin's: documented in the header.

   FAULTS. Every call into plugin code (main, callbacks, filters, factories)
   runs under __try/__except. A fault switches that plugin off for the rest of
   the run with one log line; its callbacks stop, and its replaced actors fall
   back to the factory that was in place before it. */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR 0x00000100
#endif
#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

#include "mods.h"
#include "mod_hooks.h"
#include "../mods_api/sm64ds_mod.h"

#ifndef PORT_GAME_VERSION
#define PORT_GAME_VERSION ""
#endif

typedef unsigned char u8;
typedef unsigned int u32;

extern "C" {
int host_json_string(const char *text, const char *key, char *out, size_t cap);
const char *port_fs_catalog_path(unsigned file_id);   /* hal/fs.cpp */
unsigned port_fs_interior_id(const char *want);       /* hal/fs.cpp */
extern u32 (*port_fs_mod_filter)(unsigned fileID, u8 **data, u32 size);
extern void (*port_fs_mod_claims)(void (*claim)(unsigned fileID));
extern void **data_020a4bb8;                          /* hal/actor_vtables.cpp */
extern int g_port_rb_replaying;                       /* hal/luigi_infection.cpp */
}

namespace {

enum { kMaxCb = 16, kMaxFactory = 32 };

struct FrameCb { SM64DSFrameFn fn; void *user; };
struct LevelCb { SM64DSLevelFn fn; void *user; };
struct FileCb { std::string path; SM64DSFileFn fn; void *user; unsigned id; };

struct Plugin {
    std::string id, folder;
    HMODULE dll;
    SM64DSModApi api;
    bool dead;
    std::vector<FrameCb> frame;
    std::vector<LevelCb> level;
    std::vector<FileCb> file;
    int nfactory;
    std::map<std::string, std::string> settings;
    std::string values;
    bool values_read;
    /* SEAMCODE1: the plugin's code patches (journaled around its main and its level callbacks) and its off switch */
    struct Patch { u8 *at; std::vector<u8> was, now; };
    std::vector<Patch> patches;
    bool off;
    Plugin() : dll(0), dead(false), nfactory(0), values_read(false), off(false) { memset(&api, 0, sizeof api); }
};

std::vector<Plugin *> g_plugins;
std::string g_loaded, g_skipped;
int g_nloaded, g_nskipped;

void say_skip(const std::string &id, const std::string &why)
{
    fprintf(stderr, "[plugins] skipped %s: %s\n", id.c_str(), why.c_str());
    if (g_nskipped++) g_skipped += ", ";
    g_skipped += id + " (" + why + ")";
}

/* ---- guarded calls: tiny functions, no C++ objects, so __try is legal ---- */

DWORD g_fault_code;

int fault_filter(unsigned long code)
{
    g_fault_code = code;
    return EXCEPTION_EXECUTE_HANDLER;
}

/* runs in the __except block, after the stack has unwound: a stack overflow
   consumed the guard page, and without this the next overflow (in any plugin,
   or in the game) is a plain crash */
void after_fault(void)
{
    if (g_fault_code == EXCEPTION_STACK_OVERFLOW)
        _resetstkoflw();
}

int g_main_rc;
typedef int (__cdecl *MainFn)(const SM64DSModApi *);

bool seh_main(MainFn fn, const SM64DSModApi *api)
{
    __try { g_main_rc = fn(api); return true; }
    __except (fault_filter(GetExceptionCode())) { after_fault(); return false; }
}

bool seh_frame(SM64DSFrameFn fn, void *user)
{
    __try { fn(user); return true; }
    __except (fault_filter(GetExceptionCode())) { after_fault(); return false; }
}

bool seh_level(SM64DSLevelFn fn, int level, void *user)
{
    __try { fn(level, user); return true; }
    __except (fault_filter(GetExceptionCode())) { after_fault(); return false; }
}

u32 g_file_ret;
bool seh_file(SM64DSFileFn fn, const char *path, u8 **data, u32 size, void *user)
{
    __try { g_file_ret = fn(path, data, size, user); return true; }
    __except (fault_filter(GetExceptionCode())) { after_fault(); return false; }
}

void *g_fac_ret;
bool seh_factory(SM64DSActorFactory fn)
{
    __try { g_fac_ret = fn(); return true; }
    __except (fault_filter(GetExceptionCode())) { after_fault(); return false; }
}

void kill_plugin(Plugin *p, const char *where)
{
    if (p->dead) return;
    p->dead = true;
    fprintf(stderr, "[plugins] %s faulted in %s (exception %08lx): switched off for the rest of this run\n",
            p->id.c_str(), where, (unsigned long)g_fault_code);
    fflush(stderr);
}

/* ---- the API the plugin sees ---- */

Plugin *of(const SM64DSModApi *api) { return (Plugin *)api->game_private; }

void __cdecl api_log(const SM64DSModApi *api, const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof buf - 1, fmt ? fmt : "", ap);
    va_end(ap);
    buf[sizeof buf - 1] = 0;
    fprintf(stderr, "[mod:%s] %s\n", of(api)->id.c_str(), buf);
    fflush(stderr);
}

char *slurp(const std::string &path)
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

const char *__cdecl api_setting(const SM64DSModApi *api, const char *key)
{
    Plugin *p = of(api);
    if (!key || !*key) return 0;
    if (!p->values_read) {
        p->values_read = true;
        char *t = slurp(p->folder + "/settings.values.json");
        if (t) { p->values = t; free(t); }
    }
    std::map<std::string, std::string>::const_iterator it = p->settings.find(key);
    if (it != p->settings.end()) return it->second.c_str();
    if (p->values.empty()) return 0;
    char out[1024];
    if (!host_json_string(p->values.c_str(), key, out, sizeof out)) return 0;
    return (p->settings[key] = out).c_str();
}

int __cdecl api_on_frame(const SM64DSModApi *api, SM64DSFrameFn cb, void *user)
{
    Plugin *p = of(api);
    if (!cb || (int)p->frame.size() >= kMaxCb) return 1;
    FrameCb c = { cb, user };
    p->frame.push_back(c);
    return 0;
}

int __cdecl api_on_level_load(const SM64DSModApi *api, SM64DSLevelFn cb, void *user)
{
    Plugin *p = of(api);
    if (!cb || (int)p->level.size() >= kMaxCb) return 1;
    LevelCb c = { cb, user };
    p->level.push_back(c);
    return 0;
}

int __cdecl api_on_file(const SM64DSModApi *api, const char *rom_path, SM64DSFileFn filter, void *user)
{
    Plugin *p = of(api);
    if (!rom_path || !*rom_path || !filter || (int)p->file.size() >= kMaxCb) return 1;
    FileCb c;
    c.path = rom_path;
    c.fn = filter;
    c.user = user;
    c.id = 0;
    p->file.push_back(c);
    return 0;
}

/* replace_actor: a pool of fixed thunks, one per replacement, because the
   game's factory has no argument to carry "which plugin" and a faulting
   factory must be guarded and fall back. */
struct FSlot { Plugin *p; SM64DSActorFactory fn; SM64DSActorFactory prev; };
FSlot g_fs[kMaxFactory];
int g_nfs;
struct Over { unsigned id; SM64DSActorFactory thunk; };
std::vector<Over> g_over;                 /* latest thunk per actor id */

void *run_slot(int n)
{
    FSlot &s = g_fs[n];
    if (!s.p->dead && !s.p->off) {   /* SEAMCODE1: a plugin switched off spawns the actor it replaced, as a dead one does */
        if (seh_factory(s.fn)) return g_fac_ret;
        kill_plugin(s.p, "an actor factory");
    }
    return s.prev ? s.prev() : 0;
}

template <int N> void *__cdecl fthunk(void) { return run_slot(N); }
#define T4(n) fthunk<n>, fthunk<n + 1>, fthunk<n + 2>, fthunk<n + 3>
#define T16(n) T4(n), T4(n + 4), T4(n + 8), T4(n + 12)
const SM64DSActorFactory kThunks[kMaxFactory] = { T16(0), T16(16) };

SM64DSActorFactory __cdecl api_replace_actor(const SM64DSModApi *api, unsigned id, SM64DSActorFactory factory)
{
    Plugin *p = of(api);
    if (!factory) return 0;
    if (p->nfactory >= kMaxCb || g_nfs >= kMaxFactory) {
        api_log(api, "replace_actor(%u): the replacement table is full (%d of %d for this plugin, %d of %d overall), nothing changed",
                id, p->nfactory, (int)kMaxCb, g_nfs, (int)kMaxFactory);
        return 0;
    }
    SM64DSActorFactory prev = 0;
    size_t at = g_over.size();
    for (size_t i = 0; i < g_over.size(); ++i)
        if (g_over[i].id == id) { prev = g_over[i].thunk; at = i; }
    if (!prev) prev = (SM64DSActorFactory)port_actor_registry_stock_factory(id);
    if (!prev) {
        api_log(api, "replace_actor(%u): the game has no such actor", id);
        return 0;
    }
    FSlot &s = g_fs[g_nfs];
    s.p = p;
    s.fn = factory;
    s.prev = prev;
    Over o = { id, kThunks[g_nfs] };
    if (at == g_over.size()) g_over.push_back(o); else g_over[at] = o;
    ++g_nfs;
    ++p->nfactory;
    return prev;
}

void registry_hook(void)
{
    for (size_t i = 0; i < g_over.size(); ++i) {
        void *info = data_020a4bb8 ? data_020a4bb8[g_over[i].id] : 0;
        if (info) *(void **)info = (void *)g_over[i].thunk;
    }
}

/* symbol(): name -> address from the table next to the exe */
std::unordered_map<std::string, unsigned> g_syms;
bool g_syms_tried;
unsigned g_sym_base;     /* the base the table was linked at */

bool hexval(const std::string &s, unsigned *v)
{
    if (s.empty() || s.size() > 8) return false;
    unsigned r = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (!isxdigit((unsigned char)s[i])) return false;
        r = r * 16 + (unsigned)(isdigit((unsigned char)s[i]) ? s[i] - '0' : (tolower(s[i]) - 'a' + 10));
    }
    *v = r;
    return true;
}

void load_syms(void)
{
    if (g_syms_tried) return;
    g_syms_tried = true;
    char exe[MAX_PATH];
    DWORD n = GetModuleFileNameA(0, exe, sizeof exe);
    if (!n || n >= sizeof exe) return;
    std::string base = exe;
    std::string dir = base.substr(0, base.find_last_of("\\/") + 1);
    base = base.substr(0, base.size() > 4 ? base.size() - 4 : base.size());
    g_sym_base = 0x400000;
    /* the table is named for the exe; a bundle that renamed the exe still finds
       the build's own name beside it. The map is the fallback. */
    bool from_tab = true;
    FILE *f = fopen((base + ".symtab").c_str(), "rb");
    if (!f) f = fopen((dir + "walk_window.symtab").c_str(), "rb");
    if (!f) { f = fopen((base + ".map").c_str(), "rb"); from_tab = false; }
    if (!f) { f = fopen((dir + "walk_window.map").c_str(), "rb"); from_tab = false; }
    if (!f) {
        fprintf(stderr, "[plugins] no walk_window.symtab or .map beside the exe: symbol() answers NULL\n");
        return;
    }
    /* the running image's identity, to compare with the table's */
    unsigned run_stamp = 0, run_size = 0;
    {
        const unsigned char *im = (const unsigned char *)GetModuleHandleA(0);
        const IMAGE_DOS_HEADER *dh = (const IMAGE_DOS_HEADER *)im;
        const IMAGE_NT_HEADERS32 *nt = (const IMAGE_NT_HEADERS32 *)(im + dh->e_lfanew);
        run_stamp = nt->FileHeader.TimeDateStamp;
        run_size = nt->OptionalHeader.SizeOfImage;
    }
    bool have_stamp = false, stamp_ok = true;
    char line[4096];
    while (fgets(line, sizeof line, f)) {
        if (from_tab) {
            if (line[0] == '#') {
                const char *b = strstr(line, "base=");
                if (b) { unsigned v; if (hexval(std::string(b + 5, strspn(b + 5, "0123456789abcdefABCDEF")), &v)) g_sym_base = v; }
                const char *ts = strstr(line, "stamp="), *sz = strstr(line, "size=");
                unsigned t, z;
                if (ts && sz && hexval(std::string(ts + 6, strspn(ts + 6, "0123456789abcdefABCDEF")), &t) &&
                    hexval(std::string(sz + 5, strspn(sz + 5, "0123456789abcdefABCDEF")), &z)) {
                    have_stamp = true;
                    stamp_ok = (t == run_stamp && z == run_size);
                }
                continue;
            }
            char *tab = strchr(line, '\t');
            if (!tab) continue;
            *tab = 0;
            unsigned a;
            if (hexval(std::string(tab + 1, strcspn(tab + 1, "\r\n")), &a))
                g_syms.insert(std::make_pair(std::string(line), a));
        } else {
            /* " 0001:0008b150       _name      0048c150 f   file.obj" */
            char sec[32], name[2048], addr[32];
            if (sscanf(line, " %31s %2047s %31s", sec, name, addr) != 3) continue;
            if (strlen(sec) != 13 || sec[4] != ':') continue;
            unsigned a;
            if (strlen(addr) != 8 || !hexval(addr, &a)) continue;
            g_syms.insert(std::make_pair(std::string(name), a));
        }
    }
    fclose(f);
    if (from_tab && (!have_stamp || !stamp_ok)) {
        fprintf(stderr, "[plugins] walk_window.symtab %s: it was made for another build, symbol() answers NULL\n",
                have_stamp ? "does not match this exe (timestamp or image size differ)" : "has no build stamp");
        g_syms.clear();
        return;
    }
    fprintf(stderr, "[plugins] symbol table: %u names from %s\n", (unsigned)g_syms.size(),
            from_tab ? "walk_window.symtab" : "walk_window.map");
}

void *__cdecl api_symbol(const SM64DSModApi *api, const char *name)
{
    (void)api;
    if (!name || !*name) return 0;
    load_syms();
    std::unordered_map<std::string, unsigned>::const_iterator it = g_syms.find(name);
    if (it == g_syms.end() && name[0] != '_' && name[0] != '?')
        it = g_syms.find(std::string("_") + name);
    if (it == g_syms.end()) return 0;
    return (void *)(size_t)(it->second - g_sym_base + (unsigned)(size_t)GetModuleHandleA(0));
}

/* every live api->alloc block and the size it was asked for: a file filter may
   only hand back a buffer that is in this set (and returns no more bytes than
   it holds), so a pointer from anywhere else never reaches the game's free() */
std::map<void *, size_t> g_allocs;

void *__cdecl api_alloc(const SM64DSModApi *api, size_t size)
{
    (void)api;
    void *r = malloc(size ? size : 1);
    if (r) g_allocs[r] = size;
    return r;
}

void __cdecl api_free(const SM64DSModApi *api, void *ptr)
{
    if (!ptr) return;
    std::map<void *, size_t>::iterator it = g_allocs.find(ptr);
    if (it == g_allocs.end()) {
        api_log(api, "free(%p): not a block from api->alloc (or already freed or handed to the game), ignored", ptr);
        return;
    }
    g_allocs.erase(it);
    free(ptr);
}

/* ---- the file chain ---- */

u32 (*g_prev_filter)(unsigned, u8 **, u32);
void (*g_prev_claims)(void (*)(unsigned));
bool g_resolved;

std::string lower(const std::string &s)
{
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i) r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}

void resolve_files(void)
{
    if (g_resolved) return;
    g_resolved = true;
    std::unordered_map<std::string, unsigned> loose;
    for (unsigned id = 0; id < 4096; ++id) {
        const char *cp = port_fs_catalog_path(id);
        if (cp) loose[lower(cp)] = id;
    }
    for (size_t i = 0; i < g_plugins.size(); ++i) {
        Plugin *p = g_plugins[i];
        for (size_t k = 0; k < p->file.size(); ++k) {
            FileCb &c = p->file[k];
            std::unordered_map<std::string, unsigned>::const_iterator it = loose.find(lower(c.path));
            c.id = it != loose.end() ? it->second : port_fs_interior_id(c.path.c_str());
            if (!c.id)
                fprintf(stderr, "[plugins] %s: on_file(%s) is not a ROM file, ignored\n",
                        p->id.c_str(), c.path.c_str());
        }
    }
    fflush(stderr);
}

u32 plugin_filter(unsigned fileID, u8 **data, u32 size)
{
    if (g_prev_filter)
        size = g_prev_filter(fileID, data, size);
    resolve_files();
    for (size_t i = 0; i < g_plugins.size(); ++i) {
        Plugin *p = g_plugins[i];
        if (p->dead || p->off) continue;
        for (size_t k = 0; k < p->file.size() && !p->dead; ++k) {
            FileCb &c = p->file[k];
            if (c.id != fileID) continue;
            u8 *old = *data;
            u8 *cur = old;
            /* the filter may edit in place and then fault or misbehave: keep
               the bytes it was given so the file can be put back */
            const std::vector<u8> pristine(old, old + size);
            if (seh_file(c.fn, c.path.c_str(), &cur, size, c.user)) {
                const u32 ret = g_file_ret;
                if (cur != old) {
                    /* only a live block from the API's alloc, holding at least
                       the bytes claimed, may replace ours */
                    std::map<void *, size_t>::iterator ai = g_allocs.find(cur);
                    if (ai == g_allocs.end() || ret > ai->second) {
                        fprintf(stderr, "[plugins] %s: filter for %s returned %s (%u bytes): ignored, file left as it was, plugin switched off\n",
                                p->id.c_str(), c.path.c_str(),
                                ai == g_allocs.end() ? "a buffer that is not from api->alloc" : "a size larger than its api->alloc block",
                                ret);
                        if (size) memcpy(old, &pristine[0], size);
                        *data = old;
                        p->dead = true;
                        continue;
                    }
                    g_allocs.erase(ai);        /* the game owns it from here */
                    free(old);
                    *data = cur;
                    size = ret;
                } else if (ret <= size) {
                    size = ret;
                } else {
                    fprintf(stderr, "[plugins] %s: filter for %s grew the file in place (%u > %u); ignored the new size\n",
                            p->id.c_str(), c.path.c_str(), ret, size);
                }
            } else {
                if (size) memcpy(old, &pristine[0], size);   /* undo a half-finished edit */
                *data = old;
                kill_plugin(p, "a file filter (file restored)");
            }
        }
    }
    return size;
}

void plugin_claims(void (*claim)(unsigned fileID))
{
    resolve_files();
    for (size_t i = 0; i < g_plugins.size(); ++i)
        for (size_t k = 0; k < g_plugins[i]->file.size() && !g_plugins[i]->off; ++k)
            if (g_plugins[i]->file[k].id)
                claim(g_plugins[i]->file[k].id);
    if (g_prev_claims)
        g_prev_claims(claim);
}

/* ---- frame / level ---- */

void frame_hook(void)
{
    if (g_port_rb_replaying) return;
    for (size_t i = 0; i < g_plugins.size(); ++i) {
        Plugin *p = g_plugins[i];
        if (p->off) continue;
        for (size_t k = 0; k < p->frame.size() && !p->dead; ++k)
            if (!seh_frame(p->frame[k].fn, p->frame[k].user))
                kill_plugin(p, "on_frame");
    }
}

void journal_take(void);
void journal_note(Plugin *p, const char *where);

/* SEAMCODE1: what each patched plugin's code holds as a level boots (a few bytes compared): the in-level proof that a plugin
   switched off left the game's own code and one switched on has its patches in place. One line per plugin with patches. */
void level_patch_line(int level_id)
{
    for (size_t i = 0; i < g_plugins.size(); ++i) {
        Plugin *p = g_plugins[i];
        if (p->patches.empty()) continue;
        unsigned plug = 0, game = 0;
        for (size_t k = 0; k < p->patches.size(); ++k) {
            const Plugin::Patch &pt = p->patches[k];
            if (!memcmp(pt.at, &pt.now[0], pt.now.size())) ++plug;
            else if (!memcmp(pt.at, &pt.was[0], pt.was.size())) ++game;
        }
        fprintf(stderr, "[plugins] level %d boots with %s %s: %u of %u patch run(s) hold the plugin's bytes, %u the game's own\n", level_id,
                p->id.c_str(), p->off ? "OFF" : "on", plug, (unsigned)p->patches.size(), game);
    }
}

void level_hook(int level_id)
{
    bool taken = false;
    level_patch_line(level_id);
    for (size_t i = 0; i < g_plugins.size(); ++i) {
        Plugin *p = g_plugins[i];
        if (p->off || p->dead || p->level.empty()) continue;
        if (!taken) { journal_take(); taken = true; }   /* SEAMCODE1: what a level callback patches is journaled */
        for (size_t k = 0; k < p->level.size() && !p->dead; ++k)
            if (!seh_level(p->level[k].fn, level_id, p->level[k].user))
                kill_plugin(p, "on_level_load");
        journal_note(p, "on_level_load");
    }
}

/* ---- loading ---- */

bool env_set(const char *name)
{
    const char *v = getenv(name);
    return v && *v;
}

/* the one DLL in mods/<id>/plugin/; empty + a reason when there is not exactly one */
std::string find_dll(const std::string &folder, std::string *why)
{
    WIN32_FIND_DATAA fd;
    const std::string dir = folder + "/plugin";
    HANDLE h = FindFirstFileA((dir + "/*.dll").c_str(), &fd);
    std::string found;
    int n = 0;
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            if (!n) found = dir + "/" + fd.cFileName;
            ++n;
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    if (n == 0) { *why = "no plugin/*.dll"; return ""; }
    if (n > 1) { *why = "more than one DLL in plugin/ (one per mod)"; return ""; }
    return found;
}

/* The plugin is loaded by its full path, and only the DLL's own folder and
   System32 are searched for what it imports (so a stray DLL in the game's
   folder or the working directory is never picked up in its place). Windows
   without those flags (before KB2533623) fall back to the altered search path,
   which is the same full-path load with the DLL's folder first. */
HMODULE load_plugin_dll(const std::string &dll)
{
    char full[MAX_PATH * 2];
    DWORD n = GetFullPathNameA(dll.c_str(), sizeof full, full, 0);
    std::string path = (n && n < sizeof full) ? std::string(full) : dll;
    for (size_t i = 0; i < path.size(); ++i)
        if (path[i] == '/') path[i] = '\\';
    const bool have_flags = GetProcAddress(GetModuleHandleA("kernel32.dll"), "AddDllDirectory") != 0;
    HMODULE h = 0;
    if (have_flags) {
        h = LoadLibraryExA(path.c_str(), 0, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!h && GetLastError() != ERROR_INVALID_PARAMETER) return 0;
    }
    if (!h) h = LoadLibraryExA(path.c_str(), 0, LOAD_WITH_ALTERED_SEARCH_PATH);
    return h;
}

/* Dotted release numbers ("0.9.1", "0.10", "0.9.2-beta"): compared number by
   number, a missing part counts as 0, a suffix after the numbers is ignored.
   Returns -1, 0, 1, or -2 when `b` (the plugin's minGameVersion) is not a release
   number (the plugin is then refused: a bound we cannot read is not met). An
   unreadable game release is treated as older than everything. */
bool parse_release(const std::string &s, std::vector<int> *out)
{
    size_t i = 0;
    if (i < s.size() && (s[i] == 'v' || s[i] == 'V')) ++i;
    if (i >= s.size() || !isdigit((unsigned char)s[i])) return false;
    for (;;) {
        long v = 0;
        int nd = 0;
        while (i < s.size() && isdigit((unsigned char)s[i])) {
            if (++nd > 6) return false;
            v = v * 10 + (s[i++] - '0');
        }
        out->push_back((int)v);
        if (i < s.size() && s[i] == '.' && i + 1 < s.size() && isdigit((unsigned char)s[i + 1])) { ++i; continue; }
        return true;
    }
}

int compare_release(const std::string &game, const std::string &min_game)
{
    std::vector<int> g, m;
    if (!parse_release(min_game, &m)) return -2;
    if (!parse_release(game, &g)) return -1;
    for (size_t k = 0; k < g.size() || k < m.size(); ++k) {
        const int a = k < g.size() ? g[k] : 0, b = k < m.size() ? m[k] : 0;
        if (a != b) return a < b ? -1 : 1;
    }
    return 0;
}

void load_one(int i, const std::string &release)
{
    const std::string id = port_mods_id(i);
    const std::string folder = port_mods_folder(i);
    /* THE RULE (design: out/MODPIN1/DESIGN.md). A plugin built for plugin API
       version M loads on any game that offers M: SM64DS_MOD_API_MIN_VERSION <= M
       <= SM64DS_MOD_API_VERSION. The release string in "game" is only a note of
       what the plugin was built on; it is not compared. "minGameVersion", when the
       plugin names one, is the oldest release it works on. */
    const int api = port_mods_api(i);
    if (api < SM64DS_MOD_API_MIN_VERSION || api > SM64DS_MOD_API_VERSION) {
        char b[96];
        if (api > SM64DS_MOD_API_VERSION)
            sprintf(b, "plugin api %d is newer than this game offers (%d)", api, SM64DS_MOD_API_VERSION);
        else if (api <= 0)
            sprintf(b, "mod.json names no plugin api");
        else
            sprintf(b, "plugin api %d is older than this game still supports (%d to %d)", api,
                    SM64DS_MOD_API_MIN_VERSION, SM64DS_MOD_API_VERSION);
        say_skip(id, b);
        return;
    }
    const std::string min_game = port_mods_min_game(i) ? port_mods_min_game(i) : "";
    if (!min_game.empty()) {
        const int c = compare_release(release, min_game);
        if (c == -2) {
            say_skip(id, std::string("minGameVersion \"") + min_game + "\" is not a release number");
            return;
        }
        if (c < 0) {
            say_skip(id, std::string("needs game ") + min_game + " or newer, this is \"" + release + "\"");
            return;
        }
    }
    std::string why;
    const std::string dll = find_dll(folder, &why);
    if (dll.empty()) { say_skip(id, why); return; }

    HMODULE h = load_plugin_dll(dll);
    if (!h) {
        char b[96];
        sprintf(b, "LoadLibrary failed (error %lu)", GetLastError());
        say_skip(id, b);
        return;
    }
    MainFn fn = (MainFn)GetProcAddress(h, "sm64ds_mod_main");
    if (!fn) fn = (MainFn)GetProcAddress(h, "_sm64ds_mod_main");
    if (!fn) {
        FreeLibrary(h);
        say_skip(id, "the DLL exports no sm64ds_mod_main");
        return;
    }

    Plugin *p = new Plugin();
    p->id = id;
    p->folder = folder;
    p->dll = h;
    SM64DSModApi &a = p->api;
    a.size = sizeof a;
    a.api_version = SM64DS_MOD_API_VERSION;
    a.game_release = PORT_GAME_VERSION;
    a.mod_id = p->id.c_str();
    a.mod_folder = p->folder.c_str();
    a.game_private = p;
    a.log = api_log;
    a.setting = api_setting;
    a.on_frame = api_on_frame;
    a.on_level_load = api_on_level_load;
    a.on_file = api_on_file;
    a.replace_actor = api_replace_actor;
    a.symbol = api_symbol;
    a.alloc = api_alloc;
    a.free = api_free;
    g_plugins.push_back(p);

    journal_take();
    const bool main_ok = seh_main(fn, &a);
    journal_note(p, "sm64ds_mod_main");
    if (!main_ok) {
        kill_plugin(p, "sm64ds_mod_main");
        say_skip(id, "faulted in sm64ds_mod_main");
        return;
    }
    if (g_main_rc != 0) {
        p->dead = true;
        char b[64];
        sprintf(b, "sm64ds_mod_main returned %d", g_main_rc);
        say_skip(id, b);
        return;
    }
    if (g_nloaded++) g_loaded += ", ";
    g_loaded += id;
}

/* ==== SEAMCODE1: A PLUGIN SWITCHED OFF AND ON IN PLACE (run coopfix1) ====================================================
   A coop, VS or online-world start at the title used to restart the whole game when a code mod was loaded ("a plugin cannot be
   unloaded"), because the multiplayer rule turns every code mod off (hal/mods.cpp online_ok) and nothing could take a plugin's
   changes back out of the running game. The DLL still is never unloaded; what it changed is undone instead:
     - CODE PATCHES. Both live code mods change the game by patching its code (Walljumps for Everyone NOPs a jump in
       sm64ds_mod_main; Special Triple Jump writes JMP / CALL sites in its first on_level_load). Around every call into a plugin's
       sm64ds_mod_main and on_level_load the game's read-only image sections (code and read-only data, never .data) are compared
       before and after: every byte run the call changed is journaled for that plugin (the bytes before and after). The game's own
       patches (coop_boss, coop_target, vtable seats) never run inside a plugin call, so they are never in a plugin's journal.
     - OFF: the journal is put back newest first (the bytes before the plugin wrote them), and the game stops calling the plugin:
       no on_frame, no on_level_load, no file filter, no file claim; a replaced actor spawns through the factory it replaced.
     - ON: the journal is written again oldest first and the calls resume. A plugin whose first on_level_load had not run yet (Special
       Triple Jump before any level) simply runs it at the next level load, as it would have.
   What cannot be switched (the caller keeps the restart): a plugin that registered an on_file filter (a file the game already read
   keeps the plugin's bytes), and a journal that could not be written back. Patches a plugin makes from on_frame or from an actor
   factory, or writes to writable data, are not journaled: neither live mod does that, and the plugin API rules say callbacks stay
   short; such a plugin would keep those changes while off. */
std::vector<std::pair<u8 *, size_t> > g_ranges;   /* the image's read-only sections */
std::vector<u8> g_snap;                          /* their bytes before the current plugin call */
bool g_ranges_read, g_journal_bad;

void ranges_read(void)
{
    if (g_ranges_read) return;
    g_ranges_read = true;
    u8 *im = (u8 *)GetModuleHandleA(0);
    const IMAGE_DOS_HEADER *dh = (const IMAGE_DOS_HEADER *)im;
    const IMAGE_NT_HEADERS32 *nt = (const IMAGE_NT_HEADERS32 *)(im + dh->e_lfanew);
    const IMAGE_SECTION_HEADER *sh = IMAGE_FIRST_SECTION(nt);
    size_t total = 0;
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const DWORD ch = sh[i].Characteristics;
        if (ch & IMAGE_SCN_MEM_WRITE) continue;
        if (!(ch & (IMAGE_SCN_CNT_CODE | IMAGE_SCN_CNT_INITIALIZED_DATA))) continue;
        if (!sh[i].Misc.VirtualSize) continue;
        g_ranges.push_back(std::make_pair(im + sh[i].VirtualAddress, (size_t)sh[i].Misc.VirtualSize));
        total += sh[i].Misc.VirtualSize;
    }
    g_snap.resize(total);
    fprintf(stderr, "[plugins] code patch journal: %u read-only section(s), %u KB compared around each plugin main and level callback\n",
            (unsigned)g_ranges.size(), (unsigned)(total / 1024));
}

void journal_take(void)
{
    ranges_read();
    size_t o = 0;
    for (size_t r = 0; r < g_ranges.size(); ++r) {
        memcpy(&g_snap[o], g_ranges[r].first, g_ranges[r].second);
        o += g_ranges[r].second;
    }
}

/* the runs that differ from the snapshot become p's patches (runs closer than 8 bytes are one); the snapshot takes the new bytes */
void journal_note(Plugin *p, const char *where)
{
    size_t o = 0;
    unsigned runs = 0, bytes = 0;
    for (size_t r = 0; r < g_ranges.size(); ++r) {
        u8 *base = g_ranges[r].first;
        const size_t n = g_ranges[r].second;
        size_t i = 0;
        while (i < n) {
            const size_t blk = n - i >= 64 ? 64 : n - i;
            if (!memcmp(base + i, &g_snap[o + i], blk)) { i += blk; continue; }
            if (base[i] == g_snap[o + i]) { ++i; continue; }
            size_t e = i + 1, same = 0;
            while (e < n && same < 8) {
                if (base[e] == g_snap[o + e]) ++same; else same = 0;
                ++e;
            }
            e -= same;
            Plugin::Patch pt;
            pt.at = base + i;
            pt.was.assign(&g_snap[o + i], &g_snap[o + i] + (e - i));
            pt.now.assign(base + i, base + e);
            memcpy(&g_snap[o + i], base + i, e - i);
            p->patches.push_back(pt);
            ++runs;
            bytes += (unsigned)(e - i);
            i = e;
        }
        o += n;
    }
    if (runs) {
        fprintf(stderr, "[plugins] %s: %s patched the game's code: %u run(s), %u byte(s) journaled (%u run(s) in all)\n", p->id.c_str(),
                where, runs, bytes, (unsigned)p->patches.size());
        fflush(stderr);
    }
}

bool write_code(u8 *at, const std::vector<u8> &b)
{
    DWORD old, tmp;
    if (!VirtualProtect(at, b.size(), PAGE_EXECUTE_READWRITE, &old)) return false;
    memcpy(at, &b[0], b.size());
    VirtualProtect(at, b.size(), old, &tmp);
    FlushInstructionCache(GetCurrentProcess(), at, b.size());
    return true;
}

Plugin *find_plugin(const char *id)
{
    for (size_t i = 0; i < g_plugins.size(); ++i)
        if (id && g_plugins[i]->id == id) return g_plugins[i];
    return 0;
}

std::vector<std::string> g_late_tried;   /* code mods the boot or a late load already tried (a skip is said once) */

void install_hooks(void)
{
    static bool done;
    if (done || g_plugins.empty()) return;
    done = true;
    g_prev_filter = port_fs_mod_filter;
    port_fs_mod_filter = plugin_filter;
    g_prev_claims = port_fs_mod_claims;
    port_fs_mod_claims = plugin_claims;
    port_mod_hook_frame = frame_hook;
    port_mod_hook_level = level_hook;
    port_mod_hook_registry = registry_hook;
}

}  /* namespace */

/* 0 = no plugin of that id was loaded in this process; 1 = loaded and on; 2 = loaded and switched off; 3 = loaded but dead (faulted or
   its main refused) */
extern "C" int port_mod_plugin_state(const char *id)
{
    Plugin *p = find_plugin(id);
    if (!p) return 0;
    if (p->off) return 2;
    return p->dead ? 3 : 1;
}

/* why the loaded plugin `id` cannot be switched off and on in place; 0 = it can (or nothing of that id is loaded) */
extern "C" const char *port_mod_plugin_switch_refusal(const char *id)
{
    Plugin *p = find_plugin(id);
    if (!p) return 0;
    if (g_journal_bad) return "a plugin's code patches could not be written back earlier in this run";
    if (!p->file.empty()) return "it edits ROM files (on_file), which the game may already hold";
    return 0;
}

/* switch the loaded plugin `id` off (on = 0) or on (on = 1) at the title; 1 = done (or already so), 0 = refused, nothing changed */
extern "C" int port_mod_plugin_switch(const char *id, int on, const char *why)
{
    Plugin *p = find_plugin(id);
    if (!p) return 0;
    if (!!on == !p->off) return 1;
    if (const char *r = port_mod_plugin_switch_refusal(id)) {
        fprintf(stderr, "[plugins] %s stays %s (%s): %s\n", p->id.c_str(), p->off ? "off" : "on", why ? why : "?", r);
        fflush(stderr);
        return 0;
    }
    unsigned bytes = 0, bad = 0;
    if (!on) {
        for (size_t k = p->patches.size(); k-- > 0;) {
            if (!write_code(p->patches[k].at, p->patches[k].was)) ++bad;
            bytes += (unsigned)p->patches[k].was.size();
        }
    } else {
        for (size_t k = 0; k < p->patches.size(); ++k) {
            if (!write_code(p->patches[k].at, p->patches[k].now)) ++bad;
            bytes += (unsigned)p->patches[k].now.size();
        }
    }
    /* the check a proof reads: every journaled run now holds the bytes it should */
    unsigned match = 0;
    for (size_t k = 0; k < p->patches.size(); ++k) {
        const std::vector<u8> &want = on ? p->patches[k].now : p->patches[k].was;
        if (!memcmp(p->patches[k].at, &want[0], want.size())) ++match;
    }
    if (bad) g_journal_bad = true;
    p->off = !on;
    if (g_ranges_read) journal_take();   /* the next journaled call compares against the image as it is now */
    fprintf(stderr, "[plugins] %s switched %s in place (%s): %u code patch run(s), %u byte(s) %s, %u of %u verified%s; its "
            "callbacks %s\n", p->id.c_str(), on ? "ON" : "OFF", why ? why : "?", (unsigned)p->patches.size(), bytes,
            on ? "written again" : "put back to the game's own", match, (unsigned)p->patches.size(), bad ? " (A WRITE FAILED)" : "",
            on ? "run again" : "are not called while it is off");
    fflush(stderr);
    return 1;
}

/* load the code mod at list index i in place (the title, after a relatch put it in the list), the way the boot loads it; 1 = it is
   loaded now (or was already), 0 = skipped (said once, like the boot's skip) */
extern "C" int port_mod_plugin_load_late(int i, const char *why)
{
    const char *id = port_mods_id(i);
    if (!id || !port_mods_kind(i) || strcmp(port_mods_kind(i), "code")) return 0;
    if (find_plugin(id)) return 1;
    for (size_t k = 0; k < g_late_tried.size(); ++k)
        if (g_late_tried[k] == id) return 0;
    g_late_tried.push_back(id);
    const char *off = getenv("SM64DS_MOD_PLUGINS");
    const std::string release = PORT_GAME_VERSION;
    if ((off && !strcmp(off, "0")) || release.empty()) return 0;
    const int before = g_nloaded;
    load_one(i, release);
    g_resolved = false;   /* its on_file paths are resolved on the next read */
    install_hooks();
    const bool ok = g_nloaded > before;
    fprintf(stderr, "[plugins] %s loaded in place at the title (%s): %s\n", id, why ? why : "?",
            ok ? "loaded and on (its main ran now; its level callbacks run from the next level)" : "skipped (the line above says why)");
    fflush(stderr);
    return ok ? 1 : 0;
}

extern "C" void port_mod_plugins_load(void)
{
    const int n = port_mods_count();
    int ncode = 0;
    for (int i = 0; i < n; ++i)
        if (!strcmp(port_mods_kind(i), "code")) ++ncode;
    if (!ncode) return;                       /* no code mods: nothing to say */

    const char *off = getenv("SM64DS_MOD_PLUGINS");
    if (off && !strcmp(off, "0")) {
        fprintf(stderr, "[plugins] SM64DS_MOD_PLUGINS=0: %d code mod(s) not loaded\n", ncode);
        return;
    }
    const std::string release = PORT_GAME_VERSION;
    if (release.empty()) {
        fprintf(stderr, "[plugins] this build has no release string: plugins are not loaded\n");
        return;
    }
    for (int i = 0; i < n; ++i)
        if (!strcmp(port_mods_kind(i), "code")) {
            g_late_tried.push_back(port_mods_id(i));
            load_one(i, release);
        }

    fprintf(stderr, "[plugins] %d loaded: %s | %d skipped%s%s (game %s, api %d)\n", g_nloaded,
            g_nloaded ? g_loaded.c_str() : "-", g_nskipped, g_nskipped ? ": " : "",
            g_skipped.c_str(), release.c_str(), SM64DS_MOD_API_VERSION);
    fflush(stderr);
    install_hooks();
}
