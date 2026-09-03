/* ---- SM64DS_VS_LUIGI_INFECTION: the Luigi Infection VS mode, game side ------
 *
 * A host-layer VS mode built ON TOP of the char-apply seam (SM64DS_VS_CHARS,
 * hal/player_bridges.cpp). It seats NO new behaviour into a normal VS match:
 * with the variable unset it is a compile-time-present, run-time-inert module
 * and every existing VS proof, capture and battery run is byte-for-byte what it
 * was. The mode arms ONLY when SM64DS_VS_LUIGI_INFECTION is set.
 *
 * THE MODE.  One player starts as Luigi -- the tagger -- HIDDEN from the
 * minimap while every survivor still shows. Luigi tags survivors with the moves
 * that normally drop a star (punch, slide-kick, ground-pound); a tagged
 * survivor CONVERTS to Luigi and joins his team. Timed match: the Luigis win if
 * everyone is infected before the clock runs out, the survivors win if any of
 * them are still alive when it does. Survivor-vs-survivor combat is untouched;
 * Luigi is immune to being hit, knocked or eaten and can only tag.
 *
 * TEAM MEMBERSHIP IS TRACKED HERE, NOT INFERRED FROM THE CHARACTER.  g_li_team
 * is the single source of truth: the seed sets one bit, a tag sets another, and
 * every consumer (the minimap hide, the win poll, the hit override) reads this
 * array. The Luigi BODY is a consequence of being on the team -- li_infect
 * swaps it through the game's own door path -- but the logic never keys off
 * mCharacter, so a survivor who was assigned Luigi by a mis-configured picker is
 * still a survivor and a tag is unambiguous.
 *
 * THE ENV GRAMMAR (mirrors the other SM64DS_VS_* toggles):
 *   SM64DS_VS_LUIGI_INFECTION  unset / empty / "0"  -> OFF (the common case,
 *                              silent, byte-identical normal VS).
 *                              any other non-empty value -> ON.
 *   SM64DS_VS_LUIGI_SEED       0..15, which slot starts as the tagger. Default
 *                              0. See the LOCKSTEP note below -- the HOST picks
 *                              the random tagger and passes it here, so every
 *                              console seeds the SAME slot. A local rand() would
 *                              desync a real session, so there is none.
 *   SM64DS_VS_LUIGI_TIME       optional test override, seconds. 0 / unset ->
 *                              use the ROM's own match clock (the default and
 *                              the faithful one). A positive value ends the
 *                              match after that many seconds, so a proof does
 *                              not have to wait out a real timer.
 *
 * LOCKSTEP.  The seed is deterministic by construction: it is read from the
 * environment the host built, not rolled locally, and the tag/immunity/win
 * decisions are pure functions of shared state (data_0209f394, the team array
 * every console updates the same way on the same frame). Nothing here asks the
 * other console anything. The lobby's job is to choose one random slot and put
 * it in SM64DS_VS_LUIGI_SEED for every peer; that is the online wiring this
 * game side is waiting on, the same shape as SM64DS_VS_CHARS / _NAMES.
 */

#include <cstdlib>
#include <cstdio>
#include "vs_width.h"

extern "C" {
extern unsigned char data_0209f2d8;        /* game mode: 0 adventure, 1 VS */
extern void         *data_0209f394[];      /* per-slot Player* (null = no body) */
extern unsigned char data_0209f250;        /* local player index */
extern void port_player_set_character(void *player, unsigned ch);
}

/* the minimap's per-player mapID band, sixteen wide, DEFINED in
 * unmatched/Minimap_Behavior.cpp (which inherits C linkage from the extern "C"
 * block in Minimap_wide.h); -1 in a slot means "draw no blip for it". Declared
 * extern "C" here so the symbol name matches that definition, not a C++ mangle. */
extern "C" signed char g_mm_mapID[kPortMaxPlayers];

/* engine character index: Mario 0, Luigi 1, Wario 2, Yoshi 3. */
enum { kLuigiChar = 1 };

static int g_li_read;                     /* env parsed once */
static int g_li_on;                       /* mode enabled */
static int g_li_seed_slot;                /* the initial tagger's slot */
static int g_li_time_frames;              /* >0 = test timer, in frames */
static int g_li_seeded;                   /* the seed has run this match */
static int g_li_team[kPortMaxPlayers];    /* 1 = on Luigi's team (infected) */

static void li_load(void)
{
    if (g_li_read) return;
    g_li_read = 1;
    g_li_on = 0;
    g_li_seed_slot = 0;
    g_li_time_frames = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i) g_li_team[i] = 0;

    const char *e = std::getenv("SM64DS_VS_LUIGI_INFECTION");
    if (!e || e[0] == 0 || (e[0] == '0' && e[1] == 0))
        return;                            /* absent / "0": OFF, and silent */
    g_li_on = 1;

    if (const char *s = std::getenv("SM64DS_VS_LUIGI_SEED")) {
        int v = std::atoi(s);
        if (v >= 0 && v < kPortMaxPlayers) g_li_seed_slot = v;
        else
            std::fprintf(stderr, "[luigi] SM64DS_VS_LUIGI_SEED %d out of "
                    "0..%d, using slot 0\n", v, kPortMaxPlayers - 1);
    }
    if (const char *t = std::getenv("SM64DS_VS_LUIGI_TIME")) {
        int secs = std::atoi(t);
        if (secs > 0) g_li_time_frames = secs * 60;
    }
    std::fprintf(stderr, "[luigi] Luigi Infection ARMED: tagger seed = slot %d, "
            "timer = %s\n", g_li_seed_slot,
            g_li_time_frames ? "override" : "ROM match clock");
}

extern "C" int port_luigi_enabled(void)
{
    li_load();
    return g_li_on;
}

/* the test timer, in frames, or 0 to use the ROM's own match clock. */
extern "C" int port_luigi_time_frames(void)
{
    li_load();
    return g_li_time_frames;
}

/* the slot a Player* sits in, or -1. The per-slot table is the port's own and
 * the mapping is exact, so a body that is in the match is found and a stale or
 * foreign pointer answers -1. */
static int li_slot_of(const void *p)
{
    if (!p) return -1;
    for (int i = 0; i < kPortMaxPlayers; ++i)
        if (data_0209f394[i] == p) return i;
    return -1;
}

extern "C" int port_luigi_is_infected_slot(int slot)
{
    if (!g_li_on || slot < 0 || slot >= kPortMaxPlayers) return 0;
    return g_li_team[slot];
}

extern "C" int port_luigi_is_infected_player(const void *p)
{
    if (!g_li_on) return 0;
    const int s = li_slot_of(p);
    return (s >= 0) ? g_li_team[s] : 0;
}

extern "C" int port_luigi_seeded(void) { return g_li_seeded; }

/* survivors still alive: slots that have a body and are NOT on Luigi's team.
 * -1 when the mode is off, so a caller can tell "no survivors" (0) from "not
 * this mode" (-1). */
extern "C" int port_luigi_survivors_alive(void)
{
    if (!g_li_on) return -1;
    int n = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i)
        if (data_0209f394[i] && !g_li_team[i]) ++n;
    return n;
}

/* how many bodies are in this match at all (team or not). */
extern "C" int port_luigi_match_slots(void)
{
    int n = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i)
        if (data_0209f394[i]) ++n;
    return n;
}

/* mark a slot infected and swap its body to Luigi through the game's own door
 * path -- idempotent, so a double tag on one frame is one infection. */
static void li_infect(int slot)
{
    if (slot < 0 || slot >= kPortMaxPlayers) return;
    if (g_li_team[slot]) return;
    g_li_team[slot] = 1;
    void *p = data_0209f394[slot];
    if (p) port_player_set_character(p, (unsigned)kLuigiChar);
}

/* TAG: a survivor victim converts to Luigi. Called from the hit override once
 * it has established the attacker is on Luigi's team and the victim is not. */
extern "C" void port_luigi_tag_player(const void *victim)
{
    if (!g_li_on) return;
    const int s = li_slot_of(victim);
    if (s < 0 || g_li_team[s]) return;
    std::fprintf(stderr, "[luigi] TAG: slot %d converted to Luigi "
            "(survivors left = %d)\n", s, port_luigi_survivors_alive() - 1);
    li_infect(s);
}

/* SEED: at frame 90 (the char-apply timing, past the level-entry no-control),
 * turn exactly one slot into the starting Luigi. VS only, once per match. */
extern "C" void port_luigi_seed(int frame)
{
    if (data_0209f2d8 != 1) return;        /* VS mode only */
    li_load();
    if (!g_li_on || g_li_seeded || frame != 90) return;
    g_li_seeded = 1;

    int slot = g_li_seed_slot;
    if (slot < 0 || slot >= kPortMaxPlayers || !data_0209f394[slot]) {
        /* the chosen seat has no body in this arena: fall to the first live
           slot so the mode always has exactly one tagger */
        slot = -1;
        for (int i = 0; i < kPortMaxPlayers; ++i)
            if (data_0209f394[i]) { slot = i; break; }
        if (slot >= 0)
            std::fprintf(stderr, "[luigi] seed slot %d had no body; the tagger "
                    "falls to slot %d\n", g_li_seed_slot, slot);
    }
    if (slot < 0) {
        std::fprintf(stderr, "[luigi] seed: no player bodies in the arena, "
                "nothing seeded\n");
        return;
    }
    std::fprintf(stderr, "[luigi] SEED f%d: slot %d starts as Luigi (the "
            "tagger); local player is slot %d\n", frame, slot,
            (int)data_0209f250);
    li_infect(slot);
}

/* MINIMAP HIDE: after Minimap::Behavior has reseated every slot's blip, blank
 * the mapID of every Luigi-team slot so the frozen Render loop's `== current
 * map` test never matches it -- survivors see every other survivor and never
 * the taggers. Called from the tail of the Behavior host copy, before Render
 * reads the band the same frame. Inert when the mode is off. */
extern "C" signed char data_ov002_02111148;   /* the minimap's current map id */

extern "C" void port_luigi_minimap_hide(void)
{
    if (!g_li_on) return;
    for (int i = 0; i < kPortMaxPlayers; ++i)
        if (g_li_team[i]) g_mm_mapID[i] = -1;

    /* SM64DS_VS_LUIGI_PROBE=1: after the blanking, snapshot the band once a
       second so a headless proof can read that every Luigi-team slot's mapID is
       -1 (the Render `== current map` test can never match it) while every
       survivor keeps a live mapID that DOES match -- the blip is drawn for
       survivors and suppressed for taggers, per slot. Gated and off by default. */
    static int probe = -1;
    if (probe < 0) probe = std::getenv("SM64DS_VS_LUIGI_PROBE") ? 1 : 0;
    if (!probe) return;
    static unsigned tick;
    if ((tick++ % 60) != 0) return;
    std::fprintf(stderr, "[luigi] MINIMAP curmap=%d", (int)data_ov002_02111148);
    for (int i = 0; i < kPortMaxPlayers; ++i) {
        if (!data_0209f394[i]) continue;
        std::fprintf(stderr, " s%d[%s mapID=%d]", i,
                     g_li_team[i] ? "LUIGI" : "surv", (int)g_mm_mapID[i]);
    }
    std::fprintf(stderr, "\n");
}

/* ---- SM64DS_VS_LUIGI_HITTEST: stage real hits through the host-copied hit
 * resolver, to prove steps 4 and 5 without waiting for two AI players to punch
 * each other. It sets exactly the fields func_ov002_020d869c reads on a real
 * arena Player -- the attacker's uniqueID at victim+0x2f8, the move flags at
 * +0x2f4, the two top-of-function gates at +0xd0 / +0x713 / +0x709 -- calls the
 * resolver, and RESTORES every clobbered field afterwards so the arena is left
 * as it was. Off by default; only ever set by tools/luigi_proof.py. */
extern "C" int func_ov002_020d869c(char *c);

static unsigned li_uid(void *actor) { return *(unsigned *)((char *)actor + 4); }

/* Point victim at attacker as if attacker had just landed a hit with `flags`,
 * call the resolver, restore the victim's fields, and return what the resolver
 * returned. `flags == 0` is the ground-pound/jump path, which returns without
 * connecting when the two are not stacked -- safe, and enough to show whether
 * the resolver INTERCEPTED the pair (early return) or fell through to the ROM. */
static int li_stage_hit(void *victim, void *attacker, unsigned flags)
{
    char *c = (char *)victim;
    const int   sv_d0  = *(int *)(c + 0xd0);
    const unsigned char sv_713 = *(unsigned char *)(c + 0x713);
    const unsigned char sv_709 = *(unsigned char *)(c + 0x709);
    const unsigned sv_2f8 = *(unsigned *)(c + 0x2f8);
    const unsigned sv_2f4 = *(unsigned *)(c + 0x2f4);

    *(int *)(c + 0xd0) = 0;                 /* the two top gates open */
    *(unsigned char *)(c + 0x713) = 1;
    *(unsigned char *)(c + 0x709) = 0;      /* so flags==0 reaches the jump test */
    *(unsigned *)(c + 0x2f8) = li_uid(attacker);
    *(unsigned *)(c + 0x2f4) = flags;

    const int rc = func_ov002_020d869c(c);

    *(int *)(c + 0xd0) = sv_d0;
    *(unsigned char *)(c + 0x713) = sv_713;
    *(unsigned char *)(c + 0x709) = sv_709;
    *(unsigned *)(c + 0x2f8) = sv_2f8;
    *(unsigned *)(c + 0x2f4) = sv_2f4;
    return rc;
}

static int li_char_of(int slot)
{
    void *p = data_0209f394[slot];
    return p ? (int)(*(unsigned char *)((char *)p + 0x6d9) & 7) : -1;
}

extern "C" void port_luigi_hittest(int frame)
{
    li_load();
    static int on = -1, tagall = -1;
    if (on < 0) on = std::getenv("SM64DS_VS_LUIGI_HITTEST") ? 1 : 0;
    if (tagall < 0) tagall = std::getenv("SM64DS_VS_LUIGI_TAGALL") ? 1 : 0;
    if ((!on && !tagall) || frame != 130) return;

    /* TAG-ALL: Luigi tags every survivor through the real resolver, so the next
       match-end poll sees survivors_alive==0 and fires the LUIGIS-win path.
       Proves the all-infected win end to end from real tags. */
    if (tagall && g_li_on && port_luigi_seeded()) {
        int luigi = -1;
        for (int i = 0; i < kPortMaxPlayers; ++i)
            if (data_0209f394[i] && g_li_team[i]) { luigi = i; break; }
        if (luigi >= 0) {
            for (int i = 0; i < kPortMaxPlayers; ++i)
                if (data_0209f394[i] && !g_li_team[i])
                    li_stage_hit(data_0209f394[i], data_0209f394[luigi], 0x400);
            std::fprintf(stderr, "[luigi] TAGALL f%d: survivors_alive now %d\n",
                         frame, port_luigi_survivors_alive());
        }
    }
    if (!on) return;

    /* one Luigi, and the survivors, among the live slots. The scenarios run in
       the order C, A, B on distinct slots so C sees PRISTINE survivors -- a tag
       in A must not turn C's attacker into a Luigi (the trap the first cut hit). */
    int luigi = -1, surv[kPortMaxPlayers], nsurv = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i) {
        if (!data_0209f394[i]) continue;
        if (g_li_team[i]) { if (luigi < 0) luigi = i; }
        else surv[nsurv++] = i;
    }
    std::fprintf(stderr, "[luigi] HITTEST f%d: luigi=%d survivors=%d\n",
                 frame, luigi, nsurv);

    if (nsurv >= 2) {
        /* C -- SURVIVOR vs SURVIVOR (run FIRST, pristine): the resolver is NOT
           intercepted (rc=1, fell through to the ROM path) and the victim does
           NOT convert. With the mode off the guard is skipped entirely and the
           result is identical -- that equivalence is the regression check. */
        const int a = surv[0], v = surv[1];
        const int before = li_char_of(v), team_b = g_li_team[v];
        const int rc = li_stage_hit(data_0209f394[v], data_0209f394[a], 0);
        std::fprintf(stderr, "[luigi] HITTEST C SURV-SURV (mode %s): surv %d -> "
                "surv %d | rc=%d char %d->%d team %d->%d | %s\n",
                g_li_on ? "ON" : "off", a, v, rc, before, li_char_of(v),
                team_b, g_li_team[v],
                (rc == 1 && li_char_of(v) == before
                 && g_li_team[v] == team_b) ? "PASS" : "FAIL");
    }
    if (g_li_on && luigi >= 0 && nsurv >= 1) {
        /* A -- TAG: Luigi attacks a survivor. Expect the survivor converts. */
        const int v = surv[0];
        const int before = li_char_of(v), team_b = g_li_team[v];
        const int rc = li_stage_hit(data_0209f394[v], data_0209f394[luigi], 0x400);
        std::fprintf(stderr, "[luigi] HITTEST A TAG: luigi %d -> surv %d | rc=%d "
                "char %d->%d team %d->%d | %s\n", luigi, v, rc, before,
                li_char_of(v), team_b, g_li_team[v],
                (rc == 1 && li_char_of(v) == 1 && g_li_team[v]) ? "PASS" : "FAIL");
    }
    if (g_li_on && luigi >= 0 && nsurv >= 2) {
        /* B -- IMMUNITY: a survivor (the one NOT tagged in A) attacks Luigi.
           Expect no change and rc=0 -- Luigi is never knocked, dropped or
           tagged, and this same guard makes Luigi-on-Luigi a no-op too. */
        const int a = surv[1];
        const int before = li_char_of(luigi), team_b = g_li_team[luigi];
        const int rc = li_stage_hit(data_0209f394[luigi], data_0209f394[a], 0x400);
        std::fprintf(stderr, "[luigi] HITTEST B IMMUNE: surv %d -> luigi %d | "
                "rc=%d char %d->%d team %d->%d | %s\n", a, luigi, rc, before,
                li_char_of(luigi), team_b, g_li_team[luigi],
                (rc == 0 && li_char_of(luigi) == before
                 && g_li_team[luigi] == team_b) ? "PASS" : "FAIL");
    }
}

/* Reset the per-match latch when a match tears down, so a second match in the
 * same process re-seeds cleanly. Called from the match-end disarm in
 * hal/star_flow.cpp (the one place that already tracks VS entry/exit). */
extern "C" void port_luigi_match_reset(void)
{
    g_li_seeded = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i) g_li_team[i] = 0;
}
