// TESTSPAWN1 (run coopfix1): TEST SCENARIOS. Dev only, INERT unless SM64DS_SCENARIO is set.
//
//   SM64DS_SCENARIO=<file>[#<player>]
//
// One knob puts this console's player where a test needs him and in the state it needs: the level and act, a spot (a
// position or "near the first live actor of class X"), the facing, a power-up, a cap state, health, lives, and the save
// bits the test needs (stars, keys, opened doors, unlocked characters, cannons). port/tools/scenario/run_scenario.py
// writes the file from a scenario JSON and launches one game per player (solo, or a coop room on a local relay);
// port/tools/scenario/README.md lists every key. The file is plain text:
//
//   [all]                    keys every player takes
//   name = klepto_hats
//   [p0]                     keys for player 0 only (the #<player> of the knob; 0 when it is left out)
//   level = 16
//   near = 239:300           class:distance[:dy[:index[:angle]]]
//
// WHAT IT TOUCHES AND WHEN. Nothing at all unless the knob is set. Then:
//   1. at the top of main (port_scenario_boot_env): the existing boot knobs the file implies, only those not already
//      set in the environment: SM64DS_CHARACTER, SM64DS_SKIP_MENU, and for single player SM64DS_LEVEL, SM64DS_STAR_FILTER,
//      SM64DS_ENTRANCE and SM64DS_SAVE_PATH (a scenario's own save file, so a preset never reaches a real save). A coop room
//      boots the way the launcher's room rows always have (the castle grounds, then a warp).
//   2. on the first stage boot, beside SM64DS_STARS_SEED (port_scenario_save_seed): the save preset, through the game's
//      own setters wherever one exists (CollectStar, OpenCannon, SaveData::SetCharacterIntro, SaveData::PlayerLoseCap).
//      Keys, opened doors and the castle flags have no setter on the cartridge (daObjKey_c, daDoor_c and the Toad OR the
//      bit in place), so the same bit is ORed the same way. It only ever sets bits, like a loaded file would, and it is
//      refused when neither a single player save path nor a coop room save is named (never a real save).
//   3. once a frame after the actor tick, beside the coop world frame (port_scenario_frame): the warp to the level (the
//      game's own LoadLevelNoReturn, as SM64DS_COOP_WARP does), then, once the level has settled, the spot, the facing,
//      the power-up through the Player's own Init* (the calls a Power Flower or a feather makes), the hat through
//      Player::SetNewHatCharacter, Klepto's own steal for a cap held by Klepto, GiveHealth and GiveLives. Never from a draw
//      hook (the STAGE_ROW notes: a pose step run from the draw hook broke the renderer).
// Gameplay, physics and ROM code are untouched; every line below is behind the knob.

#include "coop_boot.h"

#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;            // my player index
extern signed char data_0209f2f8;              // LEVEL_ID
extern int data_0209f220[];                    // the current act
extern unsigned char data_0209f1f0;            // the act a level change stages
extern signed char data_02092120;              // the area shown
extern unsigned char data_0209caa0[];          // the live save block (FileSaveData)
extern unsigned char data_0209cab4[];          // its star bytes, one per course
extern unsigned char data_0209f224;            // the last collected star (CollectStar sets it)
extern signed char data_0209f2f4[];            // lives
extern short data_02092144[];                  // health, per player, 0x100 a wedge
extern void *data_0209f318;                    // the camera
void func_0200d1e4(char *camera);              // the camera's own snap behind its player (Player::St_Teleport's call)
void *_ZN8dActor_c4NextEPKS_(const void *prev);
void LoadLevelNoReturn(unsigned level, unsigned entrance, unsigned star, unsigned reason);
void ChangeArea(int areaID);
void CollectStar(int course, int star);
void OpenCannon(int course);
unsigned char NumStars(void);
void _ZN8SaveData17SetCharacterIntroEi(int character);
void _ZN8SaveData13PlayerLoseCapEv(void);
int _ZN8SaveData16HasPlayerLostCapEv(void);
void GiveHealth(int idx, int amt);
void GiveLives(int delta);
void _ZN6Player16InitWingFeathersEb(void *self, unsigned char b_);
void _ZN6Player16InitBalloonMarioEv(void *self);
void _ZN6Player14InitMetalWarioEv(void *self);
void _ZN6Player15InitVanishLuigiEv(void *self);
void _ZN6Player13InitFireYoshiEv(void *self);
void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned a, unsigned b, unsigned char c);
void _ZN9dBgCh_GndC1Ev(void *self);
void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, const void *pos, void *actor);
int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
int port_scenario_klepto_steal(void *who);     // hal/coop_cap.cpp: Klepto's own swoop and contact test
extern const void *port_level_fb_ptr;
}
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

namespace {

const char *const kCharName[4] = {"mario", "luigi", "wario", "yoshi"};

struct Scenario {
    bool read = false, on = false;
    char path[512] = {}, name[64] = "scenario";
    int player = 0;
    // boot
    int character = -1, level = -1, act = 1, entrance = 0;
    // spot
    int area = -1;
    bool has_pos = false; int pos[3] = {0, 0, 0};
    int yaw = -1;                                   // DS angle units, 0x10000 a turn
    int near_cls = -1, near_dist = 200, near_dy = 0, near_index = 0, near_angle = 0, near_param = -1;
    bool drop = true;                               // put a near spot on the floor below it
    bool drop_set = false;                          // drop named in the file: a position drops too when it is 1
    // state
    char power[16] = {}; int power_ticks = -1;
    int hat = -1;
    char cap[16] = {};                              // on | lost | klepto
    int health = -1, lives = -1;
    // save preset
    struct StarSeed { int course; unsigned bits; } stars[32]; int nstars = 0;
    int star_count = -1;
    unsigned flags1 = 0, flags2 = 0;                // bits to OR in
    unsigned chars = 0;                             // SetCharacterIntro mask (bit n = character n)
    unsigned cannons = 0;                           // OpenCannon mask (bit n = course n)
    // proof
    int settle = 45;                                // ticked frames in the target level before the spot is applied
    int shot_ms = -1;                               // a picture this long after the spot (-1 none)
    char shot[256] = {};
    int census[8]; int ncensus = 0;
    int tries = 8;                                  // a near spot is retried every 30 frames while the actor is missing
};
Scenario S;

void trim(char *s) {
    char *b = s; while (*b == ' ' || *b == '\t') ++b;
    if (b != s) std::memmove(s, b, std::strlen(b) + 1);
    size_t n = std::strlen(s);
    while (n && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '\r' || s[n - 1] == '\n')) s[--n] = 0;
}

int char_of(const char *v) {
    for (int i = 0; i < 4; ++i) if (!_stricmp(v, kCharName[i])) return i;
    return (v[0] >= '0' && v[0] <= '3' && !v[1]) ? v[0] - '0' : -1;
}

unsigned mask_of_chars(const char *v) {   // "luigi,wario" or a number
    if (v[0] >= '0' && v[0] <= '9') return (unsigned)std::strtoul(v, nullptr, 0);
    unsigned m = 0; char buf[128]; std::snprintf(buf, sizeof buf, "%s", v);
    for (char *t = std::strtok(buf, ", "); t; t = std::strtok(nullptr, ", ")) { const int c = char_of(t); if (c >= 0) m |= 1u << c; }
    return m;
}

// A named save bit: the bit the cartridge's own code sets for it (see the banner).
bool named_flag(const char *n, unsigned *f1, unsigned *f2) {
    struct { const char *name; unsigned f1, f2; } const k[] = {
        {"basement_key", 2u << 0, 0},       // daObjKey_c::func_ov089_02131df4: flags1 |= 2 << kind (kind 0, Bowser 1's key)
        {"upstairs_key", 2u << 1, 0},       // kind 1, Bowser 2's key
        {"basement_door", 0x100u << 0, 0},  // daDoor_c::func_ov100_021449c8: flags1 |= 0x100 << flagShift (key door 0)
        {"upstairs_door", 0x100u << 1, 0},  // key door 1
        {"rabbits_awake", 0x40000000u, 0},  // daKinopio_c: the Toad's rabbit line; daMip_c spawns only with it
        {"moat_drained", 0, 0x80000u},      // the basement pillars' drain bit, flags2
        // the one-time messages, as seen: the castle's "Welcome. No one's home!" (Player::func_ov002_020c44c4, level 2,
        // flags2 0x200) and the power-up hints Player::func_ov002_020c43c4 records (flags2 0x400..0x4000, 0x10000000). A
        // course's own entry text shows only while that course has no star (the same function), so a preset gives it one.
        {"messages_seen", 0, 0x200u | 0x400u | 0x800u | 0x1000u | 0x2000u | 0x4000u | 0x10000000u},
    };
    for (const auto &e : k) if (!_stricmp(n, e.name)) { *f1 |= e.f1; *f2 |= e.f2; return true; }
    return false;
}

void set_key(Scenario &s, const char *k, const char *v) {
    if (!_stricmp(k, "name")) std::snprintf(s.name, sizeof s.name, "%s", v);
    else if (!_stricmp(k, "character")) s.character = char_of(v);
    else if (!_stricmp(k, "level")) s.level = std::atoi(v);
    else if (!_stricmp(k, "act")) s.act = std::atoi(v);
    else if (!_stricmp(k, "entrance")) s.entrance = std::atoi(v);
    else if (!_stricmp(k, "area")) s.area = std::atoi(v);
    else if (!_stricmp(k, "pos")) s.has_pos = std::sscanf(v, "%i,%i,%i", &s.pos[0], &s.pos[1], &s.pos[2]) == 3;
    else if (!_stricmp(k, "yaw")) s.yaw = (int)std::strtol(v, nullptr, 0) & 0xffff;
    else if (!_stricmp(k, "yaw_deg")) s.yaw = (int)std::lround(std::atof(v) * 65536.0 / 360.0) & 0xffff;
    else if (!_stricmp(k, "near")) std::sscanf(v, "%i:%i:%i:%i:%i", &s.near_cls, &s.near_dist, &s.near_dy, &s.near_index, &s.near_angle);
    else if (!_stricmp(k, "near_param")) s.near_param = (int)std::strtol(v, nullptr, 0);
    else if (!_stricmp(k, "drop")) { s.drop = std::atoi(v) != 0; s.drop_set = true; }
    else if (!_stricmp(k, "power")) std::snprintf(s.power, sizeof s.power, "%s", v);
    else if (!_stricmp(k, "power_ticks")) s.power_ticks = std::atoi(v);
    else if (!_stricmp(k, "hat")) s.hat = char_of(v);
    else if (!_stricmp(k, "cap")) std::snprintf(s.cap, sizeof s.cap, "%s", v);
    else if (!_stricmp(k, "health")) s.health = std::atoi(v);
    else if (!_stricmp(k, "lives")) s.lives = std::atoi(v);
    else if (!_stricmp(k, "stars")) {               // course:hexbits[,course:hexbits...]
        const char *p = v;
        while (*p && s.nstars < 32) {
            char *q; const long c = std::strtol(p, &q, 10);
            if (q == p || *q != ':') break;
            p = q + 1;
            const unsigned long b = std::strtoul(p, &q, 16);
            if (c >= 0 && c < 0x1e) s.stars[s.nstars++] = {(int)c, (unsigned)b & 0xff};
            p = q; while (*p == ',' || *p == ' ') ++p;
        }
    }
    else if (!_stricmp(k, "star_count")) s.star_count = std::atoi(v);
    else if (!_stricmp(k, "flags")) {               // named save bits, comma separated
        char buf[256]; std::snprintf(buf, sizeof buf, "%s", v);
        for (char *t = std::strtok(buf, ", "); t; t = std::strtok(nullptr, ", "))
            if (!named_flag(t, &s.flags1, &s.flags2)) std::fprintf(stderr, "[scenario] unknown save flag '%s' ignored\n", t);
    }
    else if (!_stricmp(k, "flags1")) s.flags1 |= (unsigned)std::strtoul(v, nullptr, 0);
    else if (!_stricmp(k, "flags2")) s.flags2 |= (unsigned)std::strtoul(v, nullptr, 0);
    else if (!_stricmp(k, "characters")) s.chars |= mask_of_chars(v);
    else if (!_stricmp(k, "cannons")) {             // course numbers, comma separated
        char buf[128]; std::snprintf(buf, sizeof buf, "%s", v);
        for (char *t = std::strtok(buf, ", "); t; t = std::strtok(nullptr, ", ")) { const int c = std::atoi(t); if (c >= 0 && c < 32) s.cannons |= 1u << c; }
    }
    else if (!_stricmp(k, "settle")) s.settle = std::atoi(v);
    else if (!_stricmp(k, "shot_ms")) s.shot_ms = std::atoi(v);
    else if (!_stricmp(k, "shot")) std::snprintf(s.shot, sizeof s.shot, "%s", v);
    else if (!_stricmp(k, "tries")) s.tries = std::atoi(v);
    else if (!_stricmp(k, "census")) {
        char buf[128]; std::snprintf(buf, sizeof buf, "%s", v);
        for (char *t = std::strtok(buf, ", "); t && s.ncensus < 8; t = std::strtok(nullptr, ", ")) s.census[s.ncensus++] = std::atoi(t);
    }
    else std::fprintf(stderr, "[scenario] unknown key '%s' ignored\n", k);
}

// Read the file once: [all] first for every player, then [p<player>] over it (two passes, so the order in the file does
// not matter).
Scenario &get() {
    if (S.read) return S;
    S.read = true;
    const char *e = std::getenv("SM64DS_SCENARIO");
    if (!e || !*e) return S;
    std::snprintf(S.path, sizeof S.path, "%s", e);
    if (char *h = std::strrchr(S.path, '#')) { S.player = std::atoi(h + 1); *h = 0; }
    for (int pass = 0; pass < 2; ++pass) {
        FILE *f = std::fopen(S.path, "rb");
        if (!f) { std::fprintf(stderr, "[scenario] cannot open %s: the knob does nothing\n", S.path); return S; }
        char line[512]; bool mine = false;
        char want[16]; std::snprintf(want, sizeof want, "p%d", S.player);
        while (std::fgets(line, sizeof line, f)) {
            trim(line);
            if (!line[0] || line[0] == ';' || line[0] == '#') continue;
            if (line[0] == '[') {
                char *r = std::strchr(line, ']'); if (r) *r = 0;
                mine = pass == 0 ? !_stricmp(line + 1, "all") : !_stricmp(line + 1, want);
                continue;
            }
            if (!mine) continue;
            char *eq = std::strchr(line, '=');
            if (!eq) continue;
            *eq = 0; char *k = line, *v = eq + 1; trim(k); trim(v);
            set_key(S, k, v);
        }
        std::fclose(f);
    }
    S.on = true;
    std::fprintf(stderr, "[scenario] %s player %d: %s, level %d act %d, %s%s, power '%s', cap '%s', hat %d, health %d, lives %d\n",
                 S.name, S.player, S.character >= 0 ? kCharName[S.character] : "(character not set)", S.level, S.act,
                 S.has_pos ? "a position" : S.near_cls >= 0 ? "near a class" : "the level's own entrance",
                 S.area >= 0 ? " in a set area" : "", S.power, S.cap, S.hat, S.health, S.lives);
    std::fflush(stderr);
    return S;
}

bool coop_room() { const char *c = std::getenv("SM64DS_COOP"); return c && *c && *c != '0'; }

// Two spellings of one file: case and slash direction do not matter on Windows.
bool same_path(const char *a, const char *b) {
    for (;; ++a, ++b) {
        char x = *a, y = *b;
        if (x == '\\') x = '/';
        if (y == '\\') y = '/';
        if (x >= 'A' && x <= 'Z') x += 32;
        if (y >= 'A' && y <= 'Z') y += 32;
        if (x != y) return false;
        if (!x) return true;
    }
}

void put_env(const char *k, const char *v) {
    const char *was = std::getenv(k);
    if (was && *was) { std::fprintf(stderr, "[scenario] boot knob %s left as the environment set it (%s)\n", k, was); return; }
    _putenv_s(k, v);
    std::fprintf(stderr, "[scenario] boot knob %s=%s\n", k, v);
}

char *my_body() { const int me = (int)data_0209f250; return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr; }
bool is_player(const void *a) { for (int i = 0; i < 16; ++i) if (data_0209f394[i] == a) return true; return false; }
bool going(const char *a) { return *(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2; }
int rd(const char *a, int off) { int v; std::memcpy(&v, a + off, 4); return v; }
void wr(char *a, int off, int v) { std::memcpy(a + off, &v, 4); }
unsigned word(int i) { unsigned w; std::memcpy(&w, data_0209caa0 + 4 * i, 4); return w; }
void or_word(int i, unsigned bits) { const unsigned w = word(i) | bits; std::memcpy(data_0209caa0 + 4 * i, &w, 4); }

// The floor under (x, y, z), world Fix12, through the level's own ground ray (the harness's spawn probe); false if none.
bool floor_below(char *me, int x, int y, int z, int *out) {
    static char rg[0x50];
    int p[3] = {x, y, z};
    _ZN9dBgCh_GndC1Ev(rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, p, me);
    rg[4] |= 1;
    *(int *)(rg + 0x4c) = 0x2000000;   // reach: 8192 units down
    if (!_ZN9dBgCh_Gnd10DetectClsnEv(rg) || !rg[0x48]) return false;
    *out = *(int *)(rg + 0x44);
    return true;
}

void census() {
    for (int c = 0; c < S.ncensus; ++c) {
        int n = 0;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(nullptr); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (*(const unsigned short *)(a + 0x0c) != (unsigned)S.census[c] || is_player(a)) continue;
            std::fprintf(stderr, "[scenario] census class %d #%d pos %d,%d,%d yaw %04x ax %d az %d param %08x area %d slot %d%s\n", S.census[c],
                         n++, rd(a, 0x5c) >> 12, rd(a, 0x60) >> 12, rd(a, 0x64) >> 12, (unsigned)(unsigned short)*(const short *)(a + 0x8e),
                         (int)*(const short *)(a + 0x8c), (int)*(const short *)(a + 0x90),
                         (unsigned)rd(a, 0x08), (int)*(const signed char *)(a + 0xcc), (int)*(const short *)(a + 0xce), going(a) ? " (going)" : "");
        }
        std::fprintf(stderr, "[scenario] census class %d: %d live\n", S.census[c], n);
    }
}

// The n-th live actor of class cls (spawn order), or null.
char *nth_actor(int cls, int n, int param) {
    int k = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(nullptr); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != (unsigned)cls || is_player(a) || going(a)) continue;
        if (param >= 0 && rd(a, 0x08) != param && (rd(a, 0x08) & 0xff) != param) continue;
        if (k++ == n) return a;
    }
    return nullptr;
}

void show_area(char *me, int area) {
    if (area < 0) return;
    *(signed char *)(me + 0xcc) = (signed char)area;
    if (data_02092120 != area) ChangeArea(area);
}

// The spot. True once placed (or nothing to place); false to retry later (the actor is not live yet).
bool place(char *me) {
    int x, y, z, yaw = S.yaw;
    if (S.has_pos) {
        x = S.pos[0] << 12; y = S.pos[1] << 12; z = S.pos[2] << 12;
        show_area(me, S.area);
        int fy = 0;
        if (S.drop_set && S.drop && floor_below(me, x, y + (300 << 12), z, &fy)) y = fy + (8 << 12);
    } else if (S.near_cls >= 0) {
        char *a = nth_actor(S.near_cls, S.near_index, S.near_param);
        if (!a) return false;
        // in front of the actor along its own facing (plus near's angle), facing it
        const int ay = (unsigned short)*(const short *)(a + 0x8e);
        const int dir = (ay + S.near_angle) & 0xffff;
        const double r = dir * (6.283185307179586 / 65536.0);
        x = rd(a, 0x5c) + (int)std::lround(std::sin(r) * S.near_dist * 4096.0);
        z = rd(a, 0x64) + (int)std::lround(std::cos(r) * S.near_dist * 4096.0);
        y = rd(a, 0x60) + (S.near_dy << 12);
        if (yaw < 0) yaw = (dir + 0x8000) & 0xffff;
        const int aa = (int)*(const signed char *)(a + 0xcc);
        show_area(me, S.area >= 0 ? S.area : aa);
        int fy = 0;
        if (S.drop && floor_below(me, x, y + (300 << 12), z, &fy)) y = fy + (8 << 12);
        std::fprintf(stderr, "[scenario] near class %d #%d (slot %d) at %d,%d,%d facing %04x, area %d\n", S.near_cls, S.near_index,
                     (int)*(const short *)(a + 0xce), rd(a, 0x5c) >> 12, rd(a, 0x60) >> 12, rd(a, 0x64) >> 12, (unsigned)ay, aa);
    } else {
        show_area(me, S.area);
        if (yaw >= 0) *(short *)(me + 0x8e) = (short)yaw;
        return true;
    }
    wr(me, 0x5c, x); wr(me, 0x60, y); wr(me, 0x64, z);
    wr(me, 0x98, 0); wr(me, 0xa4, 0); wr(me, 0xa8, 0); wr(me, 0xac, 0);
    if (yaw >= 0) *(short *)(me + 0x8e) = (short)yaw;
    // what the game's own teleport does next (Player.cpp St_Teleport): the camera snaps behind the player and shows his area
    if (char *cam = (char *)data_0209f318) if (*(char **)(cam + 0x110) == me) func_0200d1e4(cam);
    std::fprintf(stderr, "[scenario] %s p%d placed at %d,%d,%d facing %04x in level %d area %d\n", S.name, S.player, x >> 12, y >> 12,
                 z >> 12, (unsigned)(unsigned short)*(const short *)(me + 0x8e), (int)data_0209f2f8, (int)*(const signed char *)(me + 0xcc));
    return true;
}

void power(char *me) {
    if (!S.power[0] || !_stricmp(S.power, "none")) return;
    const int ch = *(const unsigned char *)(me + 0x6d9) & 3;   // Player::mCharacter
    const char *p = S.power;
    // "flower" is the character's own Power Flower; the named ones must match the character, as on the cartridge
    if (!_stricmp(p, "flower")) p = ch == 0 ? "balloon" : ch == 1 ? "vanish" : ch == 2 ? "metal" : "fire";
    struct { const char *n; int ch; void (*f)(void *); } const k[] = {
        {"balloon", 0, _ZN6Player16InitBalloonMarioEv}, {"vanish", 1, _ZN6Player15InitVanishLuigiEv},
        {"metal", 2, _ZN6Player14InitMetalWarioEv},     {"fire", 3, _ZN6Player13InitFireYoshiEv},
    };
    bool done = false;
    if (!_stricmp(p, "wing")) {
        if (ch == 0) { _ZN6Player16InitWingFeathersEb(me, 1); done = true; }   // the feather's own call (d_a_feather.cpp)
    } else {
        for (const auto &e : k) if (!_stricmp(p, e.n) && ch == e.ch) { e.f(me); done = true; }
    }
    if (!done) { std::fprintf(stderr, "[scenario] power '%s' refused: %s cannot have it\n", S.power, kCharName[ch]); return; }
    if (S.power_ticks >= 0) *(unsigned short *)(me + 0x6ae) = (unsigned short)S.power_ticks;
    std::fprintf(stderr, "[scenario] power '%s' given to %s through the Player's own Init (timer +0x6ae %u, wings %u)\n", p, kCharName[ch],
                 (unsigned)*(const unsigned short *)(me + 0x6ae), (unsigned)*(const unsigned char *)(me + 0x6ff));
}

void apply_state(char *me) {
    power(me);
    if (S.hat >= 0) {
        _ZN6Player18SetNewHatCharacterEjjb(me, (unsigned)S.hat, 0, 0);
        std::fprintf(stderr, "[scenario] hat: Player::SetNewHatCharacter(%s); character now %u, hat %u\n", kCharName[S.hat],
                     (unsigned)*(const unsigned char *)(me + 0x6d9), (unsigned)*(const unsigned char *)(me + 0x6dd));
    }
    if (!_stricmp(S.cap, "klepto")) {
        const int r = port_scenario_klepto_steal(me);
        std::fprintf(stderr, "[scenario] cap: Klepto's own steal %s (save lost bit %d)\n", r ? "took my cap" : "did NOT take a cap",
                     _ZN8SaveData16HasPlayerLostCapEv() ? 1 : 0);
    }
    const int p = (int)data_0209f250 & 15;
    if (S.health >= 0) {
        const int want = (S.health > 8 ? 8 : S.health) << 8;
        GiveHealth(p, want - data_02092144[p]);
        std::fprintf(stderr, "[scenario] health %d wedges (GiveHealth), now %d\n", S.health, data_02092144[p] >> 8);
    }
    if (S.lives >= 0) {
        GiveLives(S.lives - data_0209f2f4[0]);
        std::fprintf(stderr, "[scenario] lives %d (GiveLives), now %d\n", S.lives, (int)data_0209f2f4[0]);
    }
}

void shot() {
    char nm[300];
    if (S.shot[0]) std::snprintf(nm, sizeof nm, "%s", S.shot);
    else std::snprintf(nm, sizeof nm, "scenario_%s_p%d.bmp", S.name, S.player);
    const void *lfb = port_level_fb_ptr;
    const bool w = lfb && ntr::ppu_write_bmp(nm, *(const ntr::Framebuffer *)lfb);
    char *me = my_body();
    std::fprintf(stderr, "[scenario] picture %s %s (me at %d,%d,%d level %d area %d)\n", nm, w ? "written" : "FAILED",
                 me ? rd(me, 0x5c) >> 12 : 0, me ? rd(me, 0x60) >> 12 : 0, me ? rd(me, 0x64) >> 12 : 0, (int)data_0209f2f8,
                 me ? (int)*(const signed char *)(me + 0xcc) : -1);
}

enum Phase { kWait, kPlace, kDone };
Phase g_phase = kWait;
bool g_warped = false, g_skip_starsel = false;
unsigned g_in_level = 0, g_tries = 0, g_next_try = 0;
ULONGLONG g_shot_at = 0;
ULONGLONG g_skip_until = 0;   // the star select skip lapses at this tick count if the warp has not reached it

void disarm_starsel(const char *why) {
    if (!g_skip_starsel) return;
    g_skip_starsel = false;
    std::fprintf(stderr, "[scenario] star select skip disarmed unused (%s); later star selects show as always\n", why);
    std::fflush(stderr);
}

}  // namespace

// The level change's star select interlude asks once per entry (hal/level_change.cpp): skip it for the scenario's own warp
// only, so the act is the one the scenario named and no pad press is needed. Every later entry shows it as always.
extern "C" int port_scenario_take_starsel_skip(void) {
    if (!g_skip_starsel) return 0;
    if (GetTickCount64() > g_skip_until) { disarm_starsel("the warp did not reach the star select within 20 s"); return 0; }
    g_skip_starsel = false;
    std::fprintf(stderr, "[scenario] star select skipped for the scenario's warp (act %d)\n", S.act);
    return 1;
}

extern "C" void port_scenario_boot_env(void) {
    Scenario &s = get();
    if (!s.on) return;
    char v[64];
    if (s.character >= 0) { std::snprintf(v, sizeof v, "%d", s.character); put_env("SM64DS_CHARACTER", v); }
    put_env("SM64DS_SKIP_MENU", "1");
    if (!coop_room()) {
        // single player boots straight into the level (a coop room boots the castle grounds and warps, as its rows do)
        if (s.level >= 0) {
            std::snprintf(v, sizeof v, "%d", s.level); put_env("SM64DS_LEVEL", v);
            std::snprintf(v, sizeof v, "%d", s.act); put_env("SM64DS_STAR_FILTER", v);
            if (s.entrance) { std::snprintf(v, sizeof v, "%d", s.entrance); put_env("SM64DS_ENTRANCE", v); }
        }
        char sp[160]; std::snprintf(sp, sizeof sp, "scenario-saves/%s-p%d.sav", s.name, s.player);
        const char *had = std::getenv("SM64DS_SAVE_PATH");
        const bool mine = !(had && *had);
        put_env("SM64DS_SAVE_PATH", sp);
        // the save path this knob picked itself is the scenario's own; one already in the environment is not, unless the
        // launcher vouched for it (SM64DS_SCENARIO_SAVE_OK, checked in port_scenario_save_seed)
        if (mine) _putenv_s("SM64DS_SCENARIO_SAVE_OK", sp);
    }
    std::fflush(stderr);
}

extern "C" void port_scenario_save_seed(void) {
    static bool done = false;
    if (done) return;
    done = true;
    Scenario &s = get();
    if (!s.on) return;
    const bool any = s.nstars || s.star_count >= 0 || s.flags1 || s.flags2 || s.chars || s.cannons || !_stricmp(s.cap, "lost");
    if (!any) return;
    const char *sp = std::getenv(coop_room() ? "SM64DS_COOP_SAVE" : "SM64DS_SAVE_PATH");
    if (!sp || !*sp) {
        std::fprintf(stderr, "[scenario] save preset REFUSED: no %s names this scenario's own save\n",
                     coop_room() ? "SM64DS_COOP_SAVE" : "SM64DS_SAVE_PATH");
        return;
    }
    // Only a save the scenario owns may be seeded: SM64DS_SCENARIO_SAVE_OK must name the very file the game is using (the
    // launcher sets it to the file it made in the player's own run folder). A real save named by hand is never written.
    const char *ok = std::getenv("SM64DS_SCENARIO_SAVE_OK");
    if (!ok || !*ok || !same_path(ok, sp)) {
        std::fprintf(stderr, "[scenario] save preset REFUSED: the save in use (%s) is not this scenario's own "
                     "(SM64DS_SCENARIO_SAVE_OK %s); nothing written\n", sp, ok && *ok ? ok : "unset");
        std::fflush(stderr);
        return;
    }
    const unsigned char last = data_0209f224;   // CollectStar also records "the last star got"; a preset is not a grab
    for (int i = 0; i < s.nstars; ++i)
        for (int b = 0; b < 8; ++b) if (s.stars[i].bits & (1u << b)) CollectStar(s.stars[i].course, b);
    if (s.star_count > 0) {   // fill the main courses in order (course 0 is Bob-omb Battlefield), seven stars each
        for (int c = 0; c < 15 && NumStars() < s.star_count; ++c)
            for (int b = 0; b < 7 && NumStars() < s.star_count; ++b) if (!(data_0209cab4[c] & (1u << b))) CollectStar(c, b);
    }
    data_0209f224 = last;
    for (int c = 0; c < 32; ++c) if (s.cannons & (1u << c)) OpenCannon(c);
    for (int c = 0; c < 4; ++c) if (s.chars & (1u << c)) _ZN8SaveData17SetCharacterIntroEi(c);
    if (s.flags1) or_word(1, s.flags1);
    if (s.flags2) or_word(2, s.flags2);
    if (!_stricmp(s.cap, "lost")) _ZN8SaveData13PlayerLoseCapEv();
    std::fprintf(stderr, "[scenario] save preset applied: stars %u, flags1 %08x, flags2 %08x, cannons %08x, cap lost %d (save %s)\n",
                 (unsigned)NumStars(), word(1), word(2), word(4), _ZN8SaveData16HasPlayerLostCapEv() ? 1 : 0, sp);
    std::fflush(stderr);
}

extern "C" void port_scenario_frame(int ticked) {
    Scenario &s = get();
    if (!s.on || g_phase == kDone) return;
    char *me = my_body();
    if (!me) { g_in_level = 0; return; }
    if (!ticked || !port::coop_boot_ticking()) return;
    if (g_phase == kPlace && g_shot_at && GetTickCount64() >= g_shot_at) { g_shot_at = 0; shot(); g_phase = kDone; return; }
    if (g_phase != kWait) return;
    ++g_in_level;
    if (s.level >= 0 && (int)data_0209f2f8 != s.level) {
        if (!g_warped && g_in_level >= 30) {
            g_warped = true;
            g_in_level = 0;
            std::fprintf(stderr, "[scenario] warp to level %d act %d entrance %d (LoadLevelNoReturn, as SM64DS_COOP_WARP)\n", s.level, s.act,
                         s.entrance);
            /* HITCH1 proof knob: SM64DS_SCENARIO_KEEP_STARSEL=1 keeps the warp's own star select on screen (the select itself is the test) */
            g_skip_starsel = !(std::getenv("SM64DS_SCENARIO_KEEP_STARSEL") && *std::getenv("SM64DS_SCENARIO_KEEP_STARSEL") == '1');
            g_skip_until = GetTickCount64() + 20000;
            LoadLevelNoReturn((unsigned)s.level, (unsigned)s.entrance, (unsigned)s.act, 0);
            if (s.act > 0 && s.act < 8) data_0209f1f0 = (unsigned char)s.act;
        }
        return;
    }
    if (g_in_level < (unsigned)s.settle || g_in_level < g_next_try) return;
    // settled in the target level: the warp's own star select, if it had one, is long past
    disarm_starsel("the player is already settled in the target level");
    if (!place(me)) {
        if (++g_tries >= (unsigned)s.tries) {
            std::fprintf(stderr, "[scenario] near class %d #%d: no live actor after %u tries; staying at the entrance\n", s.near_cls,
                         s.near_index, g_tries);
        } else {
            if (g_tries == 1) std::fprintf(stderr, "[scenario] near class %d #%d: not live yet, retrying every 30 frames\n", s.near_cls, s.near_index);
            g_next_try = g_in_level + 30;
            return;
        }
    }
    apply_state(me);
    census();
    std::fprintf(stderr, "[scenario] %s p%d ready: %s in level %d act %d, stars %u, flags1 %08x\n", s.name, s.player,
                 kCharName[*(const unsigned char *)(me + 0x6d9) & 3], (int)data_0209f2f8, data_0209f220[0] & 0xff, (unsigned)NumStars(), word(1));
    std::fflush(stderr);
    g_phase = kPlace;
    if (s.shot_ms >= 0) g_shot_at = GetTickCount64() + (ULONGLONG)s.shot_ms; else g_phase = kDone;
}
