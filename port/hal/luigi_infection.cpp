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
 *   SM64DS_VS_LUIGI_SEED       a u32 in decimal (read as the session check
 *                              reads it: strtoul base 10, low 32 bits),
 *                              default 0. The tagger is the
 *                              (seed mod n)-th slot that has a body at the pick,
 *                              n = the number of bodies. A VS arena seats its
 *                              bodies in slots 0..n-1, so 0..n-1 still name a
 *                              slot directly (the old 0..15 grammar, unchanged
 *                              for every value that had a body) and the
 *                              lobby's server-minted 32-bit seed spreads evenly
 *                              over whoever is in the arena. See the LOCKSTEP
 *                              note below: a local rand() would desync a real
 *                              session, so there is none.
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
 * other console anything.
 *
 * ONLINE: THE MATCH PLAN IS THE ONLY SOURCE (1.0).  In an online session the
 * mode and the seed are the room's, not the machine's. The host picks Luigi
 * Infection in the lobby, the lobby server mints the seed at start, and every
 * console's launcher exports the same two values from the same go plan as
 * SM64DS_VS_LUIGI_INFECTION and SM64DS_VS_LUIGI_SEED. Nothing else counts:
 * hal/host_settings.cpp does not bridge the per-machine VsLuigiInfection key
 * into the environment when SM64DS_COMMS_ROLE names a session, so a player
 * whose settings.json still has the mode on cannot arm it for one console of
 * a room that did not ask for it (the per-machine key was how two consoles in
 * one room could run different modes, or pick different Luigis, and desync).
 * The session check the carrier runs at join (hal/comms_loopback.cpp, the
 * ACCEPT's settings digest, "...|luigi|seed|...") reads the same two variables
 * with the same rules as li_load below (luigi: unset / "" / "0" is 0, else 1;
 * seed: strtoul base 10, low 32 bits, counted only when luigi is 1), so a
 * console whose plan disagrees with the host's, or whose own settings file
 * would have armed the mode, is refused at the door (exit 3) instead of
 * desyncing at frame 240. Keep the two readers in step. Solo keeps the key.
 */

#include <cstdlib>
#include <cstdio>
#include "vs_width.h"

extern "C" {
extern unsigned char data_0209f2d8;        /* game mode: 0 adventure, 1 VS */
extern void         *data_0209f394[];      /* per-slot Player* (null = no body) */
extern unsigned char data_0209fc5c[];      /* per-seat live byte, 16 wide */
extern unsigned char data_0209f250;        /* local player index */
extern void port_player_set_character(void *player, unsigned ch);
unsigned port_vs_seat_live_mask(void);     /* hal/comms_loopback.cpp */
unsigned port_vs_seat_mask_round(void);    /* ... and its round      */
int port_vs_match_end_frozen(void);        /* hal/star_flow.cpp */
}

/* NONZERO WHILE A REWIND IS BEING RE-RUN. The mode's WORLD writes (the team
 * bits, the character swap, the mapID blanking) must happen on a replayed frame
 * -- reproducing them is what the replay is for -- but its EFFECTS on the
 * outside world stand down, the same rule the pad, the mouse, the camera rig
 * and the editor channel follow in the frame loop.
 *
 * DEFINED HERE AND WRITTEN BY hal/rollback.cpp, rather than read back out of
 * rollback's own rb_replaying(), because this file and hal/star_flow.cpp are
 * linked into targets that rollback.cpp is NOT in (smoke_player, the hires
 * window). Calling rb_replaying() from here is an unresolved external in every
 * one of them. Owning the flag on this side inverts that: a build with the
 * rollback boundary sets it, and a build without one leaves it 0 -- which is
 * exactly the truth for a target that can never replay a frame. */
extern "C" int g_port_rb_replaying = 0;

/* the minimap's per-player mapID band, sixteen wide, DEFINED in
 * unmatched/Minimap_Behavior.cpp (which inherits C linkage from the extern "C"
 * block in Minimap_wide.h); -1 in a slot means "draw no blip for it". Declared
 * extern "C" here so the symbol name matches that definition, not a C++ mangle. */
extern "C" signed char g_mm_mapID[kPortMaxPlayers];

/* engine character index: Mario 0, Luigi 1, Wario 2, Yoshi 3. */
enum { kLuigiChar = 1 };

static int g_li_read;                     /* env parsed once */
static int g_li_on;                       /* mode enabled */
static unsigned g_li_seed;                /* the tagger pick's seed (u32) */
static int g_li_time_frames;              /* >0 = test timer, in frames */
static int g_li_seeded;                   /* the seed has run this match */
static int g_li_team[kPortMaxPlayers];    /* 1 = on Luigi's team (infected) */
static unsigned g_li_absent;              /* bodies no round carried at START */

static void li_load(void)
{
    if (g_li_read) return;
    g_li_read = 1;
    g_li_on = 0;
    g_li_seed = 0;
    g_li_time_frames = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i) g_li_team[i] = 0;

    const char *e = std::getenv("SM64DS_VS_LUIGI_INFECTION");
    if (!e || e[0] == 0 || (e[0] == '0' && e[1] == 0))
        return;                            /* absent / "0": OFF, and silent */
    g_li_on = 1;

    /* The seed exactly as the session digest reads it (strtoul base 10, low
       32 bits), so the value the join check compared is the value that picks
       the tagger, whatever string the plan carried. */
    if (const char *s = std::getenv("SM64DS_VS_LUIGI_SEED"))
        g_li_seed = (unsigned)(std::strtoul(s, 0, 10) & 0xFFFFFFFFul);

    if (const char *t = std::getenv("SM64DS_VS_LUIGI_TIME")) {
        int secs = std::atoi(t);
        if (secs > 0) g_li_time_frames = secs * 60;
    }
    const char *role = std::getenv("SM64DS_COMMS_ROLE");
    std::fprintf(stderr, "[luigi] Luigi Infection ARMED%s: tagger seed = %u, "
            "timer = %s\n", (role && *role) ? " (online: the match plan)" : "",
            g_li_seed, g_li_time_frames ? "override" : "ROM match clock");
}

/* SEAMCLEAN1: Luigi Infection is read once per process (li_load). A VS hand-off to the running game takes the names of the match it
   brings (SM64DS_VS_LUIGI_INFECTION, _SEED, _TIME), so the read, the round's seed latch and the team table go back to "not yet". */
extern "C" void port_luigi_reset(void)
{
    g_li_read = 0;
    g_li_on = 0;
    g_li_seeded = 0;
    g_li_absent = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i) g_li_team[i] = 0;
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

/* ---- THE ROLLBACK SNAPSHOT'S COPY OF THIS MODE'S STATE --------------------
 * g_li_team and g_li_seeded ARE simulation state: the team array decides who
 * the minimap hides, who the hit resolver converts and who the win poll counts,
 * and li_infect swaps the player's BODY as a consequence of setting a bit here.
 * They are host statics, so they sit outside all three regions the rollback
 * snapshot copies (the DS arena, .dsstate and the hardware stores) -- and
 * .dsstate is not the place for them, because hal/dsstate_seg.h reserves that
 * bracket for hosted DS globals and tools/dsstate_guard.py says in as many
 * words that host-only symbols must NOT be in it.
 *
 * So they ride the snapshot the way the seam's counters and the divergence
 * detector's clock already do (port_comms_counters_get/set, port_dh_frame_get/
 * set in hal/rollback.cpp): a getter and a setter the frame boundary calls, and
 * the slot carries the bytes beside the arena cursor.
 *
 * WHAT GOES WRONG WITHOUT THIS, precisely. li_infect early-returns when the
 * team bit is already set. A rewind puts the arena back -- the converted player
 * is a survivor again -- but left the bit set, so the replay's tag reaches that
 * early return and NEVER RE-APPLIES the character swap. The world and this
 * array then disagree for the rest of the match, and the two consoles disagree
 * with each other as soon as their rewinds differ. The same holds for
 * g_li_seeded across the pick at frame 240: it latches, so a rewind over it
 * loses the tagger for good.
 *
 * The env-derived configuration (g_li_read / g_li_on / g_li_seed /
 * g_li_time_frames) is deliberately NOT carried: it is parsed once out of an
 * environment every peer shares and never moves after, so restoring it would
 * copy bytes that cannot have changed. */
extern "C" void port_luigi_state_get(int *team, int *seeded)
{
    for (int i = 0; i < kPortMaxPlayers; ++i) team[i] = g_li_team[i];
    *seeded = g_li_seeded;
}

extern "C" void port_luigi_state_set(const int *team, int seeded)
{
    for (int i = 0; i < kPortMaxPlayers; ++i) g_li_team[i] = team[i];
    g_li_seeded = seeded;
}

/* IN THE MATCH: a slot with a body whose seat is still live. A player who
 * LEAVES a VS match keeps his body in data_0209f394 -- hal/star_flow.cpp's
 * vs_remove_body parks it far out of the level and clears the seat's live
 * byte data_0209fc5c[k], the way the cartridge's own in-match drop does
 * (src/unnamed/arm9/0203/func_02030b58.c) -- so counting bodies counts the leaver for the rest
 * of the match. A survivor who left then stayed a survivor nobody could reach:
 * the Luigis could never infect "everyone", and the round ran to the clock
 * and went to the survivors even with every player still there caught.
 * The live byte is cleared from the seat mask the round carries
 * (port_vs_seat_live_mask / port_vs_seat_mask_round), at the same round on
 * every console, and it is arena state, so a rewind puts it back with the
 * world. Once the match is decided star_flow stops removing seats, so the
 * counts the banner reads afterwards hold still. A seat no round carried at
 * START (g_li_absent, latched by the pick below) is not in the match either. */
static int li_in_match(int i)

{
    return data_0209f394[i] && data_0209fc5c[i] && !(g_li_absent >> i & 1u);
}

/* survivors still alive: slots in the match that are NOT on Luigi's team.
 * -1 when the mode is off, so a caller can tell "no survivors" (0) from "not
 * this mode" (-1). */
extern "C" int port_luigi_survivors_alive(void)
{
    if (!g_li_on) return -1;
    int n = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i)
        if (li_in_match(i) && !g_li_team[i]) ++n;
    return n;
}

/* how many players are in this match at all (team or not); a slot that left
 * is not one of them. */
extern "C" int port_luigi_match_slots(void)
{
    int n = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i)
        if (li_in_match(i)) ++n;
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
    if (s < 0 || g_li_team[s] || !li_in_match(s)) return;
    if (!g_port_rb_replaying)
    std::fprintf(stderr, "[luigi] TAG: slot %d converted to Luigi "
            "(survivors left = %d)\n", s, port_luigi_survivors_alive() - 1);
    li_infect(s);
}

/* THE LAST LUIGI LEFT. A Luigi who leaves the match is parked and can tag
 * nobody (hal/star_flow.cpp removes the body at the round the seat mask drops
 * him), so when every Luigi has left, the survivors could not be caught and
 * the round would just run out the clock. The Luigi passes on instead: the
 * (seed mod n)-th player still in the match, by the same rule as the first
 * pick, becomes the Luigi. Everything it reads (the team array, the live
 * bytes star_flow cleared this frame, the shared seed) is the same on every
 * console at the same frame, so every console hands off on the same round to
 * the same player. Two or more players left: with one, star_flow's last
 * player standing ends the match; not once the match is decided. */
static void li_hand_off(int frame)
{
    if (port_vs_match_end_frozen()) return;
    int n = 0, luigis = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i)
        if (li_in_match(i)) { ++n; if (g_li_team[i]) ++luigis; }
    if (luigis || n < 2) return;
    int slot = -1;
    for (int i = 0, k = (int)(g_li_seed % (unsigned)n); i < kPortMaxPlayers; ++i)
        if (li_in_match(i) && k-- == 0) { slot = i; break; }
    if (!g_port_rb_replaying)
    std::fprintf(stderr, "[luigi] HAND-OFF f%d (round %u): every Luigi left "
            "the match; slot %d is the Luigi now (seed %u over %d players)\n",
            frame, port_vs_seat_mask_round(), slot, g_li_seed, n);
    li_infect(slot);
}

/* SEED: turn exactly one slot into the starting Luigi. VS only, once per match.
 * The TIMING is owned by the pre-round countdown driver (hal/star_flow.cpp's
 * port_luigi_countdown_drive), which calls this at countdown end (frame 240, a
 * full five seconds in) rather than the old frame 90 -- so the tagger is chosen
 * only after the READY? / 3-2-1 / START countdown has played out. Self-latches
 * on g_li_seeded, so being called every frame from the pick on seeds once;
 * after that each call only checks whether the Luigis are still here. */
extern "C" void port_luigi_seed(int frame)
{
    if (data_0209f2d8 != 1) return;        /* VS mode only */
    li_load();
    if (!g_li_on) return;
    if (g_li_seeded) { li_hand_off(frame); return; }
    g_li_seeded = 1;

    /* WHO IS IN THE MATCH AT START. The arena seats a body for every seat the
       room planned, and a seat whose console never made it into the session
       (refused at the door, crashed or quit before its first round) keeps that
       body where it spawned: a statue nobody plays. A statue picked as the
       tagger leaves a round nobody can lose; a statue counted as a survivor
       leaves a round the Luigis can never win. The seat mask the round
       carries says who is really here, the same on every console at this
       frame (0 = no session: every body is a player). Latched here, at the
       pick, so the same seats stay out for the whole round; not carried in
       the rollback slot because a rewind over the pick clears g_li_seeded and
       this runs again from the same round. A seat that is seen and then
       leaves is star_flow's (its live byte), before or after the pick. */
    g_li_absent = 0;
    const unsigned live = port_vs_seat_live_mask();
    if (live) {
        for (int i = 0; i < kPortMaxPlayers; ++i)
            if (data_0209f394[i] && !(live >> i & 1u)) g_li_absent |= 1u << i;
        if (g_li_absent && !g_port_rb_replaying)
            std::fprintf(stderr, "[luigi] START f%d: seat mask %04x at round "
                    "%u: bodies %04x are not in the match (not picked, not "
                    "counted)\n", frame, live, port_vs_seat_mask_round(),
                    g_li_absent);
    }

    /* THE PICK: the (seed mod n)-th slot in the match, n = the players in
       the match. A pure function of the shared seed, of data_0209f394 and of
       the round's seat mask, which every console holds identically at this
       frame, so every console picks the same tagger; and every player is a
       possible pick, so the mode always has exactly one tagger however many
       players the room had. */
    int n = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i)
        if (li_in_match(i)) ++n;
    if (n == 0) {
        std::fprintf(stderr, "[luigi] seed: no player bodies in the arena, "
                "nothing seeded\n");
        return;
    }
    int slot = -1;
    for (int i = 0, k = (int)(g_li_seed % (unsigned)n); i < kPortMaxPlayers; ++i)
        if (li_in_match(i) && k-- == 0) { slot = i; break; }
    if (!g_port_rb_replaying)
    std::fprintf(stderr, "[luigi] SEED f%d: slot %d starts as Luigi (the "
            "tagger); local player is slot %d; seed %u over %d bodies\n",
            frame, slot, (int)data_0209f250, g_li_seed, n);
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
    /* rb_replaying before the tick below, not after: the cadence counter is a
       host static a rewind cannot put back, so a replayed frame would both
       double-print and walk the counter past where the straight run left it. */
    if (!probe || g_port_rb_replaying) return;
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
    /* frame 260: after the pre-round countdown seeds the tagger at frame 240, so
       a Luigi exists on the team for the TAG / IMMUNITY / TAG-ALL scenarios. */
    if ((!on && !tagall) || frame != 260) return;

    /* TAG-ALL: Luigi tags every survivor through the real resolver, so the next
       match-end poll sees survivors_alive==0 and fires the LUIGIS-win path.
       Proves the all-infected win end to end from real tags. Only players
       still in the match: a Luigi can not reach a body that left (parked). */
    if (tagall && g_li_on && port_luigi_seeded()) {
        int luigi = -1;
        for (int i = 0; i < kPortMaxPlayers; ++i)
            if (li_in_match(i) && g_li_team[i]) { luigi = i; break; }
        if (luigi >= 0) {
            for (int i = 0; i < kPortMaxPlayers; ++i)
                if (li_in_match(i) && !g_li_team[i])
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
        if (!li_in_match(i)) continue;
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
    g_li_absent = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i) g_li_team[i] = 0;
}
