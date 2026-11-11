// COOP CAMERA SHIELD (run coopfix1, lane CANNON2): a friend's star never takes my camera.
//
// Tango's field report: when a friend gets a star (a castle star while the others are far away in the castle, or any star while
// I sit in a cannon), my camera breaks. Cause, from the code:
//   * A friend's star also stands on my console: announced (hal/coop_spawn.cpp apply_spawn builds or adopts my copy) or made by
//     the ROM at his eighth red coin (red_check_star). Its pop-out runs the cartridge's camera scene on my console
//     (daStar_c::func_ov002_020e763c): the view is saved and turned to the star, camera flag 8 stops my camera mode (cannon aim,
//     first person, follow) and the freeze word bit CUTSCENE_FLAG stops every actor that lacks it, my player included.
//   * He takes his copy, his GONE reaches me and kill_quiet removes mine, often in the middle of that scene. Only the scene's own
//     last steps (0x1f4 restore, 0x1f5 clear) give the camera back, so the view stayed on the star's spot, flag 8 stayed up and
//     the freeze stayed up, for the rest of the visit.
// Fix: a star that is a friend's never films here (src/game/actors/d_a_star.cpp, the _MSC_VER blocks around
// port_star_cam_foreign_hook), a friend's body that collects a copy here sets no freeze, and a copy removed from outside ends its
// scene first (port_star_end_cutscene). Only the console whose player made or took the star plays the star camera.
//
// INERT outside a coop room. Off switch (must-fail control): SM64DS_COOP_CAMSHIELD=0.
// Proof knobs, all off by default:
//   SM64DS_COOP_CAMSHIELD_TRACE=<every>   [camshield] line every N frames: camera mode, flag 8, eye / look-at, freeze words,
//                                         my player's place and whether it is in a cannon (and on every flag 8 / freeze edge)
//   SM64DS_COOP_CAMSHIELD_SHOT=f1,f2,..   camshield_f<N>.bmp (the top screen) at those coop frames
//   SM64DS_COOP_CANNON_TEST=<frame>       put my player in a cannon at that frame the ROM's way (func_ov002_020df34c: the cannon
//                                         state and the climb-in camera, then func_ov002_020df300 20 frames later: the aim camera)
//   SM64DS_COOP_CAMSHIELD_GOTO=<frame>:x,y,z   put my player at that point once (world units), to stand far from a friend
#include "coop_world.h"
#include "coop_wire.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern int (*port_star_cam_foreign_hook)(const void *star, const void *player);   // src/game/actors/d_a_star.cpp
int port_star_end_cutscene(void *star);                                           // src/game/actors/d_a_star.cpp
extern void *data_0209f394[];          // the player bodies
extern unsigned char data_0209f250;    // my player's slot
extern int data_0209b454[];            // the freeze-request word (the star's bit is 0x4000000)
extern unsigned int data_0209b464;     // this frame's copy of it
extern char *data_0209f318;            // the camera
extern unsigned data_0209b098[4], data_0209b0b8[4], data_0209b0c8[4];   // camera modes 9 (first person), 11 (climb into the cannon), 12 (cannon aim)
int _ZN5Event6GetBitEj(unsigned int bit);   // event bits 0x1e (no star takes a touch) and 0x1d (HUD hidden), for the trace
int func_ov002_020df34c(void *player); // Player.cpp: into the cannon state + the climb-in camera
int func_ov002_020df300(void *player); // Player.cpp: the cannon aim camera
}
extern "C" const void *port_level_fb_ptr;
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

namespace {

enum : unsigned { kCut = 0x4000000u };
enum : int { kCamSeq = 0x496, kStFlags = 0xb0, kStState = 0x440, kCamFlags = 0x154, kCamMode = 0x138, kCamEye = 0x8c, kCamAt = 0x80,
             kPlState = 0x370, kPlPos = 0x5c };

struct Foreign { const void *a; unsigned uid; bool filming; bool on; };
Foreign g_f[32];
bool g_env = false, g_off = false, g_log = false;
unsigned g_trace = 0;
int g_cannon_frame = -1;
unsigned g_goto_frame = 0; int g_goto[3] = {0, 0, 0}; bool g_goto_on = false;
char g_shots[128] = {0};
unsigned g_quiet = 0, g_ended = 0, g_marked = 0;

inline unsigned uid_of(const void *a) { return *(const unsigned *)((const char *)a + 4); }
char *my_player() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *o = std::getenv("SM64DS_COOP_CAMSHIELD");
    g_off = o && *o == '0';
    const char *lg = std::getenv("SM64DS_COOP_LOG");
    g_log = lg && *lg && *lg != '0';     // "0" (or empty) means off
    if (const char *t = std::getenv("SM64DS_COOP_CAMSHIELD_TRACE")) g_trace = (unsigned)std::atoi(t);
    if (const char *c = std::getenv("SM64DS_COOP_CANNON_TEST")) g_cannon_frame = std::atoi(c);
    if (const char *g = std::getenv("SM64DS_COOP_CAMSHIELD_GOTO"))
        g_goto_on = std::sscanf(g, "%u:%i,%i,%i", &g_goto_frame, &g_goto[0], &g_goto[1], &g_goto[2]) == 4;
    if (const char *s = std::getenv("SM64DS_COOP_CAMSHIELD_SHOT")) { std::strncpy(g_shots, s, sizeof g_shots - 1); }
    std::fprintf(stderr, "[camshield] %s (SM64DS_COOP_CAMSHIELD=0 turns it off)\n", g_off ? "OFF: a friend's star films here (the cartridge's way)" : "on");
}

Foreign *find(const void *a) {
    for (Foreign &f : g_f) if (f.on && f.a == a) return &f;
    return nullptr;
}

// d_a_star.cpp asks: 0 mine, 1 a friend's, 2 a friend's that was already filming when it became his; or, for a collector, nonzero
// when it is not my player.
int hook(const void *star, const void *player) {
    if (g_off || !port::coop_world_active()) return 0;
    if (player) {
        const char *me = my_player();
        if (!me || player == me) return 0;
        for (int i = 0; i < 16; ++i)
            if (data_0209f394[i] == player) {
                static unsigned n = 0;
                if (g_log && n++ < 20) std::fprintf(stderr, "[camshield] a friend's body (slot %d) touched or held a star copy here: his console collects it, not mine\n", i);
                return 1;
            }
        return 0;
    }
    Foreign *f = star ? find(star) : nullptr;
    if (!f) return 0;
    if (f->uid != uid_of(star)) { f->on = false; return 0; }      // the slot holds another actor now
    const unsigned short seq = *(const unsigned short *)((const char *)star + kCamSeq);
    if (f->filming && seq == 0xffff) f->filming = false;
    if (!f->filming) {
        ++g_quiet;
        if (g_log && seq != 0xffff)
            std::fprintf(stderr, "[camshield] a friend's star uid=%u asked for my camera (step %x): not filmed here, b454=%08x cam8=%d\n",
                         f->uid, (unsigned)seq, (unsigned)data_0209b454[0],
                         data_0209f318 ? (int)((*(const unsigned *)(data_0209f318 + kCamFlags) >> 3) & 1u) : -1);
        return 1;
    }
    return 2;
}

struct Install { Install() { port_star_cam_foreign_hook = &hook; } } g_install;

const char *mode_name(const void *m) {
    if (m == data_0209b0c8) return "cannon-aim";
    if (m == data_0209b0b8) return "cannon-in";
    if (m == data_0209b098) return "first-person";
    return "other";
}

}  // namespace

// hal/coop_spawn.cpp: a star copy here that is a friend's (built or adopted for his announce, or made by his red coins).
extern "C" void coop_camshield_mark(void *a, const char *why) {
    read_env();
    if (!a) return;
    Foreign *f = find(a);
    if (!f) for (Foreign &s : g_f) if (!s.on) { f = &s; break; }
    if (!f) { f = &g_f[g_marked % 32u]; }
    const unsigned short seq = *(const unsigned short *)((const char *)a + kCamSeq);
    const bool filming = seq != 0xffff && seq != 0 && seq != 0x64 && (*(const unsigned *)((const char *)a + kStFlags) & kCut) &&
                         (data_0209b454[0] & kCut);
    *f = {a, uid_of(a), filming, true};
    ++g_marked;
    if (g_log)
        std::fprintf(stderr, "[camshield] friend's star uid=%u (%s)%s: its camera scene stays on his console\n", f->uid, why,
                     filming ? ", already filming here: the view goes back now" : "");
}

// hal/coop_spawn.cpp kill_quiet: a star copy removed from outside ends its camera scene / collect freeze first.
extern "C" void coop_camshield_on_kill(void *a) {
    read_env();
    if (g_off || !a) return;
    const unsigned short seq = *(const unsigned short *)((const char *)a + kCamSeq);
    const unsigned b454 = (unsigned)data_0209b454[0];
    const int st = *(const int *)((const char *)a + kStState);
    if (port_star_end_cutscene(a)) {
        ++g_ended;
        if (g_log)
            std::fprintf(stderr, "[camshield] removed star uid=%u was mid-scene (step %x, state %d, b454 %08x -> %08x): its camera and freeze ended first\n",
                         uid_of(a), (unsigned)seq, st, b454, (unsigned)data_0209b454[0]);
    }
    if (Foreign *f = find(a)) f->on = false;
}

// hal/coop_spawn.cpp on_frame, once per established coop frame (its frame count).
extern "C" void coop_camshield_frame(unsigned frame) {
    read_env();
    char *pl = my_player();
    if (!pl) return;
    if (g_goto_on && frame == g_goto_frame) {
        for (int i = 0; i < 3; ++i) ((int *)(pl + kPlPos))[i] = g_goto[i] << 12;
        std::fprintf(stderr, "[camshield-knob] goto %d,%d,%d frame=%u\n", g_goto[0], g_goto[1], g_goto[2], frame);
    }
    if (g_cannon_frame >= 0) {
        if ((int)frame == g_cannon_frame)
            std::fprintf(stderr, "[camshield-knob] cannon: into the cannon state -> %d frame=%u\n", func_ov002_020df34c(pl), frame);
        if ((int)frame == g_cannon_frame + 20)
            std::fprintf(stderr, "[camshield-knob] cannon: aim camera -> %d frame=%u\n", func_ov002_020df300(pl), frame);
    }
    const char *cam = data_0209f318;
    static unsigned last_cam8 = 2, last_cut = 2;
    const unsigned cam8 = cam ? (*(const unsigned *)(cam + kCamFlags) >> 3) & 1u : 0u;
    const unsigned cut = ((unsigned)data_0209b454[0] & kCut) ? 1u : 0u;
    const bool edge = cam8 != last_cam8 || cut != last_cut;
    last_cam8 = cam8; last_cut = cut;
    if (g_trace && cam && (edge || frame % g_trace == 0)) {
        const int *eye = (const int *)(cam + kCamEye), *at = (const int *)(cam + kCamAt);
        const void *md = *(const void *const *)(cam + kCamMode);
        const int *pp = (const int *)(pl + kPlPos);
        std::fprintf(stderr, "[camshield] f%u%s mode=%s cam8=%u b454cut=%u b464cut=%u eye=%d,%d,%d at=%d,%d,%d me=%d,%d,%d pstate=%08x ev1e=%d ev1d=%d quiet=%u ended=%u\n",
                     frame, edge ? " EDGE" : "", mode_name(md), cam8, cut, (data_0209b464 & kCut) ? 1u : 0u, eye[0] >> 12, eye[1] >> 12, eye[2] >> 12,
                     at[0] >> 12, at[1] >> 12, at[2] >> 12, pp[0] >> 12, pp[1] >> 12, pp[2] >> 12, *(const unsigned *)(pl + kPlState),
                     _ZN5Event6GetBitEj(0x1e) != 0, _ZN5Event6GetBitEj(0x1d) != 0, g_quiet, g_ended);
    }
    if (g_shots[0]) {
        for (const char *q = g_shots; *q;) {
            char *end = nullptr;
            const long n = std::strtol(q, &end, 10);
            if (end == q) break;
            if (frame == (unsigned)n) {
                char nm[64]; std::snprintf(nm, sizeof nm, "camshield_f%ld.bmp", n);
                const void *lfb = ::port_level_fb_ptr;
                const bool w = lfb && ::ntr::ppu_write_bmp(nm, *(const ::ntr::Framebuffer *)lfb);
                std::fprintf(stderr, "[camshield] picture %s %s\n", nm, w ? "written" : "FAILED");
            }
            q = end; while (*q == ',' || *q == ' ') ++q;
        }
    }
}
