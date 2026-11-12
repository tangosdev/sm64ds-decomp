// COOP MINIMAP: see coop_minimap.h. Run mp1, lane SMALLS9.
#include "coop_minimap.h"
#include "../unmatched/Minimap_wide.h"
#include <cstdio>
#include <cstdlib>

extern "C" int port_adventure_ghost_seat(int slot);
extern "C" int port_adventure_ghost_drawpos(int slot, int out[3]);
extern "C" int port_adventure_ghost_mappos(int slot, int out[3], int *character);
extern "C" unsigned char data_0209f2d8;
extern "C" signed char data_02092120;
extern "C" int port_rom_frame(void);
extern "C" int port_coop_session_busy(void);   /* hal/coop_boot.cpp */

namespace {

int mode_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_COOP_MINIMAP");
        v = (e && e[0] == '0' && e[1] == 0) ? 0 : 1;
    }
    return v;
}

int mapdots_off() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_COOP_MAPDOTS_OFF") ? 1 : 0;
    return v;
}

int log_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_COOP_MINIMAP_LOG");
        v = (e && *e && !(e[0] == '0' && e[1] == 0)) ? 1 : 0;
    }
    return v;
}

int s_char[32];   // MAPDOTS1: the character of a friend answered 3 (other area), else -1

}  // namespace

extern "C" int port_coop_minimap_char(int slot) { return (slot >= 0 && slot < 32) ? s_char[slot] : -1; }

static int minimap_ghost_impl(int slot, int pos[3]) {
    if (!mode_env() || data_0209f2d8 == 1) return 0;     // VS draws its own
    {   // proof knob SM64DS_COOP_MINIMAP_BLINK=<from>:<to>: the hook answers "not a ghost" for ROM frames
        // from..to-1, so one run holds both pictures (the icon on, then off) for the same scene
        static int b0 = -2, b1 = -2;
        if (b0 == -2) {
            b0 = b1 = -1;
            const char *e = std::getenv("SM64DS_COOP_MINIMAP_BLINK");
            int x = 0, y = 0;
            if (e && std::sscanf(e, "%d:%d", &x, &y) == 2) { b0 = x; b1 = y; }
        }
        const int f = port_rom_frame();
        if (b0 >= 0 && f >= b0 && f < b1) return 0;
    }
    const bool seated = port_adventure_ghost_seat(slot) >= 0;   // MAPDOTS1: a friend in another area has no seat on a body here
    s_char[slot & 31] = -1;
    if (!seated || !port_adventure_ghost_drawpos(slot, pos)) {
        // MAPDOTS1: not drawn here. A friend in another area of THIS level is still on the
        // map, from his newest synced frame (answer 3); another level shows nothing (2).
        int ch = 0;
        if (!mapdots_off() && port_adventure_ghost_mappos(slot, pos, &ch) == 2) {
            s_char[slot & 31] = ch;
            if (log_env()) {
                static unsigned n3[kMinimapWidePlayers];
                if (slot >= 0 && slot < kMinimapWidePlayers && (n3[slot]++ % 120u) == 0)
                    std::fprintf(stderr, "[comap] slot %d OTHER AREA at (%d,%d,%d) char %d\n", slot,
                                 pos[0] >> 12, pos[1] >> 12, pos[2] >> 12, ch);
            }
            return 3;
        }
        return seated ? 2 : 0;
    }
    if (log_env()) {
        static unsigned n[kMinimapWidePlayers];
        if (slot >= 0 && slot < kMinimapWidePlayers && (n[slot]++ % 120u) == 0)
            std::fprintf(stderr, "[comap] slot %d seat %d at (%d,%d,%d)\n", slot,
                         port_adventure_ghost_seat(slot), pos[0] >> 12, pos[1] >> 12,
                         pos[2] >> 12);
    }
    return 1;
}

extern "C" int port_coop_minimap_ghost(int slot, int pos[3]) {
    const int r = minimap_ghost_impl(slot, pos);
    if (log_env() && slot >= 0 && slot < kMinimapWidePlayers) {   // MAPDOTS1: one line per ~2 s per slot, whatever the answer
        static unsigned nw[kMinimapWidePlayers];
        if ((nw[slot]++ % 240u) == 0) {
            int mp[3] = {0, 0, 0}, ch = 0;
            const int m = port_adventure_ghost_mappos(slot, mp, &ch);
            std::fprintf(stderr, "[comap] ask slot %d answer %d mappos %d my area %d seat %d\n", slot, r, m, (int)data_02092120, port_adventure_ghost_seat(slot));
        }
    }
    return r;
}

extern "C" void port_coop_minimap_trace(int slot, int gk, int x, int y, int mapid, int cur) {
    if (!log_env()) return;
    static unsigned n;
    if ((n++ % 240u) == 0)
        std::fprintf(stderr, "[comap] render slot %d answer %d icon (%d,%d) map %d current %d\n", slot, gk, x, y, mapid, cur);
}

namespace {
struct Install { Install() { g_mm_ghost_pos = port_coop_minimap_ghost; g_mm_coop_room = port_coop_session_busy; } } g_install;
}  // namespace
