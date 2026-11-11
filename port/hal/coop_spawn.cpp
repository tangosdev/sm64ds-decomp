// COOP RUNTIME SPAWNS (phase 2). Run mp1, lane COOPSPAWN1. See coop_spawn.h.
//
// Everything here hangs off the coop world core's callbacks (coop_world.h): a
// frame hook, the level-latched hook, the established hook and the owner's
// snapshot-request hook. No game file is edited; the ROM's own entries are
// called (the block's state change, the star marker's spawn, the actor spawn)
// or its own counters written.
//
// All wire handlers only validate and QUEUE; every game-state change happens
// in the frame hook, after the actor frame, never inside the hub pump.
//
// PORT_HOST_ABI. Actor offsets are the ROM's (32-bit host): uniqueID +0x04,
// param +0x08, class +0x0c, position +0x5c, area +0xcc, placed slot +0x0ce,
// dead flags +0x0f / +0x0e.

#include "coop_spawn.h"
#include "coop_boss.h"
#include "coop_world.h"
#include "coop_wire.h"
#include "comms_hub.h"
#include "hub_presence.h"

#include <windows.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
extern unsigned char data_0209f30c[];
extern unsigned char data_0209f30d[];
extern short data_0209f358[];
void _ZN8daCoin_c19func_ov002_020b16c4EP6Player(void *coin, void *player);       // the ROM's own red coin pickup
void *_ZN8dActor_c4NextEPKS_(const void *prev);
char *_ZN8dActor_c10FindWithIDEj(unsigned id);
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned id, char *prev);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
void _ZN7fBase_c18MarkForDestructionEv(char *c);
void _ZN12daStarBase_c27LinkSilverStarAndStarMarkerEPc(char *a, char *b);
void _ZN8daStar_c19func_ov002_020e7e58Ev(char *c);
void _ZN18daObjHatenaBlock_c19func_ov102_02149da8Ei(void *self, int state);
signed char NumRedCoins(void);
signed char NumVsStarsObtained(void);    // SILVERCOOP1: the ROM's silver-star sum (the gold star's "== 5" test)
extern signed char data_0209f310[];       // the silver-star count per player (GiveVsStars)
int _ZN6Player19func_ov002_020d94ccEv(void *player);   // the ROM's hurt drop of one silver star (SM64DS_SILVER_DROP proof knob)
extern void *STAR_MARKERS[12];             // 0x0209f40c: the actors showing a star glint on the minimap (COOPSILVER3 proof log)
void _ZN13daObjBlockL_c4KillEv(char *self);            // the brick block's break (SM64DS_SILVER_BRICK proof knob)
void func_ov084_021296cc(void *goomba);                // the Goomba's silver star reward (SM64DS_SILVER_GOOMBA proof knob)
void func_ov084_02129498(void *goomba);                // the Goomba's removal at the end of its death
void _ZN12daStarBase_c27SpawnRedCoinStarIfNecessaryEv(void *self);
unsigned DeathTable_GetBit(int id);
void _ZN5Event6SetBitEj(unsigned b);
extern int data_ov089_02132894[];      // Key kinds 0..6: pointers to the kind's SharedFilePtr (src/_ZN3Key13InitResourcesEv.cpp)
extern char data_ov002_0211094c;       // kind 7's model
// STARCASE2 (switch stars)
extern int data_0209b454[];             // the persistent freeze-request word (the star's camera request is 0x4000000)
extern unsigned int data_0209b464;      // this frame's latched copy, the mask Actor::BeforeBehavior reads
extern char *data_0209f318;             // the Camera singleton (+0x154 bit 8 while a star films)
int IsStarCollectedInCurLevel(int starID);
void _ZN12daStarBase_c7CollectEv(char *marker);    // the glass case breaks: sound, hidden, effects (face, port/faces_sync.txt)
}

extern "C" unsigned char data_0209caa0[];             // the live working copy of the save record
extern "C" const void *port_level_fb_ptr;               // coop_boot.cpp: the level loop's top-screen framebuffer (proof pictures)
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

namespace port {
namespace {

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}

enum : unsigned {
    kResendMs     = 100,      // every change goes out 3 times: now, +100, +200
    kSettleFrames = 20,       // frames after the group is live before the census starts
    kRedWait      = 180,      // a joiner stops waiting for the owner's red-coin sum after this many frames
    kJobMs        = 150,
    kRateWindowMs = 1000,
    kRatePerSeat  = 4,        // received SPAWNs per seat per second
    kAckAge       = 20000,
};
enum : int { kNetMax = 24, kLnkMax = 8, kInMax = 48, kBlkOutMax = 16, kAckMax = 64, kRedMax = 96, kPendSpawn = 16 };

// ---------------------------------------------------------------------------
// State.
// ---------------------------------------------------------------------------
bool g_env = false, g_log = false, g_census_log = false;
struct Off { bool block, red, spawn, gone, snap; } g_off = {};
// TOTALS9 must-fail switches (SM64DS_TOTALS_OFF=redlocal,guard,coinstar): the same build with the r3 behaviour back.
struct TOff { bool redlocal, guard, coinstar, keyview, stardup, startaken; } g_toff = {};
CoopSpawnStats g_st = {};
bool g_registered = false;
int g_my = -1;

bool     g_have_key = false;
int      g_lvl = -1, g_act = 0;
unsigned g_gen = 0;
unsigned g_frames = 0;
unsigned g_near_frame = 0; bool g_near_done = true;     // SM64DS_TOTALS_NEAR=<frame>: put my player on the nearest rabbit key (proof knob)
unsigned g_kp_until = 0;                                // TOTALS9 probe: log my Player's state until this frame (set while a rabbit key is out)
unsigned g_tc_frame = 0; int g_tc_val = -1;       // SM64DS_TOTALS_COINS=<frame>:<count>: proof knob, set my coin counter once
bool     g_armed = false;
bool     g_census_on = false;
unsigned g_wm = 0;                 // uid watermark: actors with a larger uid are new
unsigned g_census_lines = 0;

// blocks
struct BlkTrk { void *a; unsigned uid; int last; bool valid; };
BlkTrk g_blk[kSpSlots];
struct BlkOut { bool on; uint16_t slot, cls, seq; int sent; unsigned next; };
BlkOut g_bo[kBlkOutMax];
uint16_t g_bseq = 0;
uint16_t g_lastbseq[kCoopMaxSeats][kSpSlots];

// red coins
struct RedTrk { void *a; unsigned uid; short slot; unsigned char star; bool seen; };
RedTrk g_red[kRedMax]; int g_nred = 0;
int  g_red_prev = 0;
bool g_red_armed = false;
bool g_red_snap_have = false; int g_red_snap_sum = 0;
int g_red_star = 0;               // the star number the level's red coins carry (0 = not seen)

// silver stars (SILVERCOOP1)
struct SilTrk { char *a; unsigned uid; short slot; int area; bool seen, mine, other; };
enum : int { kSilMax = 24, kSilOutMax = 4, kSilGold = 5 };
SilTrk g_sil[kSilMax]; int g_nsil = 0;
int  g_sil_last = -1;              // my count as last seen or written (-1 = not armed yet)
unsigned g_sil_wm = 0;             // uid watermark: a silver star with a larger uid is new (a hurt drop is one)
bool g_sil_snap_have = false, g_sil_snap_done = false; int g_sil_snap_sum = 0;
struct EarlySil { bool on; int seat; SpSilverWire w; };
EarlySil g_early_sil;
struct SilOut { bool on; uint16_t seq; uint8_t n; int sent; unsigned next; };
SilOut g_so[kSilOutMax];
uint16_t g_sseq = 0;
uint16_t g_lastsseq[kCoopMaxSeats];
unsigned g_sil_drop_frame = 0; bool g_sil_drop_done = true;   // SM64DS_SILVER_DROP=<frame>: the ROM's hurt drop on my player once (proof knob)
unsigned g_sil_pit_frame = 0; bool g_sil_pit_done = true;     // SM64DS_SILVER_PIT=<frame>[:nomarker]: from that frame, lift the first bouncing dropped silver star out of the level's bounds once (proof knob)
bool g_sil_pit_nomarker = false;
bool g_sil_drop_nofree = false;    // SM64DS_SILVER_DROP=<frame>:nofree: no star marker counts as free during that drop (proof knob: zero free markers)   // ...and first forget its marker, so the ROM destroys even the dropper's own star
long long g_sil_goto_wall = 0;     // SM64DS_SILVER_GOTO_WALL=<wall ms>: at that machine time put my player on the lowest-slot silver star once (proof knob: two consoles on one star at one moment)
bool g_sil_off = false;            // SM64DS_SPAWN_OFF=silver: must-fail control, no silver sharing at all
struct SilStats { unsigned long long credit_take, credit_ann, credit_snap, capped, drop_sent, drop_applied, drop_local, ghost_take, sent_snap, recv_snap, fell, fell_take,
                  auth_sent, auth_recv, auth_adopt, auth_late, auth_left; } g_sst = {};
// COOPSILVER3: one console's count is THE count: the lowest seat in the group that says a count (the "leader"). Not the
// group owner: two consoles that enter a level apart can each stand as owner of the same group (seen in row cntF: seat 0
// owner=0, seat 1 owner=1, each the other's member), and then nobody would follow anybody. The leader keeps counting the way SILVERCOOP1 does (its own takes and
// drops, a friend's take seen as his copy going, a friend's DROP) and says its sum to the group on every change and once a
// second (SILVER COUNT). A member that has heard the owner stops crediting friends' takes and drops itself (it would count
// them a second time next to the owner's number) and takes the owner's sum, but only once its own last count change (a take
// or a hurt drop here) is kSilGraceMs old, so a number the owner said before it heard of that change is never taken.
// A member whose owner never says a count (an older build) keeps counting on its own, as before.
unsigned g_sil_brick_frame = 0; bool g_sil_brick_done = true;     // SM64DS_SILVER_BRICK=<frame>: break (daObjBlockL_c::Kill) the first brick block holding a silver star tag (proof knob)
unsigned g_sil_goomba_frame = 0; bool g_sil_goomba_done = true;   // SM64DS_SILVER_GOOMBA=<frame>: the first silver star Goomba gives its star and is removed, the ROM's two calls (proof knob)
unsigned g_sil_skew_frame = 0; int g_sil_skew = 0; bool g_sil_skew_done = true;   // SM64DS_SILVER_SKEW=<frame>:<delta>: my count moves by delta with nothing said, a lost message's drift (proof knob)
bool g_sil_auth_off = false;       // SM64DS_SILVER_AUTH_OFF=1: must-fail control, every console keeps its own count (the 1.0.2 behaviour)
enum : unsigned { kSilGraceMs = 1500, kSilBeatMs = 1000, kSilSeqStaleMs = 3000, kSilCreditMs = 1300, kSilUnsyncMs = 6000, kSilAloneFrames = 300 };
// REVIEW-C fixes:
// (1) a console says a count only once it knows the room's number ("synced"): it applied the owner's snapshot, or took a
//     leader's count, or stood alone in the group for kSilAloneFrames, or heard no count at all for kSilUnsyncMs after it
//     armed (nobody who knows is around). Until then it follows any member that says a count, whatever its seat, so a late
//     lowest seat never wipes the room's count with its own 0.
// (2) a console that led through a hitch (no word from the leader for kSilSeqStaleMs) gives the lead back the moment the
//     leader is heard again, and takes a LOWER number from it only from the second count it hears after that.
// (3) while the leader has been quiet for more than kSilCreditMs a follower credits friends' takes and drops itself again,
//     so a take in that window is not lost; the leader's next number settles it either way.
bool g_sil_synced = false; unsigned g_sil_armed_t = 0; int g_sil_alone_frames = 0;
unsigned g_sil_led_t = 0; bool g_sil_led = false;                 // when I last said a count as leader
bool g_sil_regain = false; unsigned g_sil_regain_t = 0;          // the leader is back after I led: lower numbers wait a beat
unsigned g_sil_change_t = 0; bool g_sil_changed = false;     // comms time of my last count change by a game event here
int g_sil_sent_sum = -1; unsigned g_sil_beat_t = 0, g_sil_burst_t = 0; int g_sil_burst = 0; uint16_t g_scseq = 0;
bool g_sil_auth_have = false; int g_sil_auth_sum = 0; int g_sil_auth_owner = -1;    // the last count heard, and the leader seat I follow
unsigned g_cnt_heard_t[kCoopMaxSeats]; bool g_cnt_heard[kCoopMaxSeats];             // when each member last said a count
uint16_t g_lastcseq[kCoopMaxSeats]; unsigned g_lastcseq_t[kCoopMaxSeats];

// announced actors
struct Net {
    bool used, remote, gone;
    uint8_t origin, kind; uint16_t counter, cls; uint32_t param; int32_t pos[3]; int8_t area;
    char *a; unsigned uid; bool seen;
    bool sil_took, sil_fell;      // SILVERCOOP1: a player took this silver star here / my copy fell off the level (the others keep theirs)
    bool alias; uint8_t alias_o; uint16_t alias_c;      // bossfix1: the other console's id for the SAME boss reward (each console keeps its own native key)
    uint8_t extra;                                       // coopgoomboss2: a Goomboss minion's index (the wire's reserved byte)
    bool took;                    // SNOWMAN2: a player took this star (its GONE says so)
    int sends; unsigned next; int gsends; unsigned gnext;
};
Net g_net[kNetMax];
// SNOWMAN2 (Tango: "Snowman's head: desynced snowball, 3 stars"): the star numbers a player took in this visit of the level. Every console
// runs its own snowman, penguin race and Koopa race, so each one that finishes makes the star again; the copy check above only sees copies
// that stand at the same moment, so finishes far apart made one star each. A star number in this set is never made again here.
bool g_star_taken[256];
void star_taken_mark(unsigned param, const char *why) {
    if (g_star_taken[param & 0xff]) return;
    g_star_taken[param & 0xff] = true;
    if (g_log) std::fprintf(stderr, "[coopspawn] star taken: star %u is done for this visit (%s) frame=%u wall=%lld\n", param & 0xff, why, g_frames, wall_ms());
}
bool key_model_ready(unsigned param);
uint16_t g_counter = 0;
struct Lnk { bool used, taken; char *a; unsigned uid; short mslot; int area; bool seen; int gsends; unsigned gnext; unsigned param; };
Lnk g_lnk[kLnkMax];
bool g_lnk_gone[kSpSlots];
struct Ack { bool on; uint8_t origin; uint16_t counter; unsigned t; };
Ack g_ack[kAckMax];

// inbound queue (filled by the hub handlers, drained in the frame)
enum : int { kEvSpawn = 1, kEvGone = 2, kEvBlock = 3, kEvSilverDrop = 4 };
struct InEv { bool on; int type, seat; unsigned born; unsigned char buf[32]; };
InEv g_in[kInMax];
struct Rate { unsigned t0; int n; };
Rate g_rate[kCoopMaxSeats];

// the owner's answers to snapshot requests
struct Job { bool on; int seat; uint32_t nonce; int sent; unsigned next; unsigned born; };
Job g_job[kCoopMaxSeats];
// an early RED from a sender not yet a member
struct EarlyRed { bool on; int seat; SpRedWire w; };
EarlyRed g_early;

// proof knobs
struct BlockKnob { unsigned frame; int cls, nth; bool done; };
BlockKnob g_kb[4]; int g_nkb = 0;
struct RedKnob { unsigned frame; int nth; bool done; };
RedKnob g_kr[10]; int g_nkr = 0;
struct StarKnob { unsigned frame; int cls; unsigned param; bool done; };
StarKnob g_ks[4]; int g_nks = 0;
struct Forge { unsigned frame; char name[12]; bool done; };
Forge g_forge[24]; int g_nforge = 0;
struct TakeKnob { unsigned frame; bool done; };
TakeKnob g_kt[4]; int g_nkt = 0;

// switch stars (STARCASE2). daStar_c offsets (include/daStar_c.h): +0x80 scale X, +0xb0 mFlags, +0x434 mMarkerID (the glass
// case), +0x438 mPlayer, +0x43c mKind, +0x440 mState, +0x496 mCamSeq (0xffff = no camera), +0x49d mStarID, +0x4a2 mStarFlags
// (4 see-through, 0x80 switch star, 0x100 switch on). daObjSwitch_c (include/daObjSwitch_c.h): +0x338 mTimer (u16), +0x33a
// mTimeLimit (u16), +0x340 mState, +0x344 mTargetActorID, +0x34d mMusicFadeDone. daStarBase_c: +0x1db mBits (2 = visible).
enum : unsigned { kCut = 0x4000000u };   // CUTSCENE_FLAG (src/game/actors/d_a_star.cpp): the star's camera request
enum : int { kSwpMax = 4, kSwsMax = 4, kSwHold = 64, kSwMusicWait = 240, kSwTakeFrames = 180, kSwTakeArm = 3600 };
struct SwPress { char *a; unsigned uid; int last; bool note, remote; };     // a star switch: who pressed it last
SwPress g_swp[kSwpMax]; int g_nswp = 0;
struct SwStar {
    char *a; unsigned uid; uint8_t id; bool seen;
    bool rm, expired; unsigned home_since;   // a friend took it: removal phase, switch timer ended, frame its switch got home
    int hold; uint16_t held;                 // a friend's press: the switch timer held while the presser's camera would run
    unsigned sup;                            // camera requests taken back
    int logf;
};
SwStar g_sws[kSwsMax]; int g_nsws = 0;
bool g_sw_gone[8];                           // star numbers a friend took this visit
struct SwSend { int n; unsigned next; } g_swsend[8];      // my take, told 3 times
struct SwOff { bool cam, gone, look; } g_swoff = {};
struct SwTake { unsigned frame, wait; bool done; };
SwTake g_swtake[4]; int g_nswtake = 0; unsigned g_swtake_until = 0, g_swtake_wait = 0, g_swtake_full = 0;
struct SwStats { unsigned long long sup, held, look_bit, look_gone, sent, recv, removed, expired, cases; } g_swst = {};
unsigned g_sw_post = 0; char *g_sw_post_sw = nullptr; unsigned g_sw_post_uid = 0;     // proof lines after a removal

// ---------------------------------------------------------------------------
// Env.
// ---------------------------------------------------------------------------
void read_env() {
    if (g_env) return;
    g_env = true;
    const char *l = std::getenv("SM64DS_COOP_LOG");
    g_log = l && *l && *l != '0';
    const char *c = std::getenv("SM64DS_SPAWN_CENSUS");
    g_census_log = c && *c && *c != '0';
    if (const char *o = std::getenv("SM64DS_TOTALS_NEAR")) {
        unsigned f = 0;
        if (std::sscanf(o, "%u", &f) == 1) { g_near_frame = f; g_near_done = false; }
    }
    if (const char *o = std::getenv("SM64DS_TOTALS_COINS")) {
        unsigned f = 0; int v = 0;
        if (std::sscanf(o, "%u:%i", &f, &v) == 2) { g_tc_frame = f; g_tc_val = v; }
    }
    if (const char *o = std::getenv("SM64DS_TOTALS_OFF")) {
        g_toff.redlocal = std::strstr(o, "redlocal") != nullptr;
        g_toff.guard = std::strstr(o, "guard") != nullptr;
        g_toff.coinstar = std::strstr(o, "coinstar") != nullptr;
        g_toff.keyview = std::strstr(o, "keyview") != nullptr;
        g_toff.startaken = std::strstr(o, "startaken") != nullptr;    // SNOWMAN2 control: a star number taken this visit can be made again
        g_toff.stardup = std::strstr(o, "stardup") != nullptr;      // COOPCOUNT2 control: two consoles' copies of one star both stand      // SMALLS9B control: the old behaviour (a watcher builds the key copy)
    }
    if (const char *o = std::getenv("SM64DS_SPAWN_OFF")) {
        g_off.block = std::strstr(o, "block") != nullptr;
        g_off.red = std::strstr(o, "red") != nullptr;
        g_off.spawn = std::strstr(o, "spawn") != nullptr;
        g_off.gone = std::strstr(o, "gone") != nullptr;
        g_off.snap = std::strstr(o, "snap") != nullptr;
        g_sil_off = std::strstr(o, "silver") != nullptr;
    }
    if (const char *e = std::getenv("SM64DS_SILVER_GOTO_WALL")) g_sil_goto_wall = std::atoll(e);
    if (const char *e = std::getenv("SM64DS_SILVER_AUTH_OFF")) g_sil_auth_off = *e && *e != '0';
    if (const char *e = std::getenv("SM64DS_SILVER_SKEW")) { unsigned f = 0; int d = 0; if (std::sscanf(e, "%u:%i", &f, &d) == 2) { g_sil_skew_frame = f; g_sil_skew = d; g_sil_skew_done = false; } }
    if (const char *e = std::getenv("SM64DS_SILVER_BRICK")) { unsigned f = 0; if (std::sscanf(e, "%u", &f) == 1) { g_sil_brick_frame = f; g_sil_brick_done = false; } }
    if (const char *e = std::getenv("SM64DS_SILVER_GOOMBA")) { unsigned f = 0; if (std::sscanf(e, "%u", &f) == 1) { g_sil_goomba_frame = f; g_sil_goomba_done = false; } }
    if (const char *e = std::getenv("SM64DS_SILVER_DROP")) {
        unsigned f = 0;
        if (std::sscanf(e, "%u", &f) == 1) { g_sil_drop_frame = f; g_sil_drop_done = false; }
        g_sil_drop_nofree = std::strstr(e, "nofree") != nullptr;
    }
    if (const char *e = std::getenv("SM64DS_SILVER_PIT")) {
        unsigned f = 0;
        if (std::sscanf(e, "%u", &f) == 1) { g_sil_pit_frame = f; g_sil_pit_done = false; }
        g_sil_pit_nomarker = std::strstr(e, "nomarker") != nullptr;
    }
    if (const char *e = std::getenv("SM64DS_SPAWN_BLOCK")) {
        const char *p = e;
        while (*p && g_nkb < 4) {
            unsigned f = 0; int cl = 0, n = 0, used = 0;
            if (std::sscanf(p, "%u:%i:%i%n", &f, &cl, &n, &used) < 3 || used <= 0) {
                used = 0; n = 0;
                if (std::sscanf(p, "%u:%i%n", &f, &cl, &used) < 2 || used <= 0) break;
            }
            g_kb[g_nkb++] = {f, cl, n, false};
            p += used;
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
    if (const char *e = std::getenv("SM64DS_SPAWN_RED")) {
        const char *p = e;
        while (*p && g_nkr < 10) {
            unsigned f = 0; int n = 0, used = 0;
            if (std::sscanf(p, "%u:%i%n", &f, &n, &used) < 2 || used <= 0) break;
            g_kr[g_nkr++] = {f, n, false};
            p += used;
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
    if (const char *e = std::getenv("SM64DS_SPAWN_STAR")) {
        const char *p = e;
        while (*p && g_nks < 4) {
            unsigned f = 0, pr = 0; int cl = 0, used = 0;
            if (std::sscanf(p, "%u:%i:%u%n", &f, &cl, &pr, &used) < 3 || used <= 0) break;
            g_ks[g_nks++] = {f, cl, pr, false};
            p += used;
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
    if (const char *e = std::getenv("SM64DS_SPAWN_TAKE")) {      // frame list: put my player on a runtime star / key
        const char *p = e;
        while (*p && g_nkt < 4) {
            unsigned f = 0; int used = 0;
            if (std::sscanf(p, "%u%n", &f, &used) < 1 || used <= 0) break;
            g_kt[g_nkt++] = {f, false};
            p += used;
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
    if (const char *f = std::getenv("SM64DS_SPAWN_FORGE")) {
        const char *p = f;
        while (*p && g_nforge < 24) {
            unsigned fr = 0; char nm[12] = {}; int used = 0;
            if (std::sscanf(p, "%u:%11[a-z0-9]%n", &fr, nm, &used) < 2 || used <= 0) break;
            g_forge[g_nforge].frame = fr; std::memcpy(g_forge[g_nforge].name, nm, sizeof nm);
            g_forge[g_nforge].done = false; ++g_nforge;
            p += used;
            while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
    if (const char *o = std::getenv("SM64DS_SWSTAR_OFF")) {
        g_swoff.cam = std::strstr(o, "cam") != nullptr;
        g_swoff.gone = std::strstr(o, "gone") != nullptr;
        g_swoff.look = std::strstr(o, "look") != nullptr;
    }
    if (const char *e = std::getenv("SM64DS_SWSTAR_TAKE")) {      // F[:W] list: from frame F, W frames after the star is full size, walk onto it
        const char *p = e;
        while (*p && g_nswtake < 4) {
            unsigned f = 0, w = 0; int used = 0;
            if (std::sscanf(p, "%u:%u%n", &f, &w, &used) < 2 || used <= 0) {
                w = 0; used = 0;
                if (std::sscanf(p, "%u%n", &f, &used) < 1 || used <= 0) break;
            }
            g_swtake[g_nswtake] = {f, w, false}; ++g_nswtake;
            p += used;
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
}

// ---------------------------------------------------------------------------
// Actors.
// ---------------------------------------------------------------------------
inline unsigned short a_cls(const char *a) { return *(const unsigned short *)(a + 0x0c); }
inline short a_slot(const char *a) { return *(const short *)(a + 0xce); }
inline unsigned a_uid(const char *a) { return *(const unsigned *)(a + 4); }
inline unsigned a_param(const char *a) { return *(const unsigned *)(a + 8); }
inline int a_area(const char *a) { return (signed char)*(const char *)(a + 0xcc); }
inline bool a_dead(const char *a) { return *(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2; }
bool is_player_ptr(const void *a) {
    for (int i = 0; i < 16; ++i) if (data_0209f394[i] == a) return true;
    return false;
}
char *my_player() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}
bool is_block_cls(unsigned c) { return c >= 20 && c <= 25; }
bool is_ann_cls(unsigned c) { return c == 178 || c == 179 || c == 276 || c == 277 || c == 282 || c == 283 || c == 229 || c == 200; }   // 200: only Goomboss's goombas (coop_boss_minion_mine)
bool is_star_cls(unsigned c) { return c == 178 || c == 179; }

CoopKeyWire wkey() { CoopKeyWire k; k.level = (uint16_t)g_lvl; k.act = (uint8_t)g_act; k.reserved = 0; return k; }
bool others_here() { return (coop_group_member_mask() & ~(1u << g_my)) != 0; }
bool send_world(int sub, const void *p, int n) { return coop_send_world(sub, p, n); }
bool send_ctl(int sub, const void *p, int n) { return coop_send_ctl(sub, p, n); }
bool live_key_is(const CoopKeyWire &k) { int l = -1, a = 0; return coop_group_key(&l, &a) && (int)k.level == l && (int)k.act == a; }
bool ready_for(const CoopKeyWire &k) { return g_have_key && (int)k.level == g_lvl && (int)k.act == g_act; }
bool is_member(int seat) { return seat >= 0 && seat < kCoopMaxSeats && ((coop_group_member_mask() >> seat) & 1u); }
// COOPSILVER3: the silver count's leader: the lowest member seat below mine that said a count in the last kSilSeqStaleMs, else me.
int sil_lead();

// ---------------------------------------------------------------------------
// Acks (net ids that are gone: a late SPAWN of one is dropped).
// ---------------------------------------------------------------------------
void ack_add(int origin, int counter, unsigned now) {
    int free_i = 0; unsigned oldest = 0xFFFFFFFFu; bool any_free = false;
    for (int i = 0; i < kAckMax; ++i) {
        if (g_ack[i].on && g_ack[i].origin == origin && g_ack[i].counter == counter) return;
        if (!g_ack[i].on) { if (!any_free) { free_i = i; any_free = true; } }
        else if (!any_free && g_ack[i].t < oldest) { oldest = g_ack[i].t; free_i = i; }
    }
    g_ack[free_i] = {true, (uint8_t)origin, (uint16_t)counter, now};
}
bool ack_has(int origin, int counter) {
    for (const Ack &a : g_ack) if (a.on && a.origin == origin && a.counter == counter) return true;
    return false;
}

// ---------------------------------------------------------------------------
// Quiet removal and spawn.
// ---------------------------------------------------------------------------
// TOTALS9 (field item 18): an actor a Player is attached to must never be deleted under it. A Player that waits on a deleted actor never
// leaves its pose (the rabbit key's caught cutscene puts the Player in the no-control state and the key releases it): arms up, no input,
// no ground friction. Held, grabbed, in the mouth, ridden, attached: any of the five Player links.
bool player_attached_to(const char *a) {
    for (int i = 0; i < 16; ++i) {
        const char *p = (const char *)data_0209f394[i];
        if (!p) continue;
        for (int off = 0x354; off <= 0x364; off += 4) if (*(const char *const *)(p + off) == a) return true;
    }
    return false;
}
bool g_guard_logged = false;
void kill_quiet(char *a) {
    const unsigned cls = a_cls(a);
    // The rabbit key (229) runs a cutscene on the Player closest to it and ends itself (func_ov085_0212e480): a copy is never removed from
    // outside, whatever stage of that cutscene it is in; its own end releases the Player.
    if (!g_toff.guard && (cls == 229 || player_attached_to(a))) {
        if (g_log && !g_guard_logged) { g_guard_logged = true; std::fprintf(stderr, "[coopspawn] guard: cls=%u not removed from outside (key cutscene / a Player is attached) wall=%lld\n", cls, wall_ms()); }
        return;
    }
    if (cls == 179) {
        char *m = _ZN8dActor_c10FindWithIDEj(*(unsigned *)(a + 0x434));
        if (m) { _ZN12daStarBase_c27LinkSilverStarAndStarMarkerEPc(m, nullptr); _ZN7fBase_c18MarkForDestructionEv(m); }
    }
    if (is_star_cls(cls)) _ZN8daStar_c19func_ov002_020e7e58Ev(a);
    _ZN7fBase_c18MarkForDestructionEv(a);
}

char *do_spawn(const SpSpawnWire &w) {
    const int32_t pos[3] = {w.pos[0], w.pos[1], w.pos[2]};
    if (w.kind == kSpKindSilver) {
        char *m = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb4, 0x50, pos, nullptr, w.area, -1);
        char *s = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb3, w.param, pos, nullptr, w.area, -1);
        if (!m || !s) {
            if (m) _ZN7fBase_c18MarkForDestructionEv(m);
            if (s) _ZN7fBase_c18MarkForDestructionEv(s);
            return nullptr;
        }
        *(unsigned *)(s + 0x434) = a_uid(m);
        _ZN12daStarBase_c27LinkSilverStarAndStarMarkerEPc(m, s);
        return s;
    }
    return _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(w.cls, w.param, pos, nullptr, w.area, -1);
}

// ---------------------------------------------------------------------------
// Announced actors.
// ---------------------------------------------------------------------------
Net *net_find(int origin, int counter) {
    for (Net &n : g_net) if (n.used && ((n.origin == origin && n.counter == counter) || (n.alias && n.alias_o == origin && n.alias_c == counter))) return &n;
    return nullptr;
}
Net *net_alloc() {
    for (Net &n : g_net) if (!n.used) return &n;
    for (Net &n : g_net) if (n.used && n.gone) return &n;       // reuse a finished one
    return nullptr;
}

// bossfix1: ONE boss reward (key 282, Grand Star 283) per group. A key can only be built where its model is loaded, and the model belongs
// to the boss on that console (Chief Chilly / Goomboss load it in InitResources, it goes with them), so a console cannot take a key built
// elsewhere once its own boss is gone. Every console therefore keeps the key its own boss makes, and the two ids are made ALIASES of one
// reward: a take on either console sends the id it knows, and the other console finds its key through the alias. A console whose boss
// was not finished when the announced key arrived builds the announced one (its boss still has the model) and removes the native one it
// makes later.
Net *reward_net(int cls) {
    static const bool off = [] { const char *e = std::getenv("SM64DS_COOP_REWARD"); return e && *e == '0'; }();     // must-fail control: every reward is its own
    if (off || (cls != 282 && cls != 283)) return nullptr;
    for (Net &n : g_net) if (n.used && !n.gone && n.cls == cls) return &n;
    return nullptr;
}

bool register_local(char *a, int cls, unsigned param, const int pos[3], int area) {
    if (Net *o = reward_net(cls)) {
        if (o->remote) {                          // the announced copy is already here: it stands, this native one goes
            if (a) kill_quiet(a);
            if (g_log) std::fprintf(stderr, "[coopspawn] reward: seat %d's announced cls=%d is already here, my native copy removed wall=%lld\n", (int)o->origin, cls, wall_ms());
            return false;
        }
    }
    // SNOWMAN2: a player already took this star number in this visit (here or on a friend's console): my late finish makes no new one.
    if (!g_toff.startaken && cls == 178 && a && g_star_taken[param & 0xff]) {
        kill_quiet(a);
        ++g_st.spawn_dup;
        if (g_log) std::fprintf(stderr, "[coopspawn] star taken: my native star %u was already taken this visit: removed, not announced frame=%u wall=%lld\n",
                                param & 0xff, g_frames, wall_ms());
        return false;
    }
    // COOPCOUNT2: the same star number already came from a friend (his announce beat my native spawn): the lower seat's copy stays
    // (see apply_spawn). A lower seat's copy here: mine goes unannounced. A higher seat's: his copy goes and mine is announced.
    if (!g_toff.stardup && cls == 178 && a) {
        for (Net &o : g_net) {
            if (!o.used || o.gone || !o.remote || o.cls != 178 || (o.param & 0xff) != (param & 0xff) || !o.a || a_dead(o.a)) continue;
            if (o.origin < g_my) {
                kill_quiet(a);
                if (g_log) std::fprintf(stderr, "[coopspawn] star dup: my native star %u is seat %d's (the lower seat, id %d/%d): mine removed, not announced wall=%lld\n",
                                        param & 0xff, (int)o.origin, (int)o.origin, (int)o.counter, wall_ms());
                return false;
            }
            if (g_log) std::fprintf(stderr, "[coopspawn] star dup: my native star %u is also seat %d's (id %d/%d): his copy removed here, mine stays wall=%lld\n",
                                    param & 0xff, (int)o.origin, (int)o.origin, (int)o.counter, wall_ms());
            kill_quiet(o.a);
            break;
        }
    }
    Net *n = net_alloc();
    if (!n) { ++g_st.drop_full; return false; }
    ++g_counter; if (!g_counter) g_counter = 1;
    *n = Net();
    n->used = true; n->origin = (uint8_t)g_my; n->counter = g_counter; n->cls = (uint16_t)cls;
    n->kind = (uint8_t)(cls == 179 ? kSpKindSilver : kSpKindActor);
    n->param = param; n->pos[0] = pos[0]; n->pos[1] = pos[1]; n->pos[2] = pos[2]; n->area = (int8_t)area;
    n->a = a; n->uid = a ? a_uid(a) : 0; n->sends = 0; n->next = 0; n->seen = true;
    if (cls == 200 && a) { const unsigned ix = *(const unsigned char *)(a + 0x474); n->extra = (uint8_t)(ix <= 40u ? ix : 0u); }   // mMinionIndex
    ++g_st.spawn_local;
    if (g_log)
        std::fprintf(stderr, "[coopspawn] announce cls=%d param=%u pos=%d,%d,%d area=%d id=%d/%d uid=%u frame=%u wall=%lld\n",
                     cls, param, pos[0] >> 12, pos[1] >> 12, pos[2] >> 12, area, g_my, (int)n->counter, n->uid, g_frames, wall_ms());
    return true;
}

void send_spawn_of(const Net &n) {
    SpSpawnWire w; std::memset(&w, 0, sizeof w);
    w.key = wkey(); w.origin = n.origin; w.kind = n.kind; w.counter = n.counter; w.cls = n.cls;
    w.area = n.area; w.param = n.param; w.pos[0] = n.pos[0]; w.pos[1] = n.pos[1]; w.pos[2] = n.pos[2];
    if (n.cls == 200) w.reserved = n.extra;
    if (send_world(kSpWorldSpawn, &w, sizeof w)) ++g_st.sent_spawn;
}

void send_net(unsigned now) {
    for (Net &n : g_net) {
        if (!n.used) continue;
        if (!n.remote && !n.gone && (int)(now - n.next) >= 0 && n.sends < 3) {
            if (others_here()) { send_spawn_of(n); ++n.sends; n.next = now + kResendMs; }
            else n.sends = 3;
        }
        if (n.gone && n.gsends < 3 && (int)(now - n.gnext) >= 0) {
            if (others_here()) {
                SpGoneWire g; std::memset(&g, 0, sizeof g);
                g.key = wkey(); g.origin = n.origin; g.kind = kSpGoneAnn; g.id = n.counter;
                g.reserved = n.took ? kSpGoneTook : 0u;     // SNOWMAN2: a take, not a copy removed or lost
                if (send_world(kSpWorldGone, &g, sizeof g)) ++g_st.sent_gone;
                ++n.gsends; n.gnext = now + kResendMs;
            } else n.gsends = 3;
        }
    }
    for (Lnk &l : g_lnk) {
        if (!l.used || !l.taken || l.gsends >= 3 || (int)(now - l.gnext) < 0) continue;
        if (!others_here()) { l.gsends = 3; continue; }
        SpGoneWire g; std::memset(&g, 0, sizeof g);
        g.key = wkey(); g.origin = 0xFF; g.kind = kSpGoneLinked; g.id = (uint16_t)l.mslot;
        if (send_world(kSpWorldGone, &g, sizeof g)) ++g_st.sent_gone;
        ++l.gsends; l.gnext = now + kResendMs;
    }
}

// A star that every console spawns natively and links to a PLACED marker is not announced.
bool linked_to_placed_marker(const char *a, short *mslot) {
    const unsigned mid = *(const unsigned *)(a + 0x434);
    if (!mid) return false;
    char *m = _ZN8dActor_c10FindWithIDEj(mid);
    if (!m || a_cls(m) != 0xb4) return false;
    const short s = a_slot(m);
    if (s < 0 || s >= kSpSlots) return false;
    *mslot = s;
    return true;
}

// A prize that comes out of a ? / ! / cap block is that player's own (the block hit is replayed on every
// console and each console spawns its own): never announce a runtime actor born at a block that is mid-hit.
bool born_at_hit_block(const char *a, const int *blk_pos, int nblk) {
    const int *p = (const int *)(a + 0x5c);
    for (int i = 0; i < nblk; ++i) {
        const long long dx = (long long)p[0] - blk_pos[i * 3 + 0], dz = (long long)p[2] - blk_pos[i * 3 + 2];
        const long long dy = (long long)p[1] - blk_pos[i * 3 + 1];
        const long long lim = 100ll << 12;
        if (dx > -lim && dx < lim && dz > -lim && dz < lim && dy > -(150ll << 12) && dy < (150ll << 12)) return true;
    }
    return false;
}

Lnk *lnk_alloc() { for (Lnk &l : g_lnk) if (!l.used) return &l; return nullptr; }

// ---------------------------------------------------------------------------
// Red coins.
// ---------------------------------------------------------------------------
// NumRedCoins reads data_0209f30c[0] and then data_0209f30d[0..n-2]: on the cartridge those are one array, in the port two
// (auto_bss.cpp), so body slot i >= 1 is data_0209f30d[i-1], not data_0209f30c[i].
inline unsigned char &red_cell(int i) { return i == 0 ? data_0209f30c[0] : data_0209f30d[i - 1]; }
int red_slots() { return data_0209f21c > 4 ? 4 : (int)data_0209f21c; }
int red_sum() {
    int s = 0;
    for (int i = 0; i < red_slots(); ++i) s += (signed char)red_cell(i);
    return s;
}
// the body slot a friend's red coin is credited to. TOTALS9 (field item 12): MY slot, the one the HUD draws, so every screen in the stage
// shows the group's red coins; the ROM's sum (NumRedCoins) is the same either way.
int red_remote_slot() {
    const int me = (int)data_0209f250, n = red_slots();
    if (!g_toff.redlocal && me >= 0 && me < n) return me;
    for (int i = 0; i < n; ++i) if (i != me) return i;
    return (me >= 0 && me < n) ? me : -1;      // a one-body count: the sum is all that matters
}
void red_check_star(int star_id) {
    if (NumRedCoins() != 8 || star_id <= 0) return;
    for (char *m = _ZN8dActor_c15FindWithActorIDEjPS_(0xb4, nullptr); m; m = _ZN8dActor_c15FindWithActorIDEjPS_(0xb4, m)) {
        if (*(unsigned char *)(m + 0x1d8) != 0) continue;
        if (*(unsigned char *)(m + 0x1d9) != star_id) continue;
        if (*(unsigned char *)(m + 0x1db) & 4) continue;           // its star is already out
        _ZN12daStarBase_c27SpawnRedCoinStarIfNecessaryEv(m);
        ++g_st.star_spawn_native;
        if (g_log)
            std::fprintf(stderr, "[coopspawn] red coin star: marker slot=%d star=%d sum=%d called the ROM's spawn frame=%u wall=%lld\n",
                         (int)a_slot(m), (int)*(unsigned char *)(m + 0x1d9), (int)NumRedCoins(), g_frames, wall_ms());
    }
}
void red_bump(int n, int star_id, const char *why) {
    const int sl = red_remote_slot();
    if (sl < 0 || n <= 0) return;
    int v = (signed char)red_cell(sl) + n;
    if (v > 0x63) v = 0x63;
    red_cell(sl) = (unsigned char)v;
    g_st.red_remote += (unsigned)n;
    g_red_prev = red_sum();
    if (g_log)
        std::fprintf(stderr, "[coopspawn] red coin %s +%d on body slot %d: sum=%d frame=%u wall=%lld\n", why, n, sl, (int)NumRedCoins(), g_frames, wall_ms());
    red_check_star(star_id);
}

void red_rebase() {
    g_nred = 0;
    g_red_prev = red_sum();
}

// ---------------------------------------------------------------------------
// Silver stars (SILVERCOOP1).
// ---------------------------------------------------------------------------
// The ROM keeps one silver-star count per player, data_0209f310[player] (GiveVsStars: +1 at the take,
// src/game/actors/d_a_star.cpp daStar_c::func_ov002_020e9d18; -1 at a hurt drop, src/game/player/Player.cpp
// Player::func_ov002_020d94cc), draws MY player's count (dMeter_c::RenderSilverStars, src/ui/hud/d_meter.cpp) and grows the
// act's gold star (kind 6) while the sum over the players, NumVsStarsObtained, is exactly 5 (daStar_c::InitResources and
// func_ov002_020ea420). Each console runs only its own player, so a friend's take is credited to MY count, the one the HUD
// draws, never past 5 (the gold star's test is "== 5").
int sil_me() { const int me = (int)data_0209f250; return (me >= 0 && me < 16) ? me : -1; }
int sil_total() { return (int)NumVsStarsObtained(); }
void sil_write(int v) {
    const int me = sil_me();
    if (me < 0) return;
    if (v < 0) v = 0;
    if (v > 0x7f) v = 0x7f;
    data_0209f310[me] = (signed char)v;
    g_sil_last = v;
}
void sil_touch() { g_sil_change_t = comms_hub_now_ms(); g_sil_changed = true; }
// A member following the owner's count (COOPSILVER3): friends' takes and drops reach it as the owner's sum, never here.
int sil_lead() {
    const unsigned now = comms_hub_now_ms();
    const int top = g_sil_synced ? g_my : kCoopMaxSeats;     // not synced yet: any member that says a count leads me
    for (int s = 0; s < top && s < kCoopMaxSeats; ++s)
        if (s != g_my && g_cnt_heard[s] && is_member(s) && (unsigned)(now - g_cnt_heard_t[s]) <= kSilSeqStaleMs) return s;
    return g_my;
}
void sil_set_synced(const char *why) {
    if (g_sil_synced) return;
    g_sil_synced = true;
    if (g_log) std::fprintf(stderr, "[coopsilver] synced (%s): sum=%d frame=%u wall=%lld\n", why, sil_total(), g_frames, wall_ms());
}
bool sil_follow() {
    if (g_sil_auth_off || g_sil_off || g_my < 0) return false;
    const int lead = sil_lead();
    if (lead == g_my) return false;
    return (unsigned)(comms_hub_now_ms() - g_cnt_heard_t[lead]) <= kSilCreditMs;   // a quiet leader: I count friends myself
}
void sil_credit(int n, const char *why) {
    const int me = sil_me();
    if (me < 0 || n <= 0) return;
    if (sil_follow()) {
        ++g_sst.auth_left;
        if (g_log) std::fprintf(stderr, "[coopsilver] credit %s +%d left to seat %d's count: mine=%d sum=%d frame=%u wall=%lld\n", why, n, g_sil_auth_owner, (int)data_0209f310[me], sil_total(), g_frames, wall_ms());
        return;
    }
    const int room = kSilGold - sil_total();
    if (n > room) { g_sst.capped += (unsigned)(n - (room > 0 ? room : 0)); n = room; }
    if (n > 0) { sil_write((int)data_0209f310[me] + n); sil_touch(); }
    if (g_log)
        std::fprintf(stderr, "[coopsilver] credit %s +%d: mine=%d sum=%d frame=%u wall=%lld\n", why, n > 0 ? n : 0, (int)data_0209f310[me], sil_total(), g_frames, wall_ms());
}
bool sil_taken_by(const char *a, const char *pl) {
    const unsigned st = *(const unsigned *)(a + 0x440);
    return pl && *(const char *const *)(a + 0x438) == pl && st >= 5u && st != 8u && st != 9u;
}
// A friend's take of an announced silver star (its GONE arrived while my copy stands, not taken by my player).
void sil_on_remote_gone(const char *a) {
    if (g_sil_off || g_sil_last < 0 || !a || a_dead(a) || a_cls(a) != 179) return;
    if (sil_taken_by(a, my_player())) return;
    for (int i = 0; i < g_nsil; ++i) if (g_sil[i].a == a) g_sil[i].mine = true;    // handled here, never again in the walk
    ++g_sst.credit_ann;
    sil_credit(1, "announced take");
}
void sil_queue_drop(int n) {
    ++g_sseq; if (!g_sseq) g_sseq = 1;
    int fi = 0;
    for (int i = 0; i < kSilOutMax; ++i) if (!g_so[i].on) { fi = i; break; }
    g_so[fi] = {true, g_sseq, (uint8_t)(n > 5 ? 5 : n), 0, 0};
    ++g_sst.drop_local;
}
void sil_send_drops(unsigned now) {
    for (SilOut &o : g_so) {
        if (!o.on || (int)(now - o.next) < 0) continue;
        if (!others_here()) { o.on = false; continue; }
        SpSilverDropWire w; std::memset(&w, 0, sizeof w);
        w.key = wkey(); w.seq = o.seq; w.n = o.n;
        if (send_world(kSpWorldSilverDrop, &w, sizeof w)) ++g_sst.drop_sent;
        if (++o.sent >= 3) o.on = false; else o.next = now + kResendMs;
    }
}
void sil_arm() {
    const int me = sil_me();
    g_nsil = 0;
    g_sil_armed_t = comms_hub_now_ms(); g_sil_alone_frames = 0;
    g_sil_last = me >= 0 ? (int)data_0209f310[me] : 0;
    unsigned m = g_sil_wm;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) if (a_uid(a) > m) m = a_uid(a);
    g_sil_wm = m;
}
void sil_scan() {
    if (g_sil_off || g_sil_last < 0) return;
    const int me = sil_me();
    if (me < 0) return;
    const char *pl = my_player();
    for (int i = 0; i < g_nsil; ++i) g_sil[i].seen = false;
    int new_drops = 0;
    unsigned maxuid = g_sil_wm;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (a_cls(a) != 179 || is_player_ptr(a)) continue;
        const unsigned uid = a_uid(a);
        if (uid > maxuid) maxuid = uid;
        if (uid > g_sil_wm && a_slot(a) == -1 && (a_param(a) & 0xffu) == 0x10u) ++new_drops;
        if (a_dead(a)) continue;
        SilTrk *t = nullptr;
        for (int i = 0; i < g_nsil; ++i) if (g_sil[i].a == a && g_sil[i].uid == uid) { t = &g_sil[i]; break; }
        if (!t && g_nsil < kSilMax) { t = &g_sil[g_nsil++]; *t = {a, uid, a_slot(a), a_area(a), false, false, false}; }
        if (!t) continue;
        t->seen = true; t->area = a_area(a);
        const char *by = *(const char *const *)(a + 0x438);
        const unsigned st = *(const unsigned *)(a + 0x440);
        if (by && st >= 5u && st != 8u && st != 9u) {
            if (by == pl) { t->mine = true; sil_touch(); }      // COOPSILVER3: my take is under way: the leader's number waits
            else if (is_player_ptr(by)) t->other = true;
        }
    }
    g_sil_wm = maxuid;
    // my own count: a rise is my take (the ROM's), a fall with a new dropped star is my hurt drop, told to the group
    const int cur = (int)data_0209f310[me];
    if (cur != g_sil_last) sil_touch();
    if (cur < g_sil_last) {
        const int n = g_sil_last - cur;
        if (new_drops > 0) sil_queue_drop(n);
        if (g_log) std::fprintf(stderr, "[coopsilver] my count fell %d -> %d (%s) frame=%u wall=%lld\n", g_sil_last, cur,
                                new_drops > 0 ? "hurt drop, sent" : "no dropped star, not sent", g_frames, wall_ms());
    } else if (cur > g_sil_last && g_log) {
        std::fprintf(stderr, "[coopsilver] my take: mine=%d sum=%d frame=%u wall=%lld\n", cur, sil_total(), g_frames, wall_ms());
    }
    g_sil_last = cur;
    // placed stars gone with their death bit up and not taken by my player: a friend's take (the death-bit sync removed my copy)
    for (int i = 0; i < g_nsil;) {
        SilTrk &t = g_sil[i];
        if (t.seen) { ++i; continue; }
        if (!t.mine && t.slot >= 0 && t.slot < kSpSlots && DeathTable_GetBit(t.slot)) {
            if (t.other) {
                ++g_sst.ghost_take;
                if (g_log) std::fprintf(stderr, "[coopsilver] slot %d taken by another body on this console: the ROM counted it there frame=%u wall=%lld\n", (int)t.slot, g_frames, wall_ms());
            } else {
                ++g_sst.credit_take;
                char why[40]; std::snprintf(why, sizeof why, "placed slot %d", (int)t.slot);
                sil_credit(1, why);
            }
        }
        g_sil[i] = g_sil[--g_nsil];
    }
}
void apply_silver_drop(int seat, const SpSilverDropWire &w) {
    const int me = sil_me();
    if (me < 0 || g_sil_last < 0) return;
    if (sil_follow()) {
        ++g_sst.auth_left;
        if (g_log) std::fprintf(stderr, "[coopsilver] seat %d dropped %d: left to seat %d's count: mine=%d sum=%d frame=%u wall=%lld\n", seat, (int)w.n, g_sil_auth_owner, (int)data_0209f310[me], sil_total(), g_frames, wall_ms());
        return;
    }
    sil_write((int)data_0209f310[me] - (int)w.n);
    sil_touch();
    ++g_sst.drop_applied;
    if (g_log) std::fprintf(stderr, "[coopsilver] seat %d dropped %d: mine=%d sum=%d frame=%u wall=%lld\n", seat, (int)w.n, (int)data_0209f310[me], sil_total(), g_frames, wall_ms());
}
void apply_silver_snap() {
    if (!g_sil_snap_have || g_sil_snap_done || g_sil_last < 0) return;
    g_sil_snap_done = true;
    const int have = sil_total();
    if (!g_sil_auth_off) {
        if (g_log) std::fprintf(stderr, "[coopsilver] snapshot from the owner: sum=%d (mine %d) wall=%lld\n", g_sil_snap_sum, have, wall_ms());
        if (sil_lead() != g_my) return;          // a member already says the count: its number syncs me (row rlate0: a snapshot here, then leading with 0, wiped the room)
        if (g_sil_snap_sum > have) { ++g_sst.credit_snap; sil_credit(g_sil_snap_sum - have, "snapshot"); }
        sil_set_synced("owner's snapshot");
        return;
    }
    if (g_log) std::fprintf(stderr, "[coopsilver] snapshot from the owner: sum=%d (mine %d) wall=%lld\n", g_sil_snap_sum, have, wall_ms());
    if (g_sil_snap_sum > have) { ++g_sst.credit_snap; sil_credit(g_sil_snap_sum - have, "snapshot"); }
}

// COOPSILVER3: the owner says its count; a member takes it.
int sil_clamped_total() { int s = sil_total(); if (s < 0) s = 0; if (s > kSilGold) s = kSilGold; return s; }
void sil_auth_send(unsigned now) {
    if (g_sil_off || g_sil_auth_off || !g_armed || g_sil_last < 0) return;
    if (sil_lead() != g_my) { g_sil_sent_sum = -1; g_sil_burst = 0; return; }
    if (!others_here()) {
        if (++g_sil_alone_frames >= (int)kSilAloneFrames) sil_set_synced("alone in the group");
        return;
    }
    g_sil_alone_frames = 0;
    if (!g_sil_synced) {
        bool heard = false;
        for (int s = 0; s < kCoopMaxSeats; ++s) heard = heard || (g_cnt_heard[s] && (unsigned)(now - g_cnt_heard_t[s]) <= kSilUnsyncMs);
        if (heard || (unsigned)(now - g_sil_armed_t) < kSilUnsyncMs) return;     // not knowing the room's number, I say nothing
        sil_set_synced("no count heard from anybody");
    }
    const int sum = sil_clamped_total();
    const bool changed = sum != g_sil_sent_sum;
    if (changed) { g_sil_burst = 3; g_sil_burst_t = now; g_sil_sent_sum = sum; }
    bool go = false;
    if (g_sil_burst > 0 && (int)(now - g_sil_burst_t) >= 0) { go = true; --g_sil_burst; g_sil_burst_t = now + kResendMs; }
    else if ((unsigned)(now - g_sil_beat_t) >= kSilBeatMs) go = true;
    if (!go) return;
    SpSilverCountWire w; std::memset(&w, 0, sizeof w);
    ++g_scseq; if (!g_scseq) g_scseq = 1;
    w.key = wkey(); w.seq = g_scseq; w.sum = (uint8_t)sum;
    if (send_world(kSpWorldSilverCount, &w, sizeof w)) ++g_sst.auth_sent;
    g_sil_beat_t = now; g_sil_led_t = now; g_sil_led = true;
    if (changed && g_log) std::fprintf(stderr, "[coopsilver] count said (leader): sum=%d seq=%u frame=%u wall=%lld\n", sum, (unsigned)w.seq, g_frames, wall_ms());
}
void sil_auth_apply(unsigned now) {
    if (!g_sil_auth_have) return;
    if (!g_armed || g_sil_last < 0) return;               // kept until my level is up
    g_sil_auth_have = false;
    if (g_sil_auth_off || g_sil_off || g_sil_auth_owner != sil_lead() || g_sil_auth_owner == g_my) return;
    const int me = sil_me();
    if (me < 0) return;
    if (g_sil_changed && (unsigned)(now - g_sil_change_t) < kSilGraceMs) { ++g_sst.auth_late; return; }   // said before the owner heard of my change
    const int have = sil_total();
    if (g_sil_regain && (unsigned)(now - g_sil_regain_t) > kSilBeatMs + kSilGraceMs) g_sil_regain = false;
    if (g_sil_regain && g_sil_auth_sum < have && (unsigned)(now - g_sil_regain_t) <= kSilBeatMs + 2 * kResendMs) {     // REVIEW-C (2): a lower number waits one beat
        ++g_sst.auth_late;
        if (g_log) std::fprintf(stderr, "[coopsilver] leader seat %d back after my lead: its lower sum %d waits a beat (mine %d) frame=%u wall=%lld\n",
                                g_sil_auth_owner, g_sil_auth_sum, have, g_frames, wall_ms());
        return;
    }
    sil_set_synced("leader's count");
    if (have == g_sil_auth_sum) return;
    const int mine = (int)data_0209f310[me];
    sil_write(mine + (g_sil_auth_sum - have));
    ++g_sst.auth_adopt;
    if (g_log) std::fprintf(stderr, "[coopsilver] count from leader seat %d: sum %d -> %d (mine %d -> %d) frame=%u wall=%lld\n", g_sil_auth_owner, have, sil_total(), mine, (int)data_0209f310[me], g_frames, wall_ms());
}

// ---------------------------------------------------------------------------
// Switch stars (STARCASE2).
// ---------------------------------------------------------------------------
// A placed STAR_MARKER of kind 6 spawns the switch star on every console at level load (Spawn(0xb2, id | 0x60, .., -1),
// src/game/actors/d_a_star.cpp daStarBase_c::InitResources), and the star spawns its own glass case (STAR_MARKER param
// 0x40, also slot -1). The star switch (class 12) turns it on; it grows in the case with a camera cutscene (STATE_IDLE,
// KIND_6) and, if nobody takes it, shrinks away when the switch's timer runs out. No death slot and a runtime case: the
// death-bit sync and the linked-star path never saw a take, so the presser's copy stayed gold.
inline unsigned short &u16at(char *a, int off) { return *(unsigned short *)(a + off); }
bool is_switch_star(const char *a) {
    return a_cls(a) == 178 && (*(const unsigned short *)(a + 0x4a2) & 0x80u) && *(const int *)(a + 0x43c) == 6;
}
bool star_taken(const char *a) {           // a body has it (STATE_COLLECT_* and on), not the silver states 8 / 9
    const unsigned st = *(const unsigned *)(a + 0x440);
    return st >= 5u && st != 8u && st != 9u && *(const void *const *)(a + 0x438) != nullptr;
}
SwPress *swp_for(char *a, bool add, int last) {
    const unsigned uid = a_uid(a);
    for (int i = 0; i < g_nswp; ++i) if (g_swp[i].a == a && g_swp[i].uid == uid) return &g_swp[i];
    if (!add || g_nswp >= kSwpMax) return nullptr;
    g_swp[g_nswp] = {a, uid, last, false, false};
    return &g_swp[g_nswp++];
}
SwPress *swp_target(unsigned star_uid) {
    for (int i = 0; i < g_nswp; ++i)
        if (g_swp[i].a && !a_dead(g_swp[i].a) && a_uid(g_swp[i].a) == g_swp[i].uid && *(const unsigned *)(g_swp[i].a + 0x344) == star_uid)
            return &g_swp[i];
    return nullptr;
}
SwStar *sws_for(char *a) {
    const unsigned uid = a_uid(a);
    for (int i = 0; i < g_nsws; ++i) if (g_sws[i].a == a && g_sws[i].uid == uid) return &g_sws[i];
    if (g_nsws >= kSwsMax) return nullptr;
    SwStar &s = g_sws[g_nsws++];
    s = SwStar(); s.a = a; s.uid = uid; s.id = (uint8_t)(*(const unsigned char *)(a + 0x49d) & 7u);
    return &s;
}

void sw_tick(unsigned now) {
    char *pl = my_player();
    // 1. star switches: was the last press mine or a friend's (coop_spawn_note_remote_press)
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (a_cls(a) != 12 || a_dead(a) || is_player_ptr(a)) continue;
        const int st = *(const int *)(a + 0x340);
        SwPress *p = swp_for(a, true, st);
        if (!p) continue;
        if (p->last == 0 && st != 0) {
            p->remote = p->note; p->note = false;
            for (int i = 0; i < g_nsws; ++i) if (*(const unsigned *)(a + 0x344) == g_sws[i].uid) g_sws[i].logf = 240;
            if (g_log) std::fprintf(stderr, "[coopswstar] switch slot=%d pressed by %s frame=%u wall=%lld\n", (int)a_slot(a),
                                    p->remote ? "a friend: the star's camera is not mine" : "my player: the camera is mine", g_frames, wall_ms());
        }
        p->last = st;
    }

    // 2. the proof knob: walk my player onto the switch star until the ROM's own touch takes it
    for (int i = 0; i < g_nswtake; ++i)
        if (!g_swtake[i].done && g_armed && g_frames >= g_swtake[i].frame) {
            g_swtake[i].done = true; g_swtake_until = g_frames + kSwTakeArm; g_swtake_wait = g_swtake[i].wait; g_swtake_full = 0;
            std::fprintf(stderr, "[coopswstar-knob] take: armed, walking onto the switch star %u frames after it is full size frame=%u wall=%lld\n",
                         g_swtake_wait, g_frames, wall_ms());
        }

    // 3. switch stars
    for (int i = 0; i < g_nsws; ++i) g_sws[i].seen = false;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (!is_switch_star(a) || a_dead(a) || is_player_ptr(a)) continue;
        SwStar *s = sws_for(a);
        if (!s) continue;
        s->seen = true;
        const int id = s->id;
        SwPress *sp = swp_target(s->uid);
        char *sw = sp ? sp->a : nullptr;
        unsigned short &fl = u16at(a, 0x4a2);
        unsigned &af = *(unsigned *)(a + 0xb0);
        if (star_taken(a)) {
            if (*(char *const *)(a + 0x438) == pl && pl && !g_swoff.gone && g_swsend[id].n == 0 && g_swsend[id].next == 0) {
                g_swsend[id].next = now ? now : 1;
                if (g_log) std::fprintf(stderr, "[coopswstar] my player took switch star %d: telling the group frame=%u wall=%lld\n", id, g_frames, wall_ms());
            }
            if (g_swtake_until && *(char *const *)(a + 0x438) == pl) {
                g_swtake_until = 0;
                std::fprintf(stderr, "[coopswstar-knob] take: the ROM took it (state %d) frame=%u wall=%lld\n", *(const int *)(a + 0x440), g_frames, wall_ms());
            }
            continue;                // a body has it here: the ROM's own collect flow, its own camera
        }
        if (g_swtake_until && pl) {
            const bool full = (fl & 0x100u) && *(const int *)(a + 0x80) == 0x1000;
            if (full && !g_swtake_full) {
                g_swtake_full = g_frames ? g_frames : 1;
                std::fprintf(stderr, "[coopswstar-knob] take: the star is full size frame=%u wall=%lld\n", g_frames, wall_ms());
            }
            if (g_frames > g_swtake_until || (g_swtake_full && g_frames > g_swtake_full + g_swtake_wait + kSwTakeFrames)) {
                g_swtake_until = 0;
                std::fprintf(stderr, "[coopswstar-knob] take: gave up, scale=%x frame=%u wall=%lld\n", *(const int *)(a + 0x80), g_frames, wall_ms());
            } else if (g_swtake_full && g_frames >= g_swtake_full + g_swtake_wait) {
                std::memcpy(pl + 0x5c, a + 0x5c, 12);
            }
        }

        // the collected look: a friend's take heard, or the star bit already in my save (the record merge)
        if (!(fl & 4u)) {
            const bool by_gone = g_sw_gone[id] && !g_swoff.gone;
            const bool by_bit = !g_swoff.look && IsStarCollectedInCurLevel(id) != 0;
            if (by_gone || by_bit) {
                fl |= 4u;
                if (by_gone) ++g_swst.look_gone; else ++g_swst.look_bit;
                s->logf = s->logf > 120 ? s->logf : 120;
                if (g_log) std::fprintf(stderr, "[coopswstar] switch star %d turned see-through (%s) scale=%x frame=%u wall=%lld\n", id,
                                        by_gone ? "a friend took it" : "its bit is in my save", *(const int *)(a + 0x80), g_frames, wall_ms());
            }
        }

        // a friend took it: once my own camera (if I pressed) is done, end its switch, break the case, remove it
        if (g_sw_gone[id] && !g_swoff.gone && !s->rm && !(af & kCut) && u16at(a, 0x496) == 0xffff) {
            s->rm = true; s->hold = 0; s->logf = 600;
            char *m = _ZN8dActor_c10FindWithIDEj(*(const unsigned *)(a + 0x434));
            const bool glass = m && a_cls(m) == 0xb4 && (*(const unsigned char *)(m + 0x1db) & 2u);
            if (glass) { _ZN12daStarBase_c7CollectEv(m); ++g_swst.cases; }
            if (g_log) std::fprintf(stderr, "[coopswstar] removing switch star %d (a friend took it): case %s, switch state=%d scale=%x frame=%u wall=%lld\n", id,
                                    glass ? "broken" : "already gone", sw ? *(const int *)(sw + 0x340) : -1, *(const int *)(a + 0x80), g_frames, wall_ms());
        }

        // only the console whose player pressed the switch films the star: take its camera request back
        bool took_back = false;
        if (((sp && sp->remote) || s->rm) && !g_swoff.cam) {
            unsigned short &seq = u16at(a, 0x496);
            if (seq == 0xffff || seq == 0x64) {
                // not filming yet: the star asked in its state handler, after this frame's camera step. Take the request back
                // (the star sets its mFlags and the global word together, and only while the global bit is clear).
                if (af & kCut) {
                    af &= ~kCut;
                    data_0209b454[0] &= ~(int)kCut;
                    took_back = true; ++s->sup; ++g_swst.sup;
                }
                seq = 0xffff;
            } else if (seq < 0x1f4 && (af & kCut)) {
                // the switch turned it off (func_ov002_020e7104(0)) before the star ran this frame, so the camera step
                // already ran once and moved the camera: end it the ROM's way (0x1f4 puts the saved view back, 0x1f5 ends it)
                seq = 0x1f4;
                if (g_log) std::fprintf(stderr, "[coopswstar] switch star %d: its camera had started, ended at once frame=%u wall=%lld\n", id, g_frames, wall_ms());
            }
        }
        // ...and its switch timer waits as long as the presser's camera holds his (d_a_obj_switch.cpp: mTimer only counts
        // while the global bit is clear), so the star leaves every screen together
        if (sw && sp && sp->remote && !s->rm && !g_swoff.cam) {
            if (took_back && (fl & 0x100u) && *(const int *)(a + 0x80) != 0x1000) {
                if (s->hold == 0) s->held = u16at(sw, 0x338);
                s->hold = kSwHold;
            }
            if (s->hold > 0) {
                --s->hold;
                if (*(const int *)(sw + 0x340) == 2) {
                    unsigned short &t = u16at(sw, 0x338);
                    if (t > s->held) { t = s->held; ++g_swst.held; } else s->held = t;
                }
            }
        }

        if (s->rm) {
            const int sst = sw ? *(const int *)(sw + 0x340) : 0;
            if (sw && sst == 2 && !s->expired) {          // the ROM's own end: the timer runs out (state 2 -> 4 -> 3 -> 0)
                u16at(sw, 0x338) = (unsigned short)(u16at(sw, 0x33a) + 1);
                s->expired = true; ++g_swst.expired;
                if (g_log) std::fprintf(stderr, "[coopswstar] switch of star %d timed out early frame=%u wall=%lld\n", id, g_frames, wall_ms());
            }
            if (!sw || sst == 0) { if (!s->home_since) s->home_since = g_frames ? g_frames : 1; } else s->home_since = 0;
            const bool shrunk = *(const int *)(a + 0x80) == 0 && !(fl & 0x100u);
            const bool music = !sw || *(const unsigned char *)(sw + 0x34d) != 0 || (s->home_since && g_frames - s->home_since >= (unsigned)kSwMusicWait);
            if (shrunk && s->home_since && music && !(af & kCut) && u16at(a, 0x496) == 0xffff) {
                if (sw) u16at(sw, 0x338) = 3;    // with no star the switch draws itself by mTimer / 3 (its Behavior's no-target arm): raised
                if (g_log) std::fprintf(stderr, "[coopswstar] switch star %d removed (camera requests taken back %u) b454=%08x cam8=%d frame=%u wall=%lld\n", id,
                                        s->sup, (unsigned)data_0209b454[0], data_0209f318 ? (int)((*(const unsigned *)(data_0209f318 + 0x154) >> 3) & 1u) : -1,
                                        g_frames, wall_ms());
                kill_quiet(a); ++g_swst.removed;
                s->seen = false;
                g_sw_post = 150; g_sw_post_sw = sw; g_sw_post_uid = sw ? a_uid(sw) : 0;
                continue;
            }
        }

        // proof: what this console runs with while the star appears or goes
        const int sc = *(const int *)(a + 0x80);
        if (s->logf > 0) --s->logf;
        if (g_log && (g_frames % 6u) == 0 && (s->logf > 0 || (sc != 0 && sc != 0x1000))) {
            std::fprintf(stderr, "[coopswstar] probe star=%d mine=%d scale=%x on=%d camseq=%x touch=%d b454cut=%d b464cut=%d cam8=%d sw=%d timer=%u/%u hold=%d sup=%u look=%d frame=%u\n",
                         id, sp ? (int)!sp->remote : -1, sc, (int)((fl >> 8) & 1u), (unsigned)u16at(a, 0x496), (int)!(*(const unsigned *)(a + 0x110 + 0x18) & 1u),
                         (int)(((unsigned)data_0209b454[0] & kCut) != 0), (int)((data_0209b464 & kCut) != 0),
                         data_0209f318 ? (int)((*(const unsigned *)(data_0209f318 + 0x154) >> 3) & 1u) : -1,
                         sw ? *(const int *)(sw + 0x340) : -1, sw ? (unsigned)u16at(sw, 0x338) : 0u, sw ? (unsigned)u16at(sw, 0x33a) : 0u,
                         s->hold, s->sup, (int)((fl >> 2) & 1u), g_frames);
        }
    }
    for (int i = 0; i < g_nsws;) { if (g_sws[i].seen) ++i; else g_sws[i] = g_sws[--g_nsws]; }
    // proof: after a removal, the switch, the freeze mask and the camera stay free
    if (g_sw_post) {
        --g_sw_post;
        if (g_log && (g_sw_post % 30u) == 0) {
            char *sw = g_sw_post_sw && !a_dead(g_sw_post_sw) && a_uid(g_sw_post_sw) == g_sw_post_uid ? g_sw_post_sw : nullptr;
            std::fprintf(stderr, "[coopswstar] after removal: switch state=%d timer=%u drawn=%x b454=%08x b464=%08x cam8=%d frame=%u\n",
                         sw ? *(const int *)(sw + 0x340) : -1, sw ? (unsigned)u16at(sw, 0x338) : 0u, sw ? *(const int *)(sw + 0x324) : 0,
                         (unsigned)data_0209b454[0], data_0209b464,
                         data_0209f318 ? (int)((*(const unsigned *)(data_0209f318 + 0x154) >> 3) & 1u) : -1, g_frames);
        }
    }

    // 4. my takes, told three times
    for (int id = 0; id < 8; ++id) {
        SwSend &o = g_swsend[id];
        if (o.next == 0 || o.n >= 3 || (int)(now - o.next) < 0) continue;
        if (!others_here()) { o.n = 3; continue; }
        SpGoneWire g; std::memset(&g, 0, sizeof g);
        g.key = wkey(); g.origin = 0xFF; g.kind = kSpGoneSwitch; g.id = (uint16_t)id;
        if (send_world(kSpWorldGone, &g, sizeof g)) { ++g_st.sent_gone; ++g_swst.sent; }
        ++o.n; o.next = now + kResendMs;
    }
}

// ---------------------------------------------------------------------------
// One walk of the live actors per frame.
// ---------------------------------------------------------------------------
void scan(unsigned now) {
    // mark
    for (Net &n : g_net) n.seen = false;
    for (Lnk &l : g_lnk) l.seen = false;
    for (int i = 0; i < g_nred; ++i) g_red[i].seen = false;
    unsigned maxuid = g_wm;
    int blk_pos[8 * 3]; int nblk = 0;      // blocks mid-hit this frame

    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (is_player_ptr(a)) continue;
        const unsigned cls = a_cls(a);
        const short slot = a_slot(a);
        const bool dead = a_dead(a);
        const unsigned uid = a_uid(a);

        // ---- blocks -------------------------------------------------------
        if (is_block_cls(cls) && slot >= 0 && slot < kSpSlots && !dead) {
            const int st = *(const int *)(a + 0x3e8);
            if (st != 0 && nblk < 8) {
                const int *p = (const int *)(a + 0x5c);
                blk_pos[nblk * 3 + 0] = p[0]; blk_pos[nblk * 3 + 1] = p[1]; blk_pos[nblk * 3 + 2] = p[2]; ++nblk;
            }
            BlkTrk &t = g_blk[slot];
            if (!t.valid || t.a != a || t.uid != uid) { t.a = a; t.uid = uid; t.last = st; t.valid = true; }
            else if (st != t.last) {
                const int prev = t.last; t.last = st;
                if (prev == 0 && st != 0 && g_armed) {
                    ++g_bseq; if (!g_bseq) g_bseq = 1;
                    int fi = -1;
                    for (int i = 0; i < kBlkOutMax; ++i) if (!g_bo[i].on) { fi = i; break; }
                    if (fi < 0) fi = 0;
                    g_bo[fi] = {true, (uint16_t)slot, (uint16_t)cls, g_bseq, 0, now};
                    ++g_st.block_local;
                    const char *pl = my_player();
                    if (g_log)
                        std::fprintf(stderr, "[coopspawn] block hit local slot=%d cls=%u hitter=%d state=%d frame=%u wall=%lld\n",
                                     (int)slot, cls, (int)(a[0x3f2] & 0xff), st, g_frames, wall_ms());
                    (void)pl;
                }
            }
        }

        // ---- red coins (placed) -------------------------------------------
        if (cls == 289 && slot >= 0 && slot < kSpSlots && !dead) {
            bool found = false;
            for (int i = 0; i < g_nred; ++i)
                if (g_red[i].slot == slot && g_red[i].a == a && g_red[i].uid == uid) { g_red[i].seen = true; found = true; break; }
            if (!found && g_nred < kRedMax)
                g_red[g_nred++] = {a, uid, slot, *(const unsigned char *)(a + 0x3ab), true};
            const unsigned char st = *(const unsigned char *)(a + 0x3ab);
            if (st >= 1 && st <= 7) g_red_star = st;
        }

        // ---- registered stars and prizes: still here? ---------------------
        if (is_ann_cls(cls)) {
            // a star in its take animation (state 5 on a player), a key flying to the player who took it (state 3, owner +0x110)
            const bool taken_now = !dead && ((is_star_cls(cls) && *(const unsigned *)(a + 0x440) == 5u &&
                                              is_player_ptr(*(void *const *)(a + 0x438))) ||
                                             (cls == 282 && *(const int *)(a + 0x448) == 3 && is_player_ptr(*(void *const *)(a + 0x110))));
            if (cls == 229 && !dead) g_kp_until = g_frames + 3000u;
            if (cls == 229 && g_log && !dead && (g_frames % 20u) == 0) {      // TOTALS9 probe: the key's state machine against my Player's state
                const char *pl = my_player();
                std::fprintf(stderr, "[coopspawn] key probe state=%08x t2c8=%d mine: pstate=%08x ptimer=%d d6e3=%d held=%08x dist=%d frame=%u wall=%lld\n",
                             *(const unsigned *)(a + 0x188), *(const int *)(a + 0x2c8), pl ? *(const unsigned *)(pl + 0x370) : 0u,
                             pl ? (int)*(const unsigned short *)(pl + 0x6a4) : -1, pl ? (int)pl[0x6e3] : -1, pl ? *(const unsigned *)(pl + 0x358) : 0u,
                             pl ? (int)(((long long)*(const int *)(pl + 0x5c) - *(const int *)(a + 0x5c)) >> 12) : -1, g_frames, wall_ms());
            }
            for (Net &n : g_net)
                if (n.used && !n.gone && n.a == a && n.uid == uid) {
                    n.seen = !dead && !taken_now;
                    if (cls == 178 && taken_now && !n.took) { n.took = true; star_taken_mark(n.param, "taken here"); }
                    if (cls == 179 && !dead && (taken_now || (is_player_ptr(*(void *const *)(a + 0x438)) && *(const unsigned *)(a + 0x440) >= 5u &&
                                                              *(const unsigned *)(a + 0x440) != 8u && *(const unsigned *)(a + 0x440) != 9u)))
                        n.sil_took = true;          // SILVERCOOP1: a player has it here
                }
            for (Lnk &l : g_lnk)
                if (l.used && !l.taken && l.a == a && l.uid == uid) {
                    l.seen = !dead && !taken_now;
                    if (taken_now) star_taken_mark(l.param, "taken here (linked)");
                }
        }

        // ---- new runtime actors -------------------------------------------
        if (uid > maxuid) maxuid = uid;
        if (g_census_on && uid > g_wm && !dead && slot == -1) {
            if (g_census_log && g_census_lines < 400) {
                ++g_census_lines;
                const int *p = (const int *)(a + 0x5c);
                std::fprintf(stderr, "[coopspawn] new actor cls=%u uid=%u pos=%d,%d,%d area=%d param=%u frame=%u wall=%lld\n",
                             cls, uid, p[0] >> 12, p[1] >> 12, p[2] >> 12, a_area(a), a_param(a), g_frames, wall_ms());
            }
            // a switch star is made natively on every console (its kind-6 marker): never announced (STARCASE2)
            if (is_ann_cls(cls) && !g_off.spawn && !(cls == 178 && is_switch_star(a))) {
                short mslot = -1;
                bool registered = false;
                for (Net &n : g_net) if (n.used && n.a == a && n.uid == uid) registered = true;
                for (Lnk &l : g_lnk) if (l.used && l.a == a && l.uid == uid) registered = true;
                if (!registered) {
                    if (cls == 178 && linked_to_placed_marker(a, &mslot)) {
                        Lnk *l = lnk_alloc();
                        if (l) {
                            *l = Lnk(); l->used = true; l->a = a; l->uid = uid; l->mslot = mslot; l->area = a_area(a); l->seen = true; l->param = a_param(a);
                            if (g_log)
                                std::fprintf(stderr, "[coopspawn] linked star cls=%u marker slot=%d uid=%u pos=%d,%d,%d frame=%u wall=%lld\n",
                                             cls, (int)mslot, uid, ((const int *)(a + 0x5c))[0] >> 12, ((const int *)(a + 0x5c))[1] >> 12,
                                             ((const int *)(a + 0x5c))[2] >> 12, g_frames, wall_ms());
                            if (!g_toff.startaken && g_star_taken[l->param & 0xff] && !g_lnk_gone[mslot]) {      // SNOWMAN2: taken this visit as an announced star
                                kill_quiet(a); l->taken = true; l->gsends = 3;
                                if (g_log) std::fprintf(stderr, "[coopspawn] star taken: my linked star %u (marker slot=%d) was already taken this visit: removed wall=%lld\n", l->param & 0xff, (int)mslot, wall_ms());
                            } else if (g_lnk_gone[mslot] && !g_off.gone) {       // the friend already took it
                                kill_quiet(a); l->taken = true; l->gsends = 3;
                                ++g_st.gone_applied;
                                if (g_log) std::fprintf(stderr, "[coopspawn] linked star marker slot=%d was already taken: removed wall=%lld\n", (int)mslot, wall_ms());
                            }
                        }
                    } else if ((cls == 276 || cls == 277) && born_at_hit_block(a, blk_pos, nblk)) {
                        // a block's own prize: per player, not announced
                    } else if (cls == 200 && !coop_boss_minion_mine(a)) {
                        // coopgoomboss2: a Goomba that is not a minion of a Goomboss this console is the authority of: not announced
                    } else {
                        const int *p = (const int *)(a + 0x5c);
                        const int pos[3] = {p[0], p[1], p[2]};
                        register_local(a, (int)cls, a_param(a), pos, a_area(a));
                    }
                }
            }
        }
    }

    // ---- gone: announced and linked ----------------------------------------
    // A copy that vanishes while its area is not the one I am standing in was unloaded with the area, not taken:
    // the others still have theirs, so nothing is sent.
    const char *me_pl = my_player();
    const int my_area = me_pl ? a_area(me_pl) : -1;
    for (Net &n : g_net) {
        if (!n.used || n.gone || n.seen || n.sil_fell) continue;
        if (n.cls == 179 && !n.sil_took && !g_sil_off && !(n.area >= 0 && my_area >= 0 && n.area != my_area)) {
            // SILVERCOOP1: a silver star whose copy here vanished with no take fell off the level: on a death floor or out of bounds
            // daStar_c::func_ov002_020e8abc sends a star back to its marker, and destroys one whose marker it can not find (a
            // dropped star finds none when no marker here is free). Nobody took it, so no GONE goes out: the others keep their
            // copy, and a later take of it there still counts here.
            n.sil_fell = true; n.a = nullptr;
            ++g_sst.fell;
            if (g_log) std::fprintf(stderr, "[coopsilver] my copy fell id=%d/%d (no take seen): not a take, nothing sent frame=%u wall=%lld\n", (int)n.origin, (int)n.counter, g_frames, wall_ms());
            continue;
        }
        if (n.cls == 179 && !n.sil_took && !g_sil_off) {
            // COOPSILVER3: a silver star unloaded with its area: no copy here now, but a friend's later take (his GONE) still counts
            n.sil_fell = true; n.a = nullptr;
            if (g_log) std::fprintf(stderr, "[coopsilver] my copy unloaded with its area id=%d/%d area=%d mine=%d: a later take still counts frame=%u wall=%lld\n", (int)n.origin, (int)n.counter, n.area, my_area, g_frames, wall_ms());
            continue;
        }
        n.gone = true; n.gsends = 0; n.gnext = now;
        if (n.area >= 0 && my_area >= 0 && n.area != my_area) {
            n.gsends = 3;
            if (g_log) std::fprintf(stderr, "[coopspawn] unloaded with its area cls=%u id=%d/%d area=%d mine=%d frame=%u wall=%lld\n", (unsigned)n.cls, (int)n.origin, (int)n.counter, n.area, my_area, g_frames, wall_ms());
            continue;
        }
        ++g_st.gone_local;
        if (g_log) std::fprintf(stderr, "[coopspawn] gone local%s cls=%u id=%d/%d frame=%u wall=%lld\n", n.remote ? " (remote copy)" : "", (unsigned)n.cls, (int)n.origin, (int)n.counter, g_frames, wall_ms());
        ack_add(n.origin, n.counter, now);
    }
    for (Lnk &l : g_lnk) {
        if (!l.used || l.taken || l.seen) continue;
        l.taken = true; l.gsends = 0; l.gnext = now;
        if (l.area >= 0 && my_area >= 0 && l.area != my_area) { l.gsends = 3; continue; }
        ++g_st.gone_local;
        if (g_log) std::fprintf(stderr, "[coopspawn] gone local linked star marker slot=%d frame=%u wall=%lld\n", (int)l.mslot, g_frames, wall_ms());
    }

    // ---- red coins gone ------------------------------------------------------
    if (g_red_armed) {
        int gone = 0, star = 0;
        for (int i = 0; i < g_nred;) {
            RedTrk &t = g_red[i];
            if (t.seen) { ++i; continue; }
            if (DeathTable_GetBit(t.slot) && t.star >= 1 && t.star <= 7) { ++gone; star = t.star; }
            g_red[i] = g_red[--g_nred];
        }
        const int sum = red_sum();
        const int inc = sum > g_red_prev ? sum - g_red_prev : 0;
        const int rem = gone - inc;
        if (rem > 0 && !g_off.red) red_bump(rem, star, "remote");
        else g_red_prev = sum;
        (void)star;
    } else {
        // not armed: forget the coins that went away while the level settled
        for (int i = 0; i < g_nred;) {
            if (g_red[i].seen) ++i; else g_red[i] = g_red[--g_nred];
        }
        g_red_prev = red_sum();
    }
    if (g_census_on && maxuid > g_wm) g_wm = maxuid;
    if (g_log && g_frames < g_kp_until && (g_frames % 20u) == 0) {
        const char *pl = my_player();
        int keys = 0;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) if (a_cls(a) == 229 && !a_dead(a)) ++keys;
        std::fprintf(stderr, "[coopspawn] pprobe pstate=%08x ptimer=%d d6e3=%d keys=%d frame=%u wall=%lld\n", pl ? *(const unsigned *)(pl + 0x370) : 0u,
                     pl ? (int)*(const unsigned short *)(pl + 0x6a4) : -1, pl ? (int)pl[0x6e3] : -1, keys, g_frames, wall_ms());
    }
    if (g_log && (g_frames % 60u) == 0)         // TOTALS9 proof: what the HUD draws on this console
        std::fprintf(stderr, "[coopspawn] hud coins=%d redcell=%d redsum=%d frame=%u wall=%lld\n", (int)data_0209f358[(int)data_0209f250 & 3],
                     (int)(signed char)red_cell((int)data_0209f250 < red_slots() ? (int)data_0209f250 : 0), red_sum(), g_frames, wall_ms());
}

// ---------------------------------------------------------------------------
// Inbound events (applied in the frame).
// ---------------------------------------------------------------------------
void push_in(int type, int seat, const void *p, int n, unsigned now) {
    for (InEv &e : g_in)
        if (!e.on) { e.on = true; e.type = type; e.seat = seat; e.born = now; std::memcpy(e.buf, p, (size_t)n); return; }
    ++g_st.drop_full;
}

char *find_block(int slot, int cls) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (a_slot(a) == slot && a_cls(a) == (unsigned)cls && !a_dead(a) && !is_player_ptr(a)) return a;
    return nullptr;
}

void apply_block(int seat, const SpBlockWire &w) {
    char *a = find_block(w.slot, w.cls);
    if (!a) { ++g_st.block_notfound; return; }
    if (*(const int *)(a + 0x3e8) != 0) { ++g_st.block_busy; return; }
    const char *pl = my_player();
    int ch = pl ? (int)(*(const unsigned *)(pl + 8) & 0xff) : 0;
    if (ch < 0 || ch > 3) ch = 0;
    *(unsigned char *)(a + 0x3f2) = (unsigned char)ch;      // MY character takes the prize
    _ZN18daObjHatenaBlock_c19func_ov102_02149da8Ei(a, 1);                               // the block's own "hit" state
    BlkTrk &t = g_blk[w.slot];
    t.a = a; t.uid = a_uid(a); t.last = *(const int *)(a + 0x3e8); t.valid = true;
    ++g_st.block_applied;
    if (g_log)
        std::fprintf(stderr, "[coopspawn] block replay slot=%d cls=%d from seat %d hitter=%d state=%d pos=%d,%d,%d frame=%u wall=%lld\n", (int)w.slot, (int)w.cls,
                     seat, ch, *(const int *)(a + 0x3e8), ((const int *)(a + 0x5c))[0] >> 12, ((const int *)(a + 0x5c))[1] >> 12, ((const int *)(a + 0x5c))[2] >> 12, g_frames, wall_ms());
}

bool near_same(const char *a, const SpSpawnWire &w) {
    const int *p = (const int *)(a + 0x5c);
    const long long lim = 24ll << 12;
    for (int i = 0; i < 3; ++i) {
        const long long d = (long long)p[i] - w.pos[i];
        if (d < -lim || d > lim) return false;
    }
    return true;
}

// A key only exists where the ROM loaded its model: Bowser's three arenas (levels 36, 38, 40, any kind) and Chief Chilly's arena (49,
// kind 4 as in func_ov073_0211fa74). Anywhere else the model is not there and the draw faults (measured: kind 0 in 49 and in BOB), so
// a forged key SPAWN must not get past this. The Big Boo key arena (47) is left out until its kind is proven.
// bossfix1: a key can only be built where its model file is loaded. Chief Chilly and Goomboss load their key's model in their own
// InitResources (LoadKeyModels), so a console where that boss was deleted before it initialised (a dead boss stays dead) has no model;
// spawning the key there faulted (measured: a late joiner applying the announced key). Such a console simply goes without the key.
bool key_model_ready(unsigned param) {
    const unsigned kind = param & 7u;
    if (kind == 7) return *(void *const *)(&data_ov002_0211094c + 4) != nullptr;
    const int sfp = data_ov089_02132894[kind];
    return sfp != 0 && *(void *const *)((const char *)(intptr_t)sfp + 4) != nullptr;
}

bool key_ok(int lvl, unsigned param) {
    if (lvl == 36 || lvl == 38 || lvl == 40) return true;
    if (lvl == 45) return (param & 7u) == 2u;       // Goomboss's key (src/actors/Goomboss.cpp: kind 2)
    if (lvl == 47) return (param & 7u) == 3u;       // the Big Boo key arena (func_ov063_02118ddc: kind 3)
    return lvl == 49 && (param & 7u) == 4u;
}

void apply_spawn(int seat, const SpSpawnWire &w, unsigned now) {
    // SMALLS9B (Tango: "everyone sees the animation of the person when they get the key"): the rabbit key (229) runs its caught cutscene
    // (pose, "You retrieved a key" box) on the Player nearest to it. A copy built here ran that on THIS console's body. The catcher's own
    // console plays it; a watcher sees the catcher's ghost play the key-get animation (the ghost's animation rides SYNG), and the key bit
    // counts for everyone through the shared save (the Mip rabbit flags are monotonic bits of the record). So no copy is built here; the id is
    // acked so a late repeat of the spawn is dropped and the catcher's later "gone" finds nothing to remove.
    if (w.cls == 229 && !g_toff.keyview) {
        ack_add(w.origin, w.counter, now);
        ++g_st.drop_class;
        g_kp_until = g_frames + 3000u;       // proof: the pprobe lines (my Player's state) run on a watcher too
        if (g_log) std::fprintf(stderr, "[coopspawn] key view: seat %d's rabbit key (id %d/%d) not built here, I watch the catcher's ghost frame=%u wall=%lld\n",
                                seat, (int)w.origin, (int)w.counter, g_frames, wall_ms());
        return;
    }
    if (w.cls == 282 && !key_ok(g_lvl, w.param)) { ++g_st.drop_class; return; }
    if (w.cls == 282 && !key_model_ready(w.param)) {
        ++g_st.drop_class;
        if (g_log) std::fprintf(stderr, "[coopspawn] key kind %u refused: its model is not loaded on this console (no boss here loaded it) wall=%lld\n", w.param & 7u, wall_ms());
        return;
    }
    if (w.cls == 283 && g_lvl != 40) { ++g_st.drop_class; return; }           // the Grand Star only exists in Bowser's last arena
    if (w.cls == 200 && !coop_boss_minion_ready()) {                         // coopgoomboss2: a minion only follows a Goomboss that is here
        ack_add(w.origin, w.counter, now);
        ++g_st.drop_class;
        if (g_log) std::fprintf(stderr, "[coopspawn] goomba id=%d/%d refused: no Goomboss on this console wall=%lld\n", (int)w.origin, (int)w.counter, wall_ms());
        return;
    }
    if (ack_has(w.origin, w.counter)) { ++g_st.spawn_dup; return; }
    if (net_find(w.origin, w.counter)) { ++g_st.spawn_dup; return; }
    if (Net *o = reward_net(w.cls)) {
        if (!o->remote && !o->alias) {            // my own native reward is out: the same reward, two ids
            o->alias = true; o->alias_o = w.origin; o->alias_c = w.counter;
            ++g_st.spawn_dup;
            if (g_log) std::fprintf(stderr, "[coopspawn] reward: seat %d's announced cls=%d (id %d/%d) is my native copy's alias wall=%lld\n", seat, (int)w.cls, (int)w.origin, (int)w.counter, wall_ms());
            return;
        }
    }
    // COOPCOUNT2: ONE copy of a star per star number. Two consoles can make the same runtime star inside one message delay (two
    // counters crossing 100 in the same frame, the same last secret coin taken on both, the fifth secret settled on two consoles):
    // each announces its own. Every console keeps the copy of the LOWER origin seat and removes the other (a native one I made goes
    // too; scan then tells the group it is gone, so a third console drops its copy of it). A copy already being taken stays.
    // SNOWMAN2: a player took this star number here before the friend's late copy arrived. It is not built, and the group hears it is
    // gone as a take, so the friend removes his own copy and every console drops it.
    if (!g_toff.startaken && w.cls == 178 && g_star_taken[w.param & 0xff]) {
        ack_add(w.origin, w.counter, now);
        ++g_st.spawn_dup;
        if (Net *t = net_alloc()) {
            *t = Net();
            t->used = true; t->remote = true; t->gone = true; t->took = true; t->origin = w.origin; t->counter = w.counter; t->cls = w.cls;
            t->kind = w.kind; t->param = w.param; t->area = w.area; t->sends = 3; t->gsends = 0; t->gnext = now;
        }
        if (g_log) std::fprintf(stderr, "[coopspawn] star taken: seat %d's star %u (id %d/%d) was already taken this visit: not built, sent back as taken frame=%u wall=%lld\n",
                                seat, w.param & 0xff, (int)w.origin, (int)w.counter, g_frames, wall_ms());
        return;
    }
    if (!g_toff.stardup && w.cls == 178) {
        for (Net &o : g_net) {
            if (!o.used || o.gone || o.cls != 178 || (o.param & 0xff) != (w.param & 0xff) || o.origin == w.origin || !o.a || a_dead(o.a)) continue;
            const bool taking = *(const unsigned *)(o.a + 0x440) == 5u && is_player_ptr(*(void *const *)(o.a + 0x438));
            if (o.origin < w.origin || taking) {
                ack_add(w.origin, w.counter, now);
                ++g_st.spawn_dup;
                if (g_log) std::fprintf(stderr, "[coopspawn] star dup: seat %d's star %u (id %d/%d) is seat %d's star %s: not built here wall=%lld\n", seat,
                                        w.param & 0xff, (int)w.origin, (int)w.counter, (int)o.origin, taking ? "being taken" : "(the lower seat)", wall_ms());
                return;
            }
            if (g_log) std::fprintf(stderr, "[coopspawn] star dup: seat %d's star %u (id %d/%d) is the lower seat's: my copy of seat %d's (id %d/%d) removed wall=%lld\n",
                                    seat, w.param & 0xff, (int)w.origin, (int)w.counter, (int)o.origin, (int)o.origin, (int)o.counter, wall_ms());
            kill_quiet(o.a);
            break;
        }
    }
    Net *n = net_alloc();
    if (!n) { ++g_st.drop_full; return; }
    // the same thing already out here (spawned natively by a replayed event): adopt it, never double it
    char *adopt = nullptr;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (a_cls(a) != w.cls || a_slot(a) != -1 || a_dead(a) || is_player_ptr(a)) continue;
        if ((a_param(a) & 0xff) != (w.param & 0xff) && is_star_cls(w.cls)) continue;
        bool taken = false;
        for (const Net &o : g_net) if (o.used && o.a == a) taken = true;
        for (const Lnk &o : g_lnk) if (o.used && o.a == a) taken = true;
        if (taken || (w.cls != 282 && w.cls != 283 && !near_same(a, w))) continue;     // a boss reward: the native key of this console IS the reward, wherever it stands
        adopt = a; break;
    }
    char *a = adopt ? adopt : (w.cls == 200 ? coop_boss_spawn_minion(w.pos, w.param, w.area, w.reserved) : do_spawn(w));
    if (!a) { ++g_st.spawn_failed; return; }
    *n = Net();
    n->used = true; n->remote = true; n->origin = w.origin; n->kind = w.kind; n->counter = w.counter; n->cls = w.cls;
    n->param = w.param; n->pos[0] = w.pos[0]; n->pos[1] = w.pos[1]; n->pos[2] = w.pos[2]; n->area = w.area;
    n->a = a; n->uid = a_uid(a); n->seen = true; n->sends = 3; n->extra = w.reserved;
    if (a_uid(a) > g_wm) g_wm = a_uid(a);
    // TOTALS9 (field item 11): a friend's 100-coin star (the ROM spawns it with param 0x20 when a counter reaches 100). Mark the ROM's own
    // "100 coin star is out" event bit, so this console's counter, shared with theirs, does not spawn a second one at its next pickup.
    if (!g_toff.coinstar && w.cls == 178 && (w.param & 0xff) == 0x20) {
        _ZN5Event6SetBitEj(0x1f);
        if (g_log) std::fprintf(stderr, "[coopspawn] 100-coin star from seat %d: event bit 0x1f set here wall=%lld\n", seat, wall_ms());
    }
    if (adopt) ++g_st.spawn_adopted; else ++g_st.spawn_applied;
    if (g_log)
        std::fprintf(stderr, "[coopspawn] spawn %s cls=%d from seat %d id=%d/%d pos=%d,%d,%d uid=%u frame=%u wall=%lld\n", adopt ? "adopted" : "applied",
                     (int)w.cls, seat, (int)w.origin, (int)w.counter, w.pos[0] >> 12, w.pos[1] >> 12, w.pos[2] >> 12, n->uid, g_frames, wall_ms());
    (void)now;
}

void apply_gone(int seat, const SpGoneWire &w, unsigned now) {
    if (w.kind == kSpGoneSwitch) {          // STARCASE2: sw_tick removes my copy once its camera is done
        if (g_swoff.gone || w.id >= 8) return;
        ++g_swst.recv;
        if (!g_sw_gone[w.id]) {
            g_sw_gone[w.id] = true; ++g_st.gone_applied;
            if (g_log) std::fprintf(stderr, "[coopswstar] seat %d took switch star %d frame=%u wall=%lld\n", seat, (int)w.id, g_frames, wall_ms());
        }
        return;
    }
    if (w.kind == kSpGoneAnn) {
        ack_add(w.origin, w.id, now);
        Net *n = net_find(w.origin, w.id);
        if (!n) { ++g_st.gone_unknown; return; }
        if (n->cls == 178 && (w.reserved & kSpGoneTook)) star_taken_mark(n->param, "a friend took it");
        if (n->gone) return;
        if (n->a && n->seen && n->cls == 179) sil_on_remote_gone(n->a);      // SILVERCOOP1: a friend's take counts here too
        else if (n->sil_fell && !g_sil_off && g_sil_last >= 0) {     // ...also when my copy of it had fallen
            ++g_sst.fell_take;
            sil_credit(1, "announced take (my copy had fallen)");
        }
        if (n->a && n->seen) kill_quiet(n->a);
        n->gone = true; n->gsends = 3;               // heard, not re-sent: the claimer already told everyone
        ++g_st.gone_applied;
        if (g_log) std::fprintf(stderr, "[coopspawn] gone applied cls=%u id=%d/%d from seat %d frame=%u wall=%lld\n", (unsigned)n->cls, (int)w.origin, (int)w.id, seat, g_frames, wall_ms());
        return;
    }
    // linked star by marker slot
    if (w.id < kSpSlots) g_lnk_gone[w.id] = true;
    for (Lnk &l : g_lnk) {
        if (!l.used || l.mslot != (short)w.id || l.taken) continue;
        if (l.seen && l.a) kill_quiet(l.a);
        if (!g_toff.startaken) star_taken_mark(l.param, "a friend took it (linked)");
        l.taken = true; l.gsends = 3;
        ++g_st.gone_applied;
        if (g_log) std::fprintf(stderr, "[coopspawn] gone applied linked star marker slot=%d from seat %d frame=%u wall=%lld\n", (int)w.id, seat, g_frames, wall_ms());
        return;
    }
    ++g_st.gone_unknown;
}

void drain_in(unsigned now) {
    for (InEv &e : g_in) {
        if (!e.on) continue;
        if ((unsigned)(now - e.born) > 8000u) { e.on = false; continue; }
        if (!g_armed && e.type != kEvGone) continue;         // a joiner waits until its level is up
        e.on = false;
        if (e.type == kEvBlock) { SpBlockWire w; std::memcpy(&w, e.buf, sizeof w); apply_block(e.seat, w); }
        else if (e.type == kEvSpawn) { SpSpawnWire w; std::memcpy(&w, e.buf, sizeof w); apply_spawn(e.seat, w, now); }
        else if (e.type == kEvGone) { SpGoneWire w; std::memcpy(&w, e.buf, sizeof w); apply_gone(e.seat, w, now); }
        else if (e.type == kEvSilverDrop) { SpSilverDropWire w; std::memcpy(&w, e.buf, sizeof w); apply_silver_drop(e.seat, w); }
    }
}

// ---------------------------------------------------------------------------
// Receiving.
// ---------------------------------------------------------------------------
void apply_red_snap(const SpRedWire &w) {
    if (g_off.snap) { ++g_st.drop_off; return; }
    g_red_snap_have = true; g_red_snap_sum = w.sum; if (w.star) g_red_star = w.star;
    ++g_st.red_snap_applied;
    if (g_log) std::fprintf(stderr, "[coopspawn] red snapshot from the owner: sum=%d (mine %d) wall=%lld\n", (int)w.sum, red_sum(), wall_ms());
}

void on_ctl(int seat, int sub, const unsigned char *p, int len, void *) {
    if (!coop_world_active()) return;
    if (sub == kSpCtlRed) {
        ++g_st.recv_red;
        if (!sp_red_validate(p, len)) { ++g_st.drop_bad; return; }
        SpRedWire w; std::memcpy(&w, p, sizeof w);
        if (seat == g_my || w.target != g_my) return;
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!ready_for(w.key) || !is_member(seat)) { g_early.on = true; g_early.seat = seat; g_early.w = w; return; }
        apply_red_snap(w);
        return;
    }
    if (sub == kSpCtlSilver) {          // SILVERCOOP1
        ++g_sst.recv_snap;
        if (!sp_silver_validate(p, len)) { ++g_st.drop_bad; return; }
        SpSilverWire w; std::memcpy(&w, p, sizeof w);
        if (seat == g_my || w.target != g_my) return;
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (g_sil_off || g_off.snap) { ++g_st.drop_off; return; }
        if (!ready_for(w.key) || !is_member(seat)) { g_early_sil.on = true; g_early_sil.seat = seat; g_early_sil.w = w; return; }
        if (!g_sil_snap_have) { g_sil_snap_have = true; g_sil_snap_sum = w.sum; }
        return;
    }
    ++g_st.drop_bad;
}

bool rate_ok(int seat) {
    const unsigned now = comms_hub_now_ms();
    Rate &r = g_rate[seat];
    if ((unsigned)(now - r.t0) >= kRateWindowMs) { r.t0 = now; r.n = 0; }
    if (r.n >= kRatePerSeat) return false;
    ++r.n;
    return true;
}

// coopgoomboss2: Goomboss summons a goomba every third frame (up to ten in a set), so his minions have their own, wider bucket.
Rate g_rate_minion[kCoopMaxSeats];
bool rate_minion_ok(int seat) {
    const unsigned now = comms_hub_now_ms();
    Rate &r = g_rate_minion[seat];
    if ((unsigned)(now - r.t0) >= kRateWindowMs) { r.t0 = now; r.n = 0; }
    if (r.n >= 40) return false;
    ++r.n;
    return true;
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (!coop_world_active()) return;
    if (seat == g_my || seat < 0 || seat >= kCoopMaxSeats) return;
    const unsigned now = comms_hub_now_ms();
    switch (sub) {
    case kSpWorldSpawn: {
        ++g_st.recv_spawn;
        if (!sp_spawn_validate(p, len)) { ++g_st.drop_bad; return; }
        SpSpawnWire w; std::memcpy(&w, p, sizeof w);
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!is_member(seat)) { ++g_st.drop_member; return; }
        if (g_off.spawn) { ++g_st.drop_off; return; }
        if (net_find(w.origin, w.counter) || ack_has(w.origin, w.counter)) { ++g_st.spawn_dup; return; }
        for (const InEv &e : g_in)
            if (e.on && e.type == kEvSpawn) {
                SpSpawnWire q; std::memcpy(&q, e.buf, sizeof q);
                if (q.origin == w.origin && q.counter == w.counter) { ++g_st.spawn_dup; return; }
            }
        if (!(w.cls == 200 ? rate_minion_ok(seat) : rate_ok(seat))) { ++g_st.drop_rate; return; }
        ++g_st.spawn_queued;
        push_in(kEvSpawn, seat, &w, (int)sizeof w, now);
        return;
    }
    case kSpWorldGone: {
        ++g_st.recv_gone;
        if (!sp_gone_validate(p, len)) { ++g_st.drop_bad; return; }
        SpGoneWire w; std::memcpy(&w, p, sizeof w);
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!is_member(seat)) { ++g_st.drop_member; return; }
        if (g_off.gone) { ++g_st.drop_off; return; }
        push_in(kEvGone, seat, &w, (int)sizeof w, now);
        return;
    }
    case kSpWorldBlock: {
        ++g_st.recv_block;
        if (!sp_block_validate(p, len)) { ++g_st.drop_bad; return; }
        SpBlockWire w; std::memcpy(&w, p, sizeof w);
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!is_member(seat)) { ++g_st.drop_member; return; }
        if (g_off.block) { ++g_st.drop_off; return; }
        uint16_t &last = g_lastbseq[seat][w.slot];
        if (!sp_seq_newer(w.seq, last)) { ++g_st.drop_old; return; }
        last = w.seq;
        push_in(kEvBlock, seat, &w, (int)sizeof w, now);
        return;
    }
    case kSpWorldSilverDrop: {          // SILVERCOOP1
        if (!sp_silver_drop_validate(p, len)) { ++g_st.drop_bad; return; }
        SpSilverDropWire w; std::memcpy(&w, p, sizeof w);
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!is_member(seat)) { ++g_st.drop_member; return; }
        if (g_sil_off) { ++g_st.drop_off; return; }
        uint16_t &last = g_lastsseq[seat];
        if (!sp_seq_newer(w.seq, last)) { ++g_st.drop_old; return; }
        last = w.seq;
        push_in(kEvSilverDrop, seat, &w, (int)sizeof w, now);
        return;
    }
    case kSpWorldSilverCount: {         // COOPSILVER3
        if (!sp_silver_count_validate(p, len)) { ++g_st.drop_bad; return; }
        SpSilverCountWire w; std::memcpy(&w, p, sizeof w);
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!is_member(seat)) { ++g_st.drop_member; return; }
        if (g_sil_off || g_sil_auth_off) { ++g_st.drop_off; return; }
        uint16_t &last = g_lastcseq[seat];
        if ((unsigned)(now - g_lastcseq_t[seat]) > kSilSeqStaleMs) last = 0;    // the owner's build restarted its numbering
        if (!sp_seq_newer(w.seq, last)) { ++g_st.drop_old; return; }
        last = w.seq; g_lastcseq_t[seat] = now;
        ++g_sst.auth_recv;
        const int lead_before = sil_lead();
        g_cnt_heard[seat] = true; g_cnt_heard_t[seat] = now;
        if (seat != sil_lead()) return;              // a higher seat that has not heard me yet: I lead
        if (lead_before == g_my && g_sil_led && (unsigned)(now - g_sil_led_t) <= kSilSeqStaleMs + kSilBeatMs) {   // REVIEW-C (2)
            g_sil_regain = true; g_sil_regain_t = now; g_sil_sent_sum = -1; g_sil_burst = 0;
            if (g_log) std::fprintf(stderr, "[coopsilver] seat %d leads again: lead given back sum=%d wall=%lld\n", seat, (int)w.sum, wall_ms());
        }
        if (g_sil_auth_owner != seat && g_log)
            std::fprintf(stderr, "[coopsilver] following seat %d's count: sum=%d wall=%lld\n", seat, (int)w.sum, wall_ms());
        g_sil_auth_owner = seat; g_sil_auth_have = true; g_sil_auth_sum = w.sum;     // taken in the frame (sil_auth_apply)
        return;
    }
    default:
        ++g_st.drop_bad;
    }
}

// ---------------------------------------------------------------------------
// Sending: blocks, the owner's join answers.
// ---------------------------------------------------------------------------
void send_blocks(unsigned now) {
    for (BlkOut &b : g_bo) {
        if (!b.on || (int)(now - b.next) < 0) continue;
        if (!others_here()) { b.on = false; continue; }
        SpBlockWire w; std::memset(&w, 0, sizeof w);
        w.key = wkey(); w.slot = b.slot; w.cls = b.cls; w.seq = b.seq;
        if (send_world(kSpWorldBlock, &w, sizeof w)) ++g_st.sent_block;
        if (++b.sent >= 3) b.on = false; else b.next = now + kResendMs;
    }
}

void serve_job(Job &j) {
    SpRedWire r; std::memset(&r, 0, sizeof r);
    r.key = wkey(); r.nonce = j.nonce; r.target = (uint8_t)j.seat;
    int s = (int)NumRedCoins(); if (s < 0) s = 0; if (s > 8) s = 8;
    r.sum = (uint8_t)s; r.star = (uint8_t)(g_red_star & 7);
    if (send_ctl(kSpCtlRed, &r, sizeof r)) ++g_st.sent_red;
    if (!g_sil_off) {                   // SILVERCOOP1: the group's silver-star count
        SpSilverWire sv; std::memset(&sv, 0, sizeof sv);
        sv.key = wkey(); sv.nonce = j.nonce; sv.target = (uint8_t)j.seat;
        int ss = sil_total(); if (ss < 0) ss = 0; if (ss > kSilGold) ss = kSilGold;
        sv.sum = (uint8_t)ss;
        if (send_ctl(kSpCtlSilver, &sv, sizeof sv)) ++g_sst.sent_snap;
    }
    int n = 0;
    for (Net &e : g_net)
        if (e.used && !e.gone && e.seen && n < 8) { send_spawn_of(e); ++n; }
    if (g_log && j.sent == 0)
        std::fprintf(stderr, "[coopspawn] serve join to seat %d: red sum=%d live announced=%d wall=%lld\n", j.seat, s, n, wall_ms());
}

int live_class(unsigned cls) {
    int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (a_cls(a) == cls && !a_dead(a) && !is_player_ptr(a)) ++n;
    return n;
}

void stats_line() {
    if (!g_log) return;
    const CoopSpawnStats &s = g_st;
    std::fprintf(stderr,
        "[coopspawnstat] seat=%d armed=%d redsum=%d stars=%d sent spawn%llu gone%llu block%llu red%llu recv spawn%llu gone%llu block%llu red%llu "
        "block local=%llu applied=%llu busy=%llu nf=%llu spawn local=%llu applied=%llu adopted=%llu dup=%llu failed=%llu "
        "gone local=%llu applied=%llu unk=%llu red remote=%llu snap=%llu nativestar=%llu drop bad=%llu group=%llu member=%llu class=%llu old=%llu off=%llu rate=%llu full=%llu\n",
        g_my, (int)g_armed, red_sum(), live_class(178), s.sent_spawn, s.sent_gone, s.sent_block, s.sent_red, s.recv_spawn, s.recv_gone, s.recv_block, s.recv_red,
        s.block_local, s.block_applied, s.block_busy, s.block_notfound, s.spawn_local, s.spawn_applied, s.spawn_adopted, s.spawn_dup, s.spawn_failed,
        s.gone_local, s.gone_applied, s.gone_unknown, s.red_remote, s.red_snap_applied, s.star_spawn_native,
        s.drop_bad, s.drop_group, s.drop_member, s.drop_class, s.drop_old, s.drop_off, s.drop_rate, s.drop_full);
}

// ---------------------------------------------------------------------------
// Proof knobs.
// ---------------------------------------------------------------------------
void knob_block(BlockKnob &k) {
    char *c[64]; int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < 64; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if ((int)a_cls(a) == k.cls && a_slot(a) >= 0 && !a_dead(a) && !is_player_ptr(a)) c[n++] = a;
    std::sort(c, c + n, [](char *x, char *y) { return a_slot(x) < a_slot(y); });
    if (n == 0) { std::fprintf(stderr, "[coopspawn-knob] block cls=%d: no live block\n", k.cls); return; }
    char *a = c[k.nth % n];
    const char *pl = my_player();
    int ch = pl ? (int)(*(const unsigned *)(pl + 8) & 0xff) : 0;
    if (ch > 3) ch = 0;
    bool did = false;
    if (*(const int *)(a + 0x3e8) == 0) {
        *(unsigned char *)(a + 0x3f2) = (unsigned char)ch;
        _ZN18daObjHatenaBlock_c19func_ov102_02149da8Ei(a, 1);               // what QuestionBlock::OnGroundPounded does after its checks
        did = true;
    }
    std::fprintf(stderr, "[coopspawn-knob] block hit cls=%d slot=%d (candidate %d of %d) did=%d wall=%lld\n", k.cls, (int)a_slot(a), k.nth % n, n, (int)did, wall_ms());
}

void knob_red(RedKnob &k) {
    char *c[64]; int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < 64; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (a_cls(a) == 289 && a_slot(a) >= 0 && !a_dead(a) && !is_player_ptr(a)) c[n++] = a;
    std::sort(c, c + n, [](char *x, char *y) { return a_slot(x) < a_slot(y); });
    char *pl = my_player();
    if (!pl || n == 0) { std::fprintf(stderr, "[coopspawn-knob] red %d: no live red coin\n", k.nth); return; }
    char *a = c[k.nth % n];
    // a coin the exact spot did not collect (a summit, inside a wall): the next try drops the player onto it from higher up
    static short last_slot = -2; static int tries = 0;
    if (a_slot(a) == last_slot) ++tries; else { last_slot = a_slot(a); tries = 0; }
    std::memcpy(pl + 0x5c, a + 0x5c, 12);
    if (tries >= 2) {                      // out of reach for the harness (a summit): run the ROM's own pickup handler on it
        _ZN8daCoin_c19func_ov002_020b16c4EP6Player(a, pl);
        std::fprintf(stderr, "[coopspawn-knob] red coin pickup handler slot=%d wall=%lld\n", (int)a_slot(a), wall_ms());
        return;
    }
    if (tries) ((int *)(pl + 0x5c))[1] += (tries * 150) << 12;
    std::fprintf(stderr, "[coopspawn-knob] red coin goto slot=%d (candidate %d of %d) pos=%d,%d,%d wall=%lld\n", (int)a_slot(a), k.nth % n, n,
                 ((const int *)(a + 0x5c))[0] >> 12, ((const int *)(a + 0x5c))[1] >> 12, ((const int *)(a + 0x5c))[2] >> 12, wall_ms());
}

void knob_star(StarKnob &k) {
    char *pl = my_player();
    if (!pl) return;
    const int *p = (const int *)(pl + 0x5c);
    const int pos[3] = {p[0] + (160 << 12), p[1] + (80 << 12), p[2]};
    char *a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as((unsigned)k.cls, k.param, pos, nullptr, a_area(pl), -1);
    std::fprintf(stderr, "[coopspawn-knob] spawn cls=%d param=%u -> %p uid=%u pos=%d,%d,%d wall=%lld\n", k.cls, k.param, (void *)a, a ? a_uid(a) : 0u,
                 pos[0] >> 12, pos[1] >> 12, pos[2] >> 12, wall_ms());
}

void knob_take(TakeKnob &) {
    char *pl = my_player();
    if (!pl) return;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned c = a_cls(a);
        if ((c != 178 && c != 282) || a_slot(a) != -1 || a_dead(a) || is_player_ptr(a)) continue;
        std::memcpy(pl + 0x5c, a + 0x5c, 12);
        if (c == 178) {                      // the same two words the harness probe writes: the star's own collect handler runs
            *(unsigned *)(a + 0x134) = *(const unsigned *)(pl + 4);
            *(unsigned *)(a + 0x130) |= 0x400000;
        }
        if (c == 282) _ZN7fBase_c18MarkForDestructionEv(a);      // a key is destroyed when its pickup animation ends; the harness stands in for the animation
        std::fprintf(stderr, "[coopspawn-knob] take goto cls=%u state=%d pos=%d,%d,%d wall=%lld\n", c, *(const int *)(a + 0x440), ((const int *)(a + 0x5c))[0] >> 12,
                     ((const int *)(a + 0x5c))[1] >> 12, ((const int *)(a + 0x5c))[2] >> 12, wall_ms());
        return;
    }
    std::fprintf(stderr, "[coopspawn-knob] take: no runtime star or key\n");
}

void knob_forge(const char *name) {
    std::fprintf(stderr, "[coopspawn-knob] forge %s\n", name);
    unsigned char b[128]; std::memset(b, 0, sizeof b);
    SpSpawnWire s; std::memset(&s, 0, sizeof s);
    s.key = wkey(); s.origin = (uint8_t)g_my; s.kind = kSpKindActor; s.counter = 9000 % 60000; s.cls = 178; s.area = 0; s.param = 0;
    s.pos[0] = 0; s.pos[1] = 0; s.pos[2] = 0;
    if (!std::strcmp(name, "cls")) { s.cls = 207; send_world(kSpWorldSpawn, &s, sizeof s); }              // a Cannon: not on the list
    else if (!std::strcmp(name, "cls2")) { s.cls = 200; send_world(kSpWorldSpawn, &s, sizeof s); }       // a Goomba
    else if (!std::strcmp(name, "kind")) { s.kind = 3; send_world(kSpWorldSpawn, &s, sizeof s); }
    else if (!std::strcmp(name, "param")) { s.param = 0x12345678u; send_world(kSpWorldSpawn, &s, sizeof s); }
    else if (!std::strcmp(name, "counter")) { s.counter = 0; send_world(kSpWorldSpawn, &s, sizeof s); }
    else if (!std::strcmp(name, "pos")) { s.pos[1] = 0x7ffffff0; send_world(kSpWorldSpawn, &s, sizeof s); }
    else if (!std::strcmp(name, "origin")) { s.origin = 77; send_world(kSpWorldSpawn, &s, sizeof s); }
    else if (!std::strcmp(name, "resv")) { s.reserved = 1; send_world(kSpWorldSpawn, &s, sizeof s); }
    else if (!std::strcmp(name, "short")) { send_world(kSpWorldSpawn, b, 9); }
    else if (!std::strcmp(name, "long")) { send_world(kSpWorldSpawn, b, 100); }
    else if (!std::strcmp(name, "key")) { s.key.act = (uint8_t)((g_act + 1) & 7); send_world(kSpWorldSpawn, &s, sizeof s); }
    else if (!std::strcmp(name, "badkey")) { s.cls = 282; s.counter = 9001; send_world(kSpWorldSpawn, &s, sizeof s); }       // a key in a course that has no key model
    else if (!std::strcmp(name, "star9")) { s.param = 9; send_world(kSpWorldSpawn, &s, sizeof s); }                        // star number 9: not a star
    else if (!std::strcmp(name, "star7")) { s.param = 7; s.counter = 9002; send_world(kSpWorldSpawn, &s, sizeof s); }      // star number 7: valid
    else if (!std::strcmp(name, "grand")) { s.cls = 283; s.param = 7; s.counter = 9003; send_world(kSpWorldSpawn, &s, sizeof s); }   // the Grand Star: level 40 only
    else if (!std::strcmp(name, "key2")) { s.cls = 282; s.param = 2; s.counter = 9004; send_world(kSpWorldSpawn, &s, sizeof s); }    // Goomboss's key: level 45 only
    else if (!std::strcmp(name, "mips")) { s.cls = 229; s.param = 0; s.counter = 9006; send_world(kSpWorldSpawn, &s, sizeof s); }      // MIPS's object (item 12): announced like a key
    else if (!std::strcmp(name, "key3")) { s.cls = 282; s.param = 3; s.counter = 9005; send_world(kSpWorldSpawn, &s, sizeof s); }    // Big Boo's key: level 47 only
    else if (!std::strcmp(name, "goodstar")) { send_world(kSpWorldSpawn, &s, sizeof s); }                 // valid: the control that the forge shape is right
    else if (!std::strcmp(name, "gone")) {
        SpGoneWire g; std::memset(&g, 0, sizeof g); g.key = wkey(); g.origin = 99; g.kind = kSpGoneAnn; g.id = 5;
        send_world(kSpWorldGone, &g, sizeof g);
    } else if (!std::strcmp(name, "goneslot")) {
        SpGoneWire g; std::memset(&g, 0, sizeof g); g.key = wkey(); g.origin = 0xFF; g.kind = kSpGoneLinked; g.id = 700;
        send_world(kSpWorldGone, &g, sizeof g);
    } else if (!std::strcmp(name, "blockslot")) {
        SpBlockWire w; std::memset(&w, 0, sizeof w); w.key = wkey(); w.slot = 600; w.cls = 21; w.seq = 1;
        send_world(kSpWorldBlock, &w, sizeof w);
    } else if (!std::strcmp(name, "blockcls")) {
        SpBlockWire w; std::memset(&w, 0, sizeof w); w.key = wkey(); w.slot = 3; w.cls = 207; w.seq = 1;
        send_world(kSpWorldBlock, &w, sizeof w);
    } else if (!std::strcmp(name, "redsum")) {
        SpRedWire r; std::memset(&r, 0, sizeof r); r.key = wkey(); r.nonce = 1; r.target = (uint8_t)(g_my ^ 1); r.sum = 9; r.star = 1;
        send_ctl(kSpCtlRed, &r, sizeof r);
    }
}

// ---------------------------------------------------------------------------
// Level lifecycle and the frame.
// ---------------------------------------------------------------------------
void new_level() {
    EarlyRed keep = g_early;
    if (keep.on && !(keep.w.key.level == g_lvl && keep.w.key.act == g_act)) keep.on = false;
    std::memset(g_blk, 0, sizeof g_blk);
    std::memset(g_bo, 0, sizeof g_bo);
    std::memset(g_lastbseq, 0, sizeof g_lastbseq);
    std::memset(g_net, 0, sizeof g_net);
    std::memset(g_lnk, 0, sizeof g_lnk);
    std::memset(g_lnk_gone, 0, sizeof g_lnk_gone);
    std::memset(g_star_taken, 0, sizeof g_star_taken);      // SNOWMAN2: per visit
    std::memset(g_ack, 0, sizeof g_ack);
    std::memset(g_in, 0, sizeof g_in);
    std::memset(g_job, 0, sizeof g_job);
    std::memset(g_rate, 0, sizeof g_rate);
    for (int i = 0; i < 4; ++i) red_cell(i) = 0;     // the port's level clear misses the data_0209f30d half
    g_nred = 0; g_red_prev = 0; g_red_armed = false; g_red_snap_have = false; g_red_snap_sum = 0; g_red_star = 0;
    g_frames = 0; g_armed = false; g_census_on = false; g_wm = 0; g_census_lines = 0;
    for (int i = 0; i < g_nkb; ++i) g_kb[i].done = false;
    for (int i = 0; i < g_nkr; ++i) g_kr[i].done = false;
    for (int i = 0; i < g_nks; ++i) g_ks[i].done = false;
    for (int i = 0; i < g_nkt; ++i) g_kt[i].done = false;
    for (int i = 0; i < g_nforge; ++i) g_forge[i].done = false;
    g_early = keep;
    EarlySil keep_sil = g_early_sil;
    if (keep_sil.on && !(keep_sil.w.key.level == g_lvl && keep_sil.w.key.act == g_act)) keep_sil.on = false;
    g_nsil = 0; g_sil_last = -1; g_sil_wm = 0; g_sil_snap_have = false; g_sil_snap_done = false; g_sil_snap_sum = 0;
    std::memset(g_so, 0, sizeof g_so);
    std::memset(g_lastsseq, 0, sizeof g_lastsseq);
    std::memset(g_lastcseq, 0, sizeof g_lastcseq); std::memset(g_lastcseq_t, 0, sizeof g_lastcseq_t);
    g_sil_changed = false; g_sil_sent_sum = -1; g_sil_burst = 0; g_sil_beat_t = 0;
    g_sil_auth_have = false; g_sil_auth_owner = -1;
    std::memset(g_cnt_heard, 0, sizeof g_cnt_heard); std::memset(g_cnt_heard_t, 0, sizeof g_cnt_heard_t);
    g_sil_synced = false; g_sil_alone_frames = 0; g_sil_led = false; g_sil_regain = false; g_sil_regain_t = 0;
    g_sil_drop_done = g_sil_drop_frame == 0;
    g_sil_pit_done = g_sil_pit_frame == 0;
    g_sil_brick_done = g_sil_brick_frame == 0;
    g_sil_skew_done = g_sil_skew_frame == 0;
    g_sil_goomba_done = g_sil_goomba_frame == 0;
    g_early_sil = keep_sil;
    std::memset(g_swp, 0, sizeof g_swp); g_nswp = 0;
    for (SwStar &s : g_sws) s = SwStar();
    g_nsws = 0;
    std::memset(g_sw_gone, 0, sizeof g_sw_gone);
    std::memset(g_swsend, 0, sizeof g_swsend);
    for (int i = 0; i < g_nswtake; ++i) g_swtake[i].done = false;
    g_swtake_until = 0;
    g_sw_post = 0; g_sw_post_sw = nullptr;
}

void on_latched(int level, int act) {
    if (!coop_world_active()) return;
    read_env();
    g_my = comms_hub_my_seat();
    g_lvl = level; g_act = act; g_have_key = true;
    ++g_gen;
    new_level();
    if (g_log) std::fprintf(stderr, "[coopspawn] latched level=%d act=%d wall=%lld\n", level, act, wall_ms());
}

void on_established(int level, int act) {
    if (!coop_world_active()) return;
    g_lvl = level; g_act = act; g_have_key = true;
}

void on_snapshot_request(int seat, unsigned nonce) {
    if (!coop_world_active() || seat < 0 || seat >= kCoopMaxSeats || seat == g_my) return;
    Job &j = g_job[seat];
    j.on = true; j.seat = seat; j.nonce = nonce; j.sent = 0; j.next = 0; j.born = comms_hub_now_ms();
}

void on_member_gone(int seat) {
    if (seat >= 0 && seat < kCoopMaxSeats) {
        std::memset(g_lastbseq[seat], 0, sizeof g_lastbseq[seat]);
        g_lastsseq[seat] = 0;
        g_job[seat].on = false;
    }
}

void register_handlers() {
    if (g_registered) return;
    g_registered = true;
    const bool a = coop_ctl_register(kSpCtlRed, kSpCtlLast, on_ctl, nullptr);
    const bool b = coop_world_register(kSpWorldSpawn, kSpWorldLast, on_world, nullptr);
    std::fprintf(stderr, "[coopspawn] spawn module on: ctl %s, world %s\n", a ? "ok" : "REFUSED", b ? "ok" : "REFUSED");
}

void on_frame(unsigned now) {
    read_env();
    if (!coop_world_active()) return;
    register_handlers();
    g_my = comms_hub_my_seat();
    int lv = -1, ac = 0;
    if (!coop_group_key(&lv, &ac) || !coop_group_established()) return;
    if (!g_have_key || lv != g_lvl || ac != g_act) {        // a level change that missed the latch
        g_lvl = lv; g_act = ac; g_have_key = true; ++g_gen; new_level();
    }
    if (!my_player()) return;
    ++g_frames;

    // SMALLS9B proof knob: SM64DS_SMALLS9B_KEYBIT=<frame>:<mask> ORs the mask into my live save copy's flags1 at that frame (the bit a rabbit
    // catch sets: 0x40 / 0x80, a monotonic bit of the shared record); while the knob is set the live flags1 is logged every 60 frames.
    if (const char *kb = std::getenv("SM64DS_SMALLS9B_KEYBIT")) {
        unsigned f = 0, m = 0;
        if (std::sscanf(kb, "%u:%i", &f, (int *)&m) == 2) {
            unsigned *fl = (unsigned *)(data_0209caa0 + 4);
            if (g_frames == f) { *fl |= m; if (g_log) std::fprintf(stderr, "[coopspawn] keybit set mask=%08x frame=%u wall=%lld\n", m, g_frames, wall_ms()); }
            if (g_log && (g_frames % 60u) == 0) std::fprintf(stderr, "[coopspawn] keybit flags1=%08x frame=%u wall=%lld\n", *fl, g_frames, wall_ms());
        }
    }

    // SMALLS9B proof pictures: SM64DS_SMALLS9B_SHOT="f1,f2" writes smalls9b_f<N>.bmp (the top screen) when this module's frame count reaches N.
    if (const char *sh = std::getenv("SM64DS_SMALLS9B_SHOT")) {
        for (const char *q = sh; *q;) {
            char *end = nullptr;
            const long n = std::strtol(q, &end, 10);
            if (end == q) break;
            if (g_frames == (unsigned)n) {
                char nm[64]; std::snprintf(nm, sizeof nm, "smalls9b_f%ld.bmp", n);
                const void *lfb = ::port_level_fb_ptr;
                const bool w = lfb && ::ntr::ppu_write_bmp(nm, *(const ::ntr::Framebuffer *)lfb);
                if (g_log) std::fprintf(stderr, "[coopspawn] SMALLS9B picture %s %s pstate=%08x wall=%lld\n", nm, w ? "written" : "FAILED",
                                        my_player() ? *(const unsigned *)(my_player() + 0x370) : 0u, wall_ms());
            }
            q = end; while (*q == ',' || *q == ' ') ++q;
        }
    }

    // the owner's red-coin sum, from a sender that was not yet a member
    if (g_early.on && is_member(g_early.seat)) {
        g_early.on = false;
        if (g_early.w.key.level == g_lvl && g_early.w.key.act == g_act) apply_red_snap(g_early.w);
    }

    // SILVERCOOP1: the owner's silver count, from a sender that was not yet a member
    if (g_early_sil.on && is_member(g_early_sil.seat)) {
        g_early_sil.on = false;
        if (g_early_sil.w.key.level == g_lvl && g_early_sil.w.key.act == g_act && !g_sil_snap_have) { g_sil_snap_have = true; g_sil_snap_sum = g_early_sil.w.sum; }
    }

    // arm: the level has settled and a joiner has been told (or nobody could tell it)
    if (!g_armed && g_frames >= kSettleFrames &&
        (g_red_snap_have || coop_group_i_am_owner() || g_frames >= kRedWait)) {
        g_armed = true;
        if (g_red_snap_have) {
            const int have = red_sum();
            if (g_red_snap_sum > have) red_bump(g_red_snap_sum - have, g_red_star, "snapshot");
        }
        red_rebase();
        g_red_armed = true;
        if (!g_sil_off) sil_arm();
        if (g_log) std::fprintf(stderr, "[coopspawn] armed frame=%u redsum=%d snap=%d wall=%lld\n", g_frames, red_sum(), g_red_snap_have ? g_red_snap_sum : -1, wall_ms());
    }
    if (!g_census_on && g_frames >= kSettleFrames) {         // everything the level placed or started with is not "new"
        g_census_on = true;
        unsigned m = 0;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) if (a_uid(a) > m) m = a_uid(a);
        g_wm = m;
    }

    if (g_frames >= 3) scan(now);
    if (g_frames >= 3) sw_tick(now);
    if (g_armed) { sil_scan(); apply_silver_snap(); }
    drain_in(now);
    sil_auth_apply(now);
    if (g_sil_goto_wall > 0 && g_armed && my_player() && wall_ms() >= g_sil_goto_wall) {    // proof knob: one star, one moment, every console
        char *best = nullptr;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
            if (a_cls(a) == 179 && !a_dead(a) && !is_player_ptr(a) && a_slot(a) >= 0 && (!best || a_slot(a) < a_slot(best))) best = a;
        g_sil_goto_wall = 0;
        if (best) std::memcpy(my_player() + 0x5c, best + 0x5c, 12);
        std::fprintf(stderr, "[coopsilver-knob] goto wall: slot=%d frame=%u wall=%lld\n", best ? (int)a_slot(best) : -1, g_frames, wall_ms());
    }
    if (!g_sil_drop_done && g_armed && g_frames >= g_sil_drop_frame && my_player()) {       // proof knob: the ROM's hurt drop
        g_sil_drop_done = true;
        unsigned char keep[16]; char *mk[16]; int nk = 0;
        if (g_sil_drop_nofree)       // state 0 is never free in daStar_c::func_ov002_020e7554
            for (char *m = _ZN8dActor_c15FindWithActorIDEjPS_(0xb4, nullptr); m && nk < 16; m = _ZN8dActor_c15FindWithActorIDEjPS_(0xb4, m)) {
                mk[nk] = m; keep[nk++] = *(unsigned char *)(m + 0x1d8); *(unsigned char *)(m + 0x1d8) = 0;
            }
        if (nk) std::fprintf(stderr, "[coopsilver-knob] nofree: %d markers held not free for the drop wall=%lld\n", nk, wall_ms());
        const int r = _ZN6Player19func_ov002_020d94ccEv(my_player());
        for (int i = 0; i < nk; ++i) *(unsigned char *)(mk[i] + 0x1d8) = keep[i];
        std::fprintf(stderr, "[coopsilver-knob] hurt drop on my player: %d (mine=%d) wall=%lld\n", r, sil_me() >= 0 ? (int)data_0209f310[sil_me()] : -1, wall_ms());
    }
    if (!g_sil_skew_done && g_armed && g_frames >= g_sil_skew_frame && sil_me() >= 0) {   // COOPSILVER3 proof knob: a silent drift
        g_sil_skew_done = true;
        const int was = (int)data_0209f310[sil_me()];
        sil_write(was + g_sil_skew);
        std::fprintf(stderr, "[coopsilver-knob] skew: mine %d -> %d sum=%d, nothing said frame=%u wall=%lld\n", was, (int)data_0209f310[sil_me()], sil_total(), g_frames, wall_ms());
    }
    if (!g_sil_brick_done && g_armed && g_frames >= g_sil_brick_frame) {     // COOPSILVER3 proof knob: a silver star brick block breaks here
        g_sil_brick_done = true;
        char *blk = nullptr;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && !blk; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            const unsigned c = a_cls(a);
            if ((c != 15 && c != 16 && c != 17) || a_dead(a)) continue;
            const char *tag = *(const char *const *)(a + 0x328);
            if (tag && a_cls(tag) == 0x144) blk = a;
        }
        if (blk) _ZN13daObjBlockL_c4KillEv(blk);
        std::fprintf(stderr, "[coopsilver-knob] brick: %s slot=%d frame=%u wall=%lld\n", blk ? "broke the silver star block" : "no block with a silver star tag",
                     blk ? (int)a_slot(blk) : -1, g_frames, wall_ms());
    }
    if (!g_sil_goomba_done && g_armed && g_frames >= g_sil_goomba_frame) {   // COOPSILVER3 proof knob: a silver star Goomba is beaten here
        g_sil_goomba_done = true;
        char *g = nullptr;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && !g; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            const unsigned c = a_cls(a);
            if (c < 200 || c > 202 || a_dead(a)) continue;
            if (*(const signed char *)(a + 0x465) >= 0) g = a;
        }
        if (g) { func_ov084_021296cc(g); func_ov084_02129498(g); }
        std::fprintf(stderr, "[coopsilver-knob] goomba: %s slot=%d frame=%u wall=%lld\n", g ? "beat the silver star Goomba" : "no Goomba holding a glint",
                     g ? (int)a_slot(g) : -1, g_frames, wall_ms());
    }
    if (!g_sil_pit_done && g_armed && g_frames >= g_sil_pit_frame) {        // proof knob: a dropped star falls out of the level
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (a_cls(a) != 179 || a_dead(a) || is_player_ptr(a) || a_slot(a) != -1 || *(const unsigned *)(a + 0x440) != 8u) continue;
            // STATE_BOUNCE runs the bounds test (daStar_c::func_ov002_020e8c34) every frame; above 80000.0 it calls
            // func_ov002_020e8abc, which destroys a star without a marker: the cartridge's "fell off the level".
            if (g_sil_pit_nomarker) *(unsigned *)(a + 0x434) = 0;      // mMarkerID
            *(int *)(a + 0x60) = 0x14000000;
            g_sil_pit_done = true;
            std::fprintf(stderr, "[coopsilver-knob] pit: dropped star uid=%u lifted out of bounds%s frame=%u wall=%lld\n", a_uid(a), g_sil_pit_nomarker ? " (marker forgotten)" : "", g_frames, wall_ms());
            break;
        }
    }

    // proof knobs
    if (!g_near_done && g_armed && g_frames >= g_near_frame) {
        char *pl = my_player();
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); pl && a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (a_cls(a) != 229 || a_dead(a)) continue;
            std::memcpy(pl + 0x5c, a + 0x5c, 12);
            g_near_done = true;
            std::fprintf(stderr, "[coopspawn-knob] near: my player put on the rabbit key at %d,%d,%d wall=%lld\n", ((const int *)(a + 0x5c))[0] >> 12,
                         ((const int *)(a + 0x5c))[1] >> 12, ((const int *)(a + 0x5c))[2] >> 12, wall_ms());
            break;
        }
    }
    if (g_tc_val >= 0 && g_armed && g_frames >= g_tc_frame) {
        data_0209f358[(int)data_0209f250 & 3] = (short)g_tc_val;
        std::fprintf(stderr, "[coopspawn-knob] coin counter set to %d wall=%lld\n", g_tc_val, wall_ms());
        g_tc_val = -1;
    }
    for (int i = 0; i < g_nkb; ++i)
        if (!g_kb[i].done && g_armed && g_frames >= g_kb[i].frame) { g_kb[i].done = true; knob_block(g_kb[i]); }
    for (int i = 0; i < g_nkr; ++i)
        if (!g_kr[i].done && g_armed && g_frames >= g_kr[i].frame) { g_kr[i].done = true; knob_red(g_kr[i]); }
    for (int i = 0; i < g_nks; ++i)
        if (!g_ks[i].done && g_armed && g_frames >= g_ks[i].frame) { g_ks[i].done = true; knob_star(g_ks[i]); }
    for (int i = 0; i < g_nkt; ++i)
        if (!g_kt[i].done && g_armed && g_frames >= g_kt[i].frame) { g_kt[i].done = true; knob_take(g_kt[i]); }
    for (int i = 0; i < g_nforge; ++i)
        if (!g_forge[i].done && g_armed && g_frames >= g_forge[i].frame) { g_forge[i].done = true; knob_forge(g_forge[i].name); }

    // outgoing
    send_blocks(now);
    send_net(now);
    sil_send_drops(now);
    sil_auth_send(now);
    if (coop_group_i_am_owner())
        for (Job &j : g_job) {
            if (!j.on || (int)(now - j.next) < 0) continue;
            if (!is_member(j.seat)) { if ((unsigned)(now - j.born) > 5000u) j.on = false; continue; }
            serve_job(j);
            ++j.sent;
            if (j.sent >= 3) j.on = false; else j.next = now + (j.sent == 1 ? kJobMs : 400u);
        }
    if (g_log && (g_frames % 120u) == 0) {
        stats_line();
        const int me = sil_me();
        std::fprintf(stderr, "[coopsilver] stat mine=%d sum=%d tracked=%d credit take=%llu ann=%llu snap=%llu capped=%llu drops sent=%llu applied=%llu local=%llu ghost=%llu snap sent=%llu recv=%llu fell=%llu fell take=%llu frame=%u\n",
                     me >= 0 ? (int)data_0209f310[me] : -1, sil_total(), g_nsil, g_sst.credit_take, g_sst.credit_ann, g_sst.credit_snap, g_sst.capped,
                     g_sst.drop_sent, g_sst.drop_applied, g_sst.drop_local, g_sst.ghost_take, g_sst.sent_snap, g_sst.recv_snap,
                     g_sst.fell, g_sst.fell_take, g_frames);
        {
            char gl[160]; int o = 0, n = 0;
            for (int m = 0; m < 12; ++m)
                if (const char *g = (const char *)STAR_MARKERS[m]) {
                    ++n;
                    if (o < 140) o += std::snprintf(gl + o, sizeof gl - (size_t)o, " %d:%u/%d", m, a_cls(g), (int)a_slot(g));
                }
            gl[o < (int)sizeof gl ? o : (int)sizeof gl - 1] = 0;
            std::fprintf(stderr, "[coopsilver] glints %d:%s frame=%u\n", n, gl, g_frames);
        }
        std::fprintf(stderr, "[coopsilver] auth synced=%d lead=%d owner=%d iam=%d follow=%d said=%llu heard=%llu adopted=%llu late=%llu left=%llu\n",
                     (int)g_sil_synced, sil_lead(), coop_group_owner(), (int)coop_group_i_am_owner(), (int)sil_follow(), g_sst.auth_sent, g_sst.auth_recv, g_sst.auth_adopt,
                     g_sst.auth_late, g_sst.auth_left);
        std::fprintf(stderr, "[coopswstar] stat tracked=%d switches=%d camera taken back=%llu timer held=%llu see-through gone=%llu bit=%llu sent=%llu recv=%llu "
                     "timed out=%llu cases=%llu removed=%llu frame=%u\n", g_nsws, g_nswp, g_swst.sup, g_swst.held, g_swst.look_gone, g_swst.look_bit,
                     g_swst.sent, g_swst.recv, g_swst.expired, g_swst.cases, g_swst.removed, g_frames);
    }
}

struct Reg {
    Reg() {
        coop_on_frame(on_frame);
        coop_on_group_latched(on_latched);
        coop_on_group_established(on_established);
        coop_on_snapshot_request(on_snapshot_request);
        coop_on_member_gone(on_member_gone);
    }
} g_reg;

}  // namespace

CoopSpawnStats coop_spawn_stats() { return g_st; }

bool coop_spawn_announce(void *local_actor, int cls, unsigned param, const int pos[3], int area) {
    if (!coop_world_active() || !coop_group_established() || !g_have_key || g_my < 0) return false;
    if (!is_ann_cls((unsigned)cls) || !pos || !sp_param_ok(cls, param) || area < -1 || area > 7) return false;
    for (int i = 0; i < 3; ++i) if (!sp_pos_ok(pos[i])) return false;
    if (local_actor) {
        char *a = (char *)local_actor;
        for (const Net &n : g_net) if (n.used && n.a == a && n.uid == a_uid(a)) return true;     // already announced
    }
    return register_local((char *)local_actor, cls, param, pos, area);
}

void *coop_spawn_and_announce(int cls, unsigned param, const int pos[3], int area) {
    if (!pos) return nullptr;
    char *a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as((unsigned)cls, param, pos, nullptr, area, -1);
    if (!a) return nullptr;
    if (cls == 179) {
        // a Silver Star needs its marker, the way the brick block makes the pair
        char *m = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb4, 0x50, pos, nullptr, area, -1);
        if (m) { *(unsigned *)(a + 0x434) = a_uid(m); _ZN12daStarBase_c27LinkSilverStarAndStarMarkerEPc(m, a); }
    }
    coop_spawn_announce(a, cls, param, pos, area);
    return a;
}

void coop_spawn_note_remote_press(void *switch_actor) {
    if (!switch_actor || !coop_world_active()) return;
    char *a = (char *)switch_actor;
    // a switch first seen here was idle before this press (the press is only applied to an idle switch)
    SwPress *p = swp_for(a, true, 0);
    if (!p) return;
    p->note = true;
    if (g_log) std::fprintf(stderr, "[coopswstar] a friend's press reached switch slot=%d frame=%u wall=%lld\n", (int)a_slot(a), g_frames, wall_ms());
}

}  // namespace port
