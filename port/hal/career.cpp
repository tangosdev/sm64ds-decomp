/* hal/career.cpp: see career.h. Never a crash and never a stop: a missing, unreadable or hand-edited career file is "nothing recorded". */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "career.h"
#include "char_save.h"
#include "mods.h"
#include "ntr/backup.h"

extern "C" {
extern unsigned char data_0209f2c4[];   /* 0 = playing, else the pause is up */
extern unsigned char data_0209f20c[];   /* level-clear owns the game */
extern unsigned char data_0209f2d8[];   /* game mode: 1 = VS */
int port_scene_live_id(void);           /* hal/scene_boot.cpp: 3 the Stage, -1 a level with no scene, 1 the title */
int port_title_page_state(int *top, int *elem, int *req);   /* hal/title_entry.cpp: the title's page (copy = 7 / 5) */
/* seams, null by default, in files that sit on more targets than this one */
extern void (*port_career_ending_hook)(void);                                                          /* hal/player_bridges.cpp */
extern void (*port_backup_write_hook)(unsigned addr, const void *src, unsigned len, const unsigned char *image);   /* ntr/backup.cpp */
}

namespace {

const unsigned long long kSaveEvery = 60ull * 60;     /* a minute of counted play */
const unsigned kRec = 0x80, kRecData = 10, kRecLen = 0x44;   /* SaveData::SaveDataToCart: file n's record at n << 7, payload at +10, 0x44 bytes */

struct File { unsigned long long play, clear; int has_clear, any; };

File g_f[3];
std::string g_path;          /* the side file the numbers above belong to ("" = none loaded) */
int g_dirty;
unsigned long long g_since_save;
int g_was_paused;
int g_was_stage;
int g_atexit;

/* the side file for the save medium in use now; "" for a guest world */
std::string career_path(void)
{
    const char *sav = ntr::backup::path();
    std::string s = sav ? sav : "";
    if (s.empty()) return "";
    const size_t n = s.size();
    if (n >= 8 && !_stricmp(s.c_str() + n - 8, ".coopsav")) return "";
    const char *over = getenv("SM64DS_SAVE_PATH");
    if (over && *over) return s + ".career.json";
    const size_t cut = s.find_last_of("/\\");
    const std::string dir = cut == std::string::npos ? std::string(".") : s.substr(0, cut);
    const std::string base = cut == std::string::npos ? s : s.substr(cut + 1);
    if (!_stricmp(base.c_str(), "online.sav")) return dir + "/online.career.json";
    return dir + "/career.json";
}

unsigned long long num_after(const char *p, const char *key, int *found)
{
    *found = 0;
    const char *k = strstr(p, key);
    if (!k || !(k = strchr(k, ':'))) return 0;
    ++k;
    while (*k == ' ') ++k;
    if (*k < '0' || *k > '9') return 0;
    *found = 1;
    return _strtoui64(k, 0, 10);
}

void save_now(void);

void load_for_now(void)
{
    const std::string p = career_path();
    if (p == g_path) return;
    if (!g_path.empty() && g_dirty) save_now();          /* the medium changed (online <-> solo): the old numbers reach their own file first */
    g_path = p;
    memset(g_f, 0, sizeof g_f);
    g_dirty = 0;
    if (p.empty()) return;
    FILE *f = fopen(p.c_str(), "rb");
    if (!f) return;
    char buf[2048];
    const size_t n = fread(buf, 1, sizeof buf - 1, f);
    fclose(f);
    buf[n] = 0;
    const char *q = strstr(buf, "\"files\"");
    for (int i = 0; q && i < 3; ++i) {
        const char *a = strchr(q, '{');
        if (!a) break;
        const char *b = strchr(a, '}');
        if (!b) break;
        const std::string sub(a, (size_t)(b + 1 - a));
        int got = 0, gotc = 0;
        g_f[i].play = num_after(sub.c_str(), "\"playTicks\"", &got);
        g_f[i].clear = num_after(sub.c_str(), "\"clearTicks\"", &gotc);
        g_f[i].has_clear = gotc;
        g_f[i].any = g_f[i].play > 0 || gotc;
        q = b + 1;
    }
    fprintf(stderr, "[career] read %s: A %llu B %llu C %llu ticks\n", p.c_str(), g_f[0].play, g_f[1].play, g_f[2].play);
}

void save_now(void)
{
    g_since_save = 0;
    if (g_path.empty()) return;
    const std::string tmp = g_path + ".tmp";
    FILE *f = fopen(tmp.c_str(), "wb");
    if (!f) { fprintf(stderr, "[career] cannot write %s\n", tmp.c_str()); return; }
    fprintf(f, "{\n  \"version\": 1,\n  \"files\": [\n");
    for (int i = 0; i < 3; ++i) {
        if (g_f[i].has_clear)
            fprintf(f, "    { \"playTicks\": %llu, \"clearTicks\": %llu }%s\n", g_f[i].play, g_f[i].clear, i < 2 ? "," : "");
        else
            fprintf(f, "    { \"playTicks\": %llu }%s\n", g_f[i].play, i < 2 ? "," : "");
    }
    fprintf(f, "  ]\n}\n");
    const int ok = fclose(f) == 0;
    if (!ok || !MoveFileExA(tmp.c_str(), g_path.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        fprintf(stderr, "[career] could not replace %s\n", g_path.c_str());
        remove(tmp.c_str());
        return;
    }
    g_dirty = 0;
    fprintf(stderr, "[career] wrote %s: A %llu B %llu C %llu ticks\n", g_path.c_str(), g_f[0].play, g_f[1].play, g_f[2].play);
}

void at_exit(void)
{
    if (g_dirty) save_now();
}

void mark_dirty(void)
{
    g_dirty = 1;
    if (!g_atexit) { g_atexit = 1; atexit(at_exit); }
}

int slot_now(void)
{
    if (data_0209f2d8[0] != 0) return -1;                                  /* VS */
    if (!port_char_save_file_known() && !port_mods_multiplayer()) return -1;   /* a scripted level boot: no file */
    if (career_path().empty()) return -1;                                  /* a guest in someone else's world */
    const int s = port_char_save_slot();
    return s >= 0 && s <= 2 ? s : -1;
}

void on_ending(void)
{
    const int s = slot_now();
    if (s < 0) { fprintf(stderr, "[career] the ending began with no file known: nothing recorded\n"); return; }
    load_for_now();
    File &f = g_f[s];
    if (f.has_clear) {
        fprintf(stderr, "[career] file %c: the ending again, clear time stays %llu ticks\n", 'A' + s, f.clear);
        return;
    }
    f.has_clear = 1;
    f.any = 1;
    f.clear = f.play;
    mark_dirty();
    char t[32];
    port_career_format(f.clear, t, sizeof t);
    fprintf(stderr, "[career] file %c: final Bowser beaten, clear time %s (%llu ticks)\n", 'A' + s, t, f.clear);
    save_now();
}

/* a write of the save chip, before it lands (image = the chip as it is now) */
void on_backup_write(unsigned addr, const void *src, unsigned len, const unsigned char *image)
{
    if (!src || !image || len < kRecLen) return;
    int d = -1;
    for (int i = 0; i < 3; ++i)
        if (addr == i * kRec + kRecData) d = i;
    if (d < 0) return;
    const unsigned char *rec = (const unsigned char *)src;
    const unsigned flags1 = (unsigned)rec[4] | (unsigned)rec[5] << 8 | (unsigned)rec[6] << 16 | (unsigned)rec[7] << 24;
    load_for_now();
    if (g_path.empty()) return;
    if (!(flags1 & 1)) {
        /* SaveData::EraseSaveFile: SetDefaultValues leaves flags1 bit 0 clear, SaveFile never does */
        if (g_f[d].any) {
            fprintf(stderr, "[career] file %c erased: its playtime and clear time are cleared\n", 'A' + d);
            memset(&g_f[d], 0, sizeof g_f[d]);
            mark_dirty();
            save_now();
        }
        return;
    }
    if (port_scene_live_id() != 1) return;          /* a save from the game itself */
    int top = -1, elem = -1, req = -1;
    port_title_page_state(&top, &elem, &req);
    for (int s = 0; s < 3; ++s) {
        if (s == d) continue;
        const unsigned char *o = image + s * kRec + kRecData;
        /* the title's buffers come from ReadFileData, which masks the word at +0xc with the live block's (SaveData::ReadFileData): that word
           may differ from the chip's, every other byte of a copy is the source file's */
        if (memcmp(o, rec, 0xc) != 0 || memcmp(o + 0x10, rec + 0x10, kRecLen - 0x10) != 0) continue;
        if (top != 7 && top != 8) {      /* the copy page, and the file options page it is back on when the copy is written */
            fprintf(stderr, "[career] file %c written at the title with file %c's bytes, but the page is %d/%d (not copy): not carried\n",
                    'A' + d, 'A' + s, top, elem);
            return;
        }
        g_f[d] = g_f[s];
        mark_dirty();
        char t[32];
        port_career_format(g_f[d].play, t, sizeof t);
        fprintf(stderr, "[career] file %c copied to file %c: playtime %s carried%s\n", 'A' + s, 'A' + d, t, g_f[d].has_clear ? " with its clear time" : "");
        save_now();
        return;
    }
}

int seat(void)
{
    port_career_ending_hook = on_ending;
    port_backup_write_hook = on_backup_write;
    return 1;
}
const int g_seated = seat();

}  // namespace

extern "C" {

void port_career_frame(int ticked)
{
    const int scene = port_scene_live_id();
    const int stage = scene == 3 || scene == -1;
    const int paused = data_0209f2c4[0] != 0;
    /* the numbers reach the file when the pause opens and when the Stage ends (a course left, the title), and once a minute of play */
    if ((paused && !g_was_paused) || (!stage && g_was_stage)) { if (g_dirty) save_now(); }
    g_was_paused = paused;
    g_was_stage = stage;
    if (!ticked || !stage || paused || data_0209f20c[0] != 0) return;
    const int s = slot_now();
    if (s < 0) return;
    load_for_now();
    if (g_path.empty()) return;
    ++g_f[s].play;
    g_f[s].any = 1;
    mark_dirty();
    if (++g_since_save >= kSaveEvery) save_now();
}

int port_career_get(int slot, unsigned long long *play, int *has_clear, unsigned long long *clear)
{
    if (slot < 0 || slot > 2) return 0;
    load_for_now();
    const File &f = g_f[slot];
    if (play) *play = f.play;
    if (has_clear) *has_clear = f.has_clear;
    if (clear) *clear = f.clear;
    return f.any && !g_path.empty();
}

int port_career_slot_now(void) { return slot_now(); }

void port_career_format(unsigned long long ticks, char *out, int cap)
{
    const unsigned long long sec = ticks / 60;
    snprintf(out, (size_t)cap, "%llu:%02u:%02u", sec / 3600, (unsigned)(sec / 60 % 60), (unsigned)(sec % 60));
}

}
