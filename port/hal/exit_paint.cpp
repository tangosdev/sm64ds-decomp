// Run coopfix1, lane EXITPAINT1: EXIT COURSE LEAVES THROUGH THE PAINTING (single player, setting ExitToPainting, off by default).
//
// WHAT TANGO ASKED FOR (11-13, Thunder's "Exit from Painting"): on the DS the pause menu's Exit Course always puts the player in the
// castle's first floor on the star rug. With this cheat on, Exit Course puts the player where a lost life in that course does: in
// front of the course's painting, jumping out of it, with no star dance, no lost-life animation and no clear screen.
//
// HOW THE CARTRIDGE DECIDES. Three ways out of a course, two places to land:
//   - a star (Player::func_ov002_020c94a4 -> ExitLevel -> SetNextLevel(1)) and a lost life (KillPlayer -> SetNextLevel(2),
//     HitDeathPlane) both go through SetNextLevel (src/SetNextLevel.c). Its destination is, in order: Over the Rainbows (sublevel
//     0x22) on anything but a star, the castle grounds entrance 0xa (the fall out of the sky); then the ROM's own table
//     data_02075638 for eight sublevels (the three Bowser roads, the three character bosses, Big Boo's Haunt); then the return
//     the painting recorded when the player went in (data_0209211c / data_0209f200, written by LoadLevel's returnState from the
//     painting's exit object, src/game/actors/d_a_ch_scene.cpp); with nothing recorded, or from the castle itself, the castle
//     grounds entrance 0xd.
//   - the pause menu's Exit Course (dScStage_c::PS_Update case 0x13, src/game/stages/d_s_stage_methods.cpp) does not call
//     SetNextLevel at all: LoadLevelNoReturn(2, 0, -1, 0), the first floor entrance 0, reason 0.
//   - what the player does on arrival is the castle entrance's type (Player::func_ov002_020c7dd0) bent by the reason: reason 2 (a
//     lost life) turns the painting jump-outs 8 / 0xb into their dazed forms 9 / 0xc and takes health away, reason 1 (a star)
//     heals, may hand over a Bowser key and opens the course clear screen. Reason 0, Exit Course's own, is the plain jump-out.
//
// WHAT THE SETTING CHANGES. Only the two numbers of the load in case 0x13: the hook works out SetNextLevel's destination for a
// lost life (without writing anything) and makes the same LoadLevelNoReturn(level, entrance, -1, 0) Exit Course makes, so the
// reason stays 0, the act, the death table, the saved sub-area chain, the wipe and the fade are Exit Course's, and the player
// jumps out of the painting. When SetNextLevel would land on the grounds entrance 0xd (no painting recorded) the hook says 0 and
// the cartridge's star rug stays.
//
// WHERE IT APPLIES. The setting is on and this console plays alone (no room of any kind, no VS match; the same test as
// hal/star_stay.cpp). A room's Exit Course is left exactly as it is: the group's level tracking and the room's own returns
// (hal/coop_tp.cpp) are built around the star rug, and nothing here is synced. The row is greyed in a room (settings_live).
//
// Dev-only proof knobs (compiled out of PORT_ROM_CLEAN, all off unless set):
//   SM64DS_EXITPAINT_LOG=1                     the decision at Exit Course, the arrival (level, entrance, reason, the player's
//                                              entry step) and every change of the player's state for 600 frames after it, with
//                                              lives, coins and the star count.
//   SM64DS_EXITPAINT_ENTER=<level>:<frame>[:<act>]   <frame> frames into any castle sublevel, enter course <level> the way its
//                                              painting does (LoadLevel with the painting's return, table hal/coop_tp.cpp), once.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "comms_hub.h"        // port::comms_hub_active
#include "host_settings.h"    // host_setting_exit_to_painting
#ifndef PORT_ROM_CLEAN
#include "coop_tp.h"          // port::coop_castle_entrance_for_course (proof knob only)
#endif

extern "C" {
extern int (*port_exit_course_hook)(void);   // src/game/stages/d_s_stage_methods.cpp
extern unsigned char data_0209f2d8[];        // stage mode, 1 = VS
extern signed char data_0209f2f8;            // current sublevel
extern signed char data_0209211c;            // the castle sublevel the painting recorded, < 0 = none
extern unsigned char data_0209f200;          // its entrance
extern unsigned char data_02075638[];        // SetNextLevel's table: sublevel, destination, entrance (8 rows of 3)
int SublevelToLevel(int i);
void LoadLevelNoReturn(unsigned level, unsigned entrance, unsigned star, unsigned unk);
#ifndef PORT_ROM_CLEAN
extern void (*port_sdat_frame_hook)(void);   // hal/sdat/consumer.cpp: one call per game frame, chained
extern void *data_0209f394[];
extern unsigned char data_0209f250;          // my player index
extern unsigned char data_02092110[];        // pending level change, < 0 = none
extern int data_0209f264[];                  // current entrance
extern unsigned char data_0209f2fc;          // entry reason
extern int data_0209f220[];                  // current act
extern signed char data_0209f2f4[];          // lives
extern unsigned char data_ov002_021104b4[];  // Player's level-entry state
unsigned char NumStars(void);
short NumCoins(void);
void LoadLevel(unsigned level, unsigned entrance, unsigned star, unsigned unk, unsigned retEntrance);
#endif
}

namespace {

bool env_set(const char *n) {
    const char *e = std::getenv(n);
    return e && *e && !(e[0] == '0' && e[1] == 0);
}

// Alone: no room of any kind and no VS match (hal/star_stay.cpp's test).
int solo() {
    if (data_0209f2d8[0] == 1) return 0;
    if (port::comms_hub_active()) return 0;
    if (env_set("SM64DS_COOP") || env_set("SM64DS_PARTY") || env_set("SM64DS_VS_MAP")) return 0;
    const char *k = std::getenv("SM64DS_ROOM_KIND");
    if (k && *k) return 0;
    return 1;
}

// SetNextLevel's destination for a lost life in the current sublevel, read only. False when it would be the grounds entrance 0xd
// (no painting recorded, or the castle itself).
bool life_lost_destination(int *level, int *entrance) {
    const int v = data_0209f2f8;
    int to = data_0209211c, at = data_0209f200;
    if (v == 0x22) {
        to = 1; at = 0xa;
    } else {
        for (int i = 0; i < 8; ++i) {
            const unsigned char *p = data_02075638 + i * 3;
            if (v == (signed char)p[0]) { to = (signed char)p[1]; at = p[2]; break; }
        }
    }
    if (to < 0 || SublevelToLevel(v) == 0x1d) return false;
    *level = to; *entrance = at;
    return true;
}

#ifndef PORT_ROM_CLEAN
bool g_log;
#endif

int exit_course_hook() {
    if (!host_setting_exit_to_painting()) return 0;
    const int alone = solo();
    int to = -1, at = -1;
    const bool have = alone && life_lost_destination(&to, &at);
#ifndef PORT_ROM_CLEAN
    if (g_log)
        std::fprintf(stderr, "[exitpaint] Exit Course in sublevel %d (recorded return %d:%d): %s level %d entrance %d\n",
                     (int)data_0209f2f8, (int)data_0209211c, (int)data_0209f200,
                     !alone ? "in a room, cartridge star rug," : have ? "PAINTING," : "no painting, cartridge star rug,",
                     have ? to : 2, have ? at : 0);
#endif
    if (!have) return 0;
    LoadLevelNoReturn((unsigned)to, (unsigned)at, (unsigned)-1, 0);
    return 1;
}

#ifndef PORT_ROM_CLEAN
struct Knobs {
    int en_lvl = -1, en_fr = 0, en_act = 1; bool en_done = false;
    int in_castle = 0;
    int last_lvl = -999, since = 0, watch = 0;
    const void *last_state = nullptr; int last_step = -1;
};
Knobs K;
void (*g_prev)(void);

char *my_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}

void frame() {
    if (g_prev) g_prev();
    const int lv = (int)data_0209f2f8;
    if (lv != K.last_lvl) {
        K.last_lvl = lv; K.since = 0; K.watch = g_log ? 600 : 0; K.last_state = nullptr; K.last_step = -1;
        if (g_log)
            std::fprintf(stderr, "[exitpaint] arrived: sublevel %d entrance %d reason %d act %d lives %d coins %d stars %d\n", lv,
                         data_0209f264[0] & 0xff, (int)data_0209f2fc, data_0209f220[0] & 0xff, (int)data_0209f2f4[0],
                         (int)NumCoins(), (int)NumStars());
    }
    ++K.since;
    if (K.watch > 0) {
        --K.watch;
        if (char *b = my_body()) {
            const void *st = *(void **)(b + 0x370);
            const int step = *(unsigned char *)(b + 0x6e3);
            if (st != K.last_state || step != K.last_step) {
                K.last_state = st; K.last_step = step;
                std::fprintf(stderr, "[exitpaint] f%d sublevel %d: state %s step %d lives %d coins %d stars %d\n", K.since, lv,
                             st == (const void *)data_ov002_021104b4 ? "LEVEL-ENTRY" : "other", step, (int)data_0209f2f4[0],
                             (int)NumCoins(), (int)NumStars());
            }
        }
    }
    if (K.en_lvl >= 0 && !K.en_done && SublevelToLevel(lv) == 0x1d && (signed char)data_02092110[0] < 0 && my_body() &&
        K.since >= K.en_fr) {
        K.en_done = true;
        int castle = -1, ret = -1;
        if (port::coop_castle_entrance_for_course(K.en_lvl, &castle, &ret)) {
            std::fprintf(stderr, "[exitpaint] ENTER knob: course %d act %d from castle sublevel %d, return %d:%d (the painting's load)\n",
                         K.en_lvl, K.en_act, lv, castle, ret);
            LoadLevel((unsigned)K.en_lvl, 0, (unsigned)K.en_act, 0, (unsigned)ret);
            data_0209211c = (signed char)castle;
        } else {
            std::fprintf(stderr, "[exitpaint] ENTER knob: course %d has no painting in the table, no load\n", K.en_lvl);
        }
    }
}
#endif

struct Reg {
    Reg() {
        port_exit_course_hook = &exit_course_hook;
#ifndef PORT_ROM_CLEAN
        g_log = env_set("SM64DS_EXITPAINT_LOG");
        if (const char *e = std::getenv("SM64DS_EXITPAINT_ENTER"))
            if (std::sscanf(e, "%d:%d:%d", &K.en_lvl, &K.en_fr, &K.en_act) < 2) K.en_lvl = -1;
        if (g_log || K.en_lvl >= 0) { g_prev = port_sdat_frame_hook; port_sdat_frame_hook = frame; }
#endif
    }
} g_reg;

}  // namespace
