// TEMPORARY PROBE: a frame-scripted pad press for headless runs.
//
// This is a test rig, not game behaviour. There is no person on the pad in a
// headless selftest, so a dialogue box that reaches its page/close prompt
// (Message::Update state 7/8, gated on IsButtonInputValid) holds forever, and a
// talk that needs an A press to start never starts. This lets a run script the
// presses from the command line so the whole talk chain -- open, advance, close
// -- can be driven and captured with no rebuild.
//
//   SM64DS_PROBE_INPUT="120:A,180:A"   -- frame:button pairs, comma separated.
//   On each listed frame the named DS pad bits are ORed into the pad mirror for
//   that one frame. A range "120-123:A" holds the press across the frames
//   120..123 inclusive (an edge can be missed if it lands on a paused frame, so
//   a short hold is the safe way to guarantee the box sees it). Button letters:
//     A B X Y  L R  U D (dpad)  START SELECT
//   mapped to the DS KEYINPUT bit layout (A=bit0, B=bit1, ...). Case
//   insensitive, '+' joins buttons on one frame ("120:A+B").
//
// WHY IT WRITES TWO SYMBOLS. On the DS, data_020a0e58 is PadData[4] with stride
// 4 ({u16 held @ +0, u16 pressed @ +2}), so data_020a0e5a IS data_020a0e58+2 --
// the same memory, pad 0's "pressed" field. In the port those are two separate
// auto_bss symbols, so a write to one does not reach the other. IsButtonInputValid
// and Message::Update read data_020a0e5a[player*2]; Stage::CheckInput reads
// data_020a0e58[i].held/pressed and remaps it into the Ctrl block the walk
// states read. To make one scripted press reach both consumers the probe writes
// the DS mirror data_020a0e58 (held AND pressed) and the split pressed symbol
// data_020a0e5a together, restoring the DS aliasing for that frame.
//
// Delete this file, its CMake lines and its two call sites in walk_window when a
// real input source (a recorded demo, a person on the pad) drives the box.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

extern "C" {

/* the DS KEYINPUT mirror (PadData[4], {u16 held, u16 pressed}) and the split
   "pressed" symbol that overlays pad 0's pressed field on hardware */
extern unsigned short data_020a0e58[];   /* [0]=pad0.held [1]=pad0.pressed ... */
extern int data_020a0e5a[];              /* the pressed-word split symbol */
extern unsigned char data_020a0e40;      /* the local player index */
extern unsigned char data_0209d6bc;      /* the Message box state (Message::Update) */
extern unsigned char data_0209d660;      /* nonzero while a message is active */
extern unsigned char data_0209d684;      /* the choice box's answer (0 = unanswered) */
/* the rest of the words the star's post-collect sequence gates on, for the
   SM64DS_TRACE_STAR readout below */
extern unsigned char data_0209d654;      /* save-screen arm flag */
extern unsigned char data_0209d67c;      /* save-screen close countdown */
extern unsigned char data_0209d664;      /* Message's in-talk flag */
extern short data_0209d6d4;              /* current message id (-1 = none) */
extern int _ZN6Player12GetTalkStateEv(void *p);
/* the pause triple Actor::BeforeBehavior and IsButtonInputValid both read */
extern int data_0209f20c[], data_0209f294[], data_0209f2c4[];
/* the Ctrl block's analog magnitude and X deflection, which the walk states
   read: mag 0 means the input layer published no stick this frame */
extern char data_0209f4a0[];
extern int data_0209f4a2[];
extern int data_0209b454[];              /* persistent freeze-request word */
extern unsigned int data_0209b464;       /* this frame's copy of it (the mask) */
extern int data_0208e430;                /* the track id Sound::PlaySub latched */
extern int data_0209b490[];              /* music volume fade word */
extern int data_0209b49c[];              /* music state fade word */

}  /* extern "C" */

static void *find_actor_by_class(unsigned short cls);

/* TEMPORARY star-sequence trace: SM64DS_TRACE_STAR=1 logs, every frame a
   PowerStar (class 178) is alive, the whole set of words the post-collect
   sequence gates on, so a freeze says WHICH gate it is sitting on instead of
   just being still.

   The sequence is func_ov002_020e9af4 (the star's state 11) switching on the
   star's own +0x49b, and every one of its arms waits on something printed
   here:
     case 2  Player::GetTalkState(player) == -1, then the choice bits
             (+0x4a2 >> 10) & 3: 1 = save, 2 = do not save, 0/3 = NO ARM,
             which is a permanent spin
     case 3  data_0209d660 == 0   (the box fully closed)
     case 4  func_ov002_020c6e14
     case 5  data_0209d660 == 0
     case 6  func_ov002_020e8618, and control comes back
   d654/d67c are the save-screen arm's own flag and countdown
   (hal/message_pump.cpp), d664 is Message's in-talk flag, d684 the choice
   box's answer that +0x4a2 latches from. Reads only. */
extern "C" void port_input_probe_trace_star(int frame)
{
    if (!std::getenv("SM64DS_TRACE_STAR")) return;
    char *st = (char *)find_actor_by_class(178);
    char *pl = (char *)find_actor_by_class(0xbf);
    static int was_alive, after;
    if (!st && !was_alive && !after)
        return;                     /* nothing to say before the star exists */
    if (!st && was_alive) {
        std::fprintf(stderr, "[star] f%d the PowerStar is gone (killed or "
                     "destroyed)\n", frame);
        was_alive = 0;
        after = 1;
    }
    if (st) {
        was_alive = 1;
    } else if (after) {
        /* keep reporting for a while past the star's death: the freeze this
           trace exists for shows up AFTER the level change, in the mask and
           the fresh player's flags, long after the star itself is gone. */
        if (++after > 600)
            return;
    }
    unsigned short a2 = st ? *(unsigned short *)(st + 0x4a2) : 0;
    /* THE FREEZE WORD IS THE POINT. data_0209b454 is the persistent
       freeze-REQUEST word the collect handler ORs 0x4000000 into
       (src/func_ov002_020e8ef0.cpp:151-155); data_0209b464 is this frame's
       latched copy, and Actor::BeforeBehavior skips every actor whose own
       +0xb0 does not intersect it. func_ov002_020e8618 is the only clear.
       Printed with the player's own +0xb0 so a freeze says whether the
       player is INSIDE or OUTSIDE the mask that is still up. */
    std::fprintf(stderr,
                 "[star] f%d %s sub49b=%d choice=%u a2=%04x x43c=%d x440=%d "
                 "x490=%u x49d=%u | pl step6e3=%u kind70a=%u msg688=%d "
                 "noctl709=%u talk=%d plb0=%08x | b454=%08x b464=%08x | "
                 "pause(f20c=%u f294=%u f2c4=%u) stick(mag=%d nx=%d) | "
                 "d660=%u d654=%u d67c=%u d664=%u d684=%u msgid=%d\n",
                 frame, st ? "star" : "STAR-GONE",
                 st ? (int)*(unsigned char *)(st + 0x49b) : -1,
                 (unsigned)((a2 >> 10) & 3), (unsigned)a2,
                 st ? *(int *)(st + 0x43c) : -1,
                 st ? *(int *)(st + 0x440) : -1,
                 st ? (unsigned)*(unsigned short *)(st + 0x490) : 0u,
                 st ? (unsigned)*(unsigned char *)(st + 0x49d) : 0u,
                 pl ? (unsigned)*(unsigned char *)(pl + 0x6e3) : 99u,
                 pl ? (unsigned)*(unsigned char *)(pl + 0x70a) : 99u,
                 pl ? *(int *)(pl + 0x688) : -99,
                 pl ? (unsigned)*(unsigned char *)(pl + 0x709) : 99u,
                 pl ? _ZN6Player12GetTalkStateEv(pl) : -99,
                 pl ? (unsigned)*(unsigned int *)(pl + 0xb0) : 0u,
                 (unsigned)data_0209b454[0], data_0209b464,
                 (unsigned)data_0209f20c[0], (unsigned)data_0209f294[0],
                 (unsigned)data_0209f2c4[0],
                 (int)*(short *)data_0209f4a0, (int)*(short *)data_0209f4a2,
                 (unsigned)data_0209d660, (unsigned)data_0209d654,
                 (unsigned)data_0209d67c, (unsigned)data_0209d664,
                 (unsigned)data_0209d684, (int)data_0209d6d4);
}

/* TEMPORARY message-state trace: SM64DS_TRACE_MSG=1 logs the box state each
   frame it is active, so a headless close can be watched frame by frame. */
extern "C" void port_input_probe_trace_msg(int frame)
{
    port_input_probe_trace_star(frame);
    if (!std::getenv("SM64DS_TRACE_MSG")) return;
    static int was_active;
    int active = data_0209d660 != 0;
    if (active || was_active)
        std::fprintf(stderr, "[msg] f%d active=%d state=%d\n",
                     frame, active, (int)data_0209d6bc);
    was_active = active;
}

/* TEMPORARY cannon-bit trace: SM64DS_TRACE_CANNON=1 reads the MATCHED
   IsCannonOpenInCurLevel() each frame and logs the edge, so a headless run shows
   the exact frame the Bob-omb Buddy's OpenCannonInCurLevel() flips the save bit
   through the real talk path. This is the whole point of the seam. */
extern "C" int IsCannonOpenInCurLevel(void);
extern "C" void port_input_probe_trace_cannon(int frame)
{
    if (!std::getenv("SM64DS_TRACE_CANNON")) return;
    static int last = -1;
    int open = IsCannonOpenInCurLevel() != 0;
    if (open != last) {
        std::fprintf(stderr, "[cannon] f%d IsCannonOpenInCurLevel=%d%s\n",
                     frame, open, last >= 0 && open ? "  <-- FLIPPED OPEN" : "");
        last = open;
    }
}

/* TEMPORARY talk-trigger for a headless Bob-omb Buddy proof.
 *
 * On the DS the buddy's state-0 main (func_ov084_0212c8b0) enters the talk only
 * once its collision cylinder has detected the player: it reads its own +0xf4
 * clsn flags for bit 0x8000000 and its +0xf8 clsn-partner id, finds that actor
 * (the player, class 0xbf) and calls the REAL Player::StartTalk. The port does
 * not yet drive that cylinder overlap for this parallel-lane class headlessly,
 * so this stands in for JUST the detection: it sets +0xf4 |= 0x8000000 and
 * +0xf8 = the player's unique id (player+0x4, what Actor::FindWithID matches),
 * so the buddy's OWN state-0 main runs the real StartTalk on the real player.
 * Everything downstream -- StartTalk, ChangeState(ST_TALK), the buddy's talk and
 * walk-back states, ShowMessage, GetTalkState, OpenCannonInCurLevel -- is
 * genuine matched code. Nothing here writes the cannon bit or fakes a talk.
 *
 *   SM64DS_BUDDY_TRIGGER=1  -- arm the detection each frame from frame 60 on.
 *
 * Delete with the probe when the buddy's cylinder overlap is hosted headlessly.
 */
extern "C" {
extern void *data_0209b468;                 /* the live-actor list head node */
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov084_0212c8b0(void *buddy);   /* buddy state-0 main */
extern void func_ov084_0212c960(void *buddy, int i);  /* buddy ChangeState */
extern unsigned char data_0209f49e[];        /* per-player pressed word, stride 0x18 */
extern int _ZN6Player12GetTalkStateEv(void *p);
/* STAGE 0 (temporary, investigation only): the VS carried-star array and the
   ROM's own sum of it. Read only, never written here. */
extern signed char data_0209f310[];
extern signed char NumVsStarsObtained(void);
extern unsigned char data_0209f2d8;          /* game mode: 1 = VS            */
extern unsigned char data_0209f204;          /* VS "time is up" flag         */
extern unsigned short data_ov002_02111188;   /* VS timer sub-counter         */
extern int data_0209fc68;                    /* wireless session state       */
extern unsigned char data_0209f2bc;          /* the 3-2-1 countdown          */
extern void func_ov002_020e7d84(char *m);    /* StarMarker's own ball break  */
}

/* STAGE 0 (temporary): walk EVERY class-178 PowerStar in the live list and
   report the ones the five-star test actually looks at, plus the score array.
   A VS arena carries five of them and only one is awake at a time -- the
   harness's own find_actor_by_class takes the FIRST, which in every arena
   measured is a caged one parked in state 9, so the touch words it writes are
   read by nobody. Nothing here writes game state. */
extern "C" void port_stage0_vs_score(int frame)
{
    if (!std::getenv("SM64DS_VS_SCORE")) return;
    struct Node { void *x0; Node *next; char **x8; };
    static int last_sum = -999, last_shape = -1;
    int sum = (int)NumVsStarsObtained();
    char shape[512];
    int n = 0, w = 0;
    shape[0] = 0;
    Node *nd = *(Node **)&data_0209b468;
    while (nd && n < 16) {
        char *a = (char *)nd->x8;
        if (a && *(unsigned short *)(a + 0xc) == 178) {
            w += std::snprintf(shape + w, sizeof shape - (unsigned)w,
                               " [%p kind=%d st=%d idx=%u a2=%04x]",
                               (void *)a, *(int *)(a + 0x43c),
                               *(int *)(a + 0x440),
                               (unsigned)*(unsigned char *)(a + 0x49d),
                               (unsigned)*(unsigned short *)(a + 0x4a2));
            ++n;
            if (w > 400) break;
        }
        nd = nd->next;
    }
    int hash = n * 7919 + w;
    for (int i = 0; i < w; ++i) hash = hash * 31 + shape[i];
    if (sum == last_sum && hash == last_shape) return;
    last_sum = sum; last_shape = hash;
    std::fprintf(stderr, "[vsscore] f%d mode=%u stars=%d,%d,%d,%d sum=%d "
                 "| %d PowerStar(s):%s\n", frame, (unsigned)data_0209f2d8,
                 (int)data_0209f310[0], (int)data_0209f310[1],
                 (int)data_0209f310[2], (int)data_0209f310[3], sum, n,
                 w ? shape : " none");
}

/* STAGE 0 (temporary): SM64DS_VS_BREAKALL=<frame>. Correction from Tango: a
   VS star comes out when a player BREAKS THE BALL around it; the marker's own
   break, func_ov002_020e7d84, is that release, and the earlier runs proved
   only that robot players who never swing leave every ball intact. At the
   armed frame this dumps every star<->marker ID link, then runs the ROM's own
   break on EVERY container marker, so the picked star's own marker is
   guaranteed to be among them. Fires identically in both lockstep processes
   (same env, same list) so the worlds stay in step. */
extern "C" void port_stage0_vs_breakall(int frame)
{
    static int at = -2;
    if (at == -2) {
        const char *e = std::getenv("SM64DS_VS_BREAKALL");
        at = e ? std::atoi(e) : -1;
    }
    if (at < 0 || frame != at) return;

    struct Node { void *x0; Node *next; char **x8; };
    for (Node *nd = *(Node **)&data_0209b468; nd; nd = nd->next) {
        char *a = (char *)nd->x8;
        if (!a) continue;
        unsigned short cls = *(unsigned short *)(a + 0xc);
        if (cls == 178)
            std::fprintf(stderr, "[breakall] f%d star %p uid=%08x "
                         "marker-uid=%08x idx=%u st=%d a2=%04x\n", frame,
                         (void *)a, *(unsigned int *)(a + 4),
                         *(unsigned int *)(a + 0x434),
                         (unsigned)*(unsigned char *)(a + 0x49d),
                         *(int *)(a + 0x440),
                         (unsigned)*(unsigned short *)(a + 0x4a2));
        else if (cls == 180)
            std::fprintf(stderr, "[breakall] f%d marker %p uid=%08x state=%d "
                         "flags=%02x countdown=%u\n", frame, (void *)a,
                         *(unsigned int *)(a + 4),
                         (int)*(unsigned char *)(a + 0x1d8),
                         (unsigned)*(unsigned char *)(a + 0x1db),
                         (unsigned)*(unsigned short *)(a + 0x1d4));
    }
    for (Node *nd = *(Node **)&data_0209b468; nd; nd = nd->next) {
        char *a = (char *)nd->x8;
        if (!a || *(unsigned short *)(a + 0xc) != 180) continue;
        if (*(unsigned char *)(a + 0x1d8) == 0) continue;
        std::fprintf(stderr, "[breakall] f%d breaking marker %p (uid %08x)\n",
                     frame, (void *)a, *(unsigned int *)(a + 4));
        func_ov002_020e7d84(a);
    }
}

/* STAGE 0 (temporary): the VS MATCH CLOCK, which is what actually ends a
   versus match on the DS. HUD::UpdateVsTimer counts this+0x60 down and sets
   data_0209f204 when it reaches zero; Stage::Behavior then does
   Scene::StartSceneFade(7, 0, 0) -- the results screen. Reads only. */
extern "C" void port_stage0_vs_timer(int frame)
{
    if (!std::getenv("SM64DS_VS_SCORE")) return;
    char *hud = (char *)find_actor_by_class(0x14e);
    static int last = -1;
    int clock = hud ? (int)*(unsigned short *)(hud + 0x60) : -1;
    if (clock == last && frame % 600 != 0) return;
    last = clock;
    std::fprintf(stderr, "[vstimer] f%d hud=%p clock=%d sub=%u timeup=%u "
                 "wireless_state=%d countdown=%u\n", frame, (void *)hud, clock,
                 (unsigned)data_ov002_02111188, (unsigned)data_0209f204,
                 data_0209fc68, (unsigned)data_0209f2bc);
}
/* walk the list (Node{void*x0; Node*next@4; int*x8@8}) for the first actor
   whose class word (+0xc) equals `cls`; return the actor (node+8) or 0. */
static void *find_actor_by_class(unsigned short cls)
{
    struct Node { void *x0; Node *next; char **x8; };
    Node *n = *(Node **)&data_0209b468;
    while (n) {
        char *a = (char *)n->x8;   /* node->x8 IS the actor pointer (node+8) */
        if (a && *(unsigned short *)(a + 0xc) == cls)
            return a;
        n = n->next;
    }
    return 0;
}
/* TEMPORARY star-touch trigger, the STAR half of the same seam the buddy, the
 * sign and the rabbit already stand in for.
 *
 * PowerStar state 4 (func_ov002_020ea420) ends every frame by calling the real
 * touch gate func_ov002_020e930c, which reads the star's own two collision
 * fields -- +0x134, the unique id of the actor on its cylinder, and +0x130 bit
 * 0x400000, the touch flag -- finds that actor, checks it is not already in a
 * no-control state and that Event 0x1e is clear, and then runs the REAL
 * func_ov002_020e8ef0. The port does not drive that cylinder overlap for a
 * headless run (the player walks a scripted line, not into a spawned star), so
 * this stands in for JUST the detection. Everything from func_ov002_020e930c
 * down -- the collect handler, SetNoControlState, PrepareTalk, the star's own
 * state machine, the message box -- is the matched code in the ROM's own order.
 *
 *   SM64DS_STAR_TRIGGER=<frame>[,<frame>...]   collect a star on each frame
 *
 * A frame with no live PowerStar puts one there first, through the level's own
 * debug spawn (port_debug_spawn, the same entry SM64DS_SPAWN_ACTOR uses), and
 * arms it two frames later once its InitResources has run. That is what makes a
 * SECOND star in one session testable: the env spawn fires once at the process
 * boot, so a star collected before a level change leaves nothing to collect
 * after one.
 *
 * Each arm is ONE frame only: the gate is edge-shaped (Event 0x1e latches after
 * the first collect) and re-arming would re-detect a star already inside its
 * collect sequence. Delete with the probe when the star's cylinder overlap is
 * driven headlessly.
 */
extern "C" void *port_debug_spawn(unsigned id, unsigned param);

static int star_trigger_wants(int frame)
{
    const char *e = std::getenv("SM64DS_STAR_TRIGGER");
    if (!e) return 0;
    for (;;) {
        char *end;
        long at = std::strtol(e, &end, 0);
        if (end == e) break;
        if (at <= 0) at = 120;
        if (frame == (int)at) return 1;
        e = end;
        if (*e == ',') ++e; else break;
    }
    return 0;
}

/* TEMPORARY read-only readout for the VS scoring investigation.
 *
 *   SM64DS_VS_STARS=1
 *
 * Walks the live actor list and prints every PowerStar (class 178/0xb2 and the
 * silver 0xb3) and every StarMarker (class 180/0xb4) with the exact fields the
 * state-9 wake-up path reads, plus the arena's running star order. Prints only
 * when something changes, so a stalled arena prints once and then goes quiet.
 * Writes nothing. */
extern "C" {
extern unsigned char *data_0209f344;    /* the arena's star running order */
extern unsigned char data_0209f208;     /* how far into that order we are */
extern unsigned char data_0209f2d8;     /* 1 = VS mode */
extern signed char data_0209f310[];     /* per-player VS star counts */
extern signed char NumVsStarsObtained(void);
}

extern "C" void port_vs_stars_probe(int frame)
{
    if (!std::getenv("SM64DS_VS_STARS")) return;
    struct Node { void *x0; Node *next; char **x8; };
    char buf[1400];
    int w = 0;
    buf[0] = 0;
    Node *n = *(Node **)&data_0209b468;
    while (n && w < 1100) {
        char *a = (char *)n->x8;
        if (a) {
            unsigned cls = *(unsigned short *)(a + 0xc);
            if (cls == 0xb2 || cls == 0xb3) {
                w += std::snprintf(buf + w, sizeof buf - (unsigned)w,
                    " STAR{p=%p uid=%08x cls=%03x kind=%d st=%d st444=%d "
                    "idx=%u a2=%04x mk=%08x}",
                    (void *)a, *(unsigned *)(a + 4), cls,
                    *(int *)(a + 0x43c), *(int *)(a + 0x440),
                    *(int *)(a + 0x444), (unsigned)*(unsigned char *)(a + 0x49d),
                    (unsigned)*(unsigned short *)(a + 0x4a2),
                    *(unsigned *)(a + 0x434));
            } else if (cls == 0xb4) {
                w += std::snprintf(buf + w, sizeof buf - (unsigned)w,
                    " MARK{p=%p uid=%08x par=%08x mState=%u mStarID=%u "
                    "mFlags=%02x timer=%u spawnedID=%08x ec=%08x}",
                    (void *)a, *(unsigned *)(a + 4), *(unsigned *)(a + 8),
                    (unsigned)*(unsigned char *)(a + 0x1d8),
                    (unsigned)*(unsigned char *)(a + 0x1d9),
                    (unsigned)*(unsigned char *)(a + 0x1db),
                    (unsigned)*(unsigned short *)(a + 0x1d4),
                    *(unsigned *)(a + 0x1cc), *(unsigned *)(a + 0xec));
            }
        }
        n = n->next;
    }
    unsigned ordv = 0;
    if (data_0209f344)
        for (int i = 0; i < 5; ++i) ordv = ordv * 16 + (data_0209f344[i] & 0xf);
    static unsigned last_hash = 0xffffffffu;
    unsigned h = (unsigned)w * 2654435761u + ordv;
    for (int i = 0; i < w; ++i) h = h * 31u + (unsigned char)buf[i];
    if (h == last_hash) return;
    last_hash = h;
    std::fprintf(stderr, "[vsstar] f%d mode=%u ordptr=%p ordidx=%u order=%05x "
                 "scores=%d,%d,%d,%d sum=%d |%s\n",
                 frame, (unsigned)data_0209f2d8, (void *)data_0209f344,
                 (unsigned)data_0209f208, ordv,
                 (int)data_0209f310[0], (int)data_0209f310[1],
                 (int)data_0209f310[2], (int)data_0209f310[3],
                 (int)NumVsStarsObtained(), w ? buf : " (no stars, no markers)");
}

extern "C" void port_input_probe_star_trigger(int frame)
{
    static int arm_at = -1;
    int want = star_trigger_wants(frame);
    if (!want && frame != arm_at) return;

    char *player = (char *)find_actor_by_class(0xbf);
    /* STAGE 0: prefer a star the touch gate is actually reachable from. The
       gate func_ov002_020e930c is only called out of PowerStar state 4
       (func_ov002_020ea420) and state 8 (func_ov002_020e99e8); a VS arena's
       four table stars are type 3 and park in state 9, where nothing reads
       the words written below, and one of them is what find_actor_by_class
       hands back. Fall back to the first star so single-player behaviour is
       unchanged. */
    char *star = 0;
    {
        struct Node { void *x0; Node *next; char **x8; };
        Node *nd = *(Node **)&data_0209b468;
        while (nd) {
            char *a = (char *)nd->x8;
            if (a && *(unsigned short *)(a + 0xc) == 178) {
                const int st = *(int *)(a + 0x440);
                if (st == 4 || st == 8) { star = a; break; }
            }
            nd = nd->next;
        }
    }
    if (!star) star = (char *)find_actor_by_class(178);
    if (!player) {
        std::fprintf(stderr, "  [startrig] f%d no player to collect with\n",
                     frame);
        return;
    }
    if (!star) {
        if (frame == arm_at) {
            std::fprintf(stderr, "  [startrig] f%d the spawned star is not on "
                         "the list; nothing armed\n", frame);
            arm_at = -1;
            return;
        }
        void *made = port_debug_spawn(178, 0);
        arm_at = frame + 2;
        std::fprintf(stderr, "  [startrig] f%d no live PowerStar: spawned one "
                     "(%p) through the level's own debug spawn; arming its "
                     "touch at f%d\n", frame, made, arm_at);
        return;
    }
    if (frame == arm_at)
        arm_at = -1;
    unsigned int pid = *(unsigned int *)(player + 0x4);   /* player's unique id */
    *(unsigned int *)(star + 0x134) = pid;
    *(unsigned int *)(star + 0x130) |= 0x400000;
    std::fprintf(stderr, "  [startrig] f%d armed the touch: star %p (kind=%d "
                 "state=%d idx=%u) +0x134 = "
                 "player uid 0x%x, +0x130 |= 0x400000; func_ov002_020e930c "
                 "runs the real collect handler this frame\n",
                 frame, (void *)star, *(int *)(star + 0x43c),
                 *(int *)(star + 0x440),
                 (unsigned)*(unsigned char *)(star + 0x49d), pid);
}

extern "C" void port_input_probe_buddy_trigger(int frame)
{
    if (!std::getenv("SM64DS_BUDDY_TRIGGER")) return;
    if (frame < 60) return;
    char *buddy = (char *)find_actor_by_class(181);
    char *player = (char *)find_actor_by_class(0xbf);
    if (!buddy || !player) return;
    unsigned int pid = *(unsigned int *)(player + 0x4);   /* player's unique id */
    static int entered, staged2;
    /* Arm the buddy's clsn-detect fields ONLY until the talk is entered. Re-arming
       every frame made his own state-0 main re-detect the player after each talk
       closed and loop the whole talk over -- a headless artifact of standing in
       for the collision cylinder. Once entered, leave the fields alone so the
       buddy walks his real post-talk states. */
    if (!entered) {
        *(unsigned int *)(buddy + 0xf4) |= 0x8000000;
        *(unsigned int *)(buddy + 0xf8) = pid;
    }
    static int announced;
    if (!announced) {
        announced = 1;
        std::fprintf(stderr, "  [buddy] f%d armed talk detection: buddy %p, "
                     "player %p (uid 0x%x); state-0 main will run the real "
                     "Player::StartTalk\n", frame, (void *)buddy,
                     (void *)player, pid);
    }

    /* Drive the entry ONCE, in the right order. On the DS the buddy's Behavior
       runs before the player consumes the button, so StartTalk sees ST_WAIT +
       A-pressed and enters ST_TALK. In the port the player node runs first, so
       here (before the actor tick) we set the A-pressed bit and call the buddy's
       OWN state-0 main directly: it runs the REAL Player::StartTalk on the real
       player while he is still in ST_WAIT. Once the talk is entered this stops.
       Nothing downstream is faked -- the buddy's state machine, the message and
       OpenCannonInCurLevel are all the matched code, driven in the ROM's order. */
    if (!entered && _ZN6Player12GetTalkStateEv(player) < 0) {
        int idx = (int)data_020a0e40; if (idx < 0 || idx > 3) idx = 0;
        *(unsigned short *)(data_0209f49e + idx * 0x18) |= 0x1;  /* A pressed */
        func_ov084_0212c8b0(buddy);   /* real state-0 main -> real StartTalk */
        if (_ZN6Player12GetTalkStateEv(player) >= 0) {
            entered = 1;
            std::fprintf(stderr, "  [buddy] f%d StartTalk ENTERED the talk: "
                         "GetTalkState=%d\n", frame,
                         _ZN6Player12GetTalkStateEv(player));
        }
    }
    /* THE CAMERA TEARDOWN IS THE CAMERA'S NOW. St_Talk's exit step 6 waits on
       cam+0x154 bit 15 (talk mode); on the DS the Camera clears it in its own
       Camera::Behavior (block_7: when bit 3 is set, if bits 0xc000 are up it
       clears them and ChangeStates back). That path runs on the host -- proven
       by the cam154 trace going 0x4000 -> 0x8000 -> 0 while the player sits at
       pstep 6, after which GetTalkState -> -1 on its own. The old stand-in
       force-cleared bit 15 from here; that latch-clear is DELETED. The one piece
       still driven is the buddy's own approach: his state machine reaches the
       OpenCannon state (func_ov084_0212c1a0) only after a turn-to-face + walk-back
       ANIMATION the headless degenerate spawn does not play, so once the talk is
       genuinely over (GetTalkState < 0, the camera having cleared the latch) the
       buddy is handed to that state through his OWN ChangeState. Its main then
       walks GetTalkState 0->2->-1 and calls the matched OpenCannonInCurLevel().
       The talk machine, the camera teardown, GetTalkState and the cannon write
       are all real code -- only the buddy's missing approach animation is stood
       in for. */
    if (entered && std::getenv("SM64DS_BUDDY_OPENCANNON")) {
        /* once the talk is fully over -- the CAMERA having cleared bit 15 on its
           own -- hand the buddy to his OpenCannon state (its main opens it) */
        if (!staged2 && _ZN6Player12GetTalkStateEv(player) < 0) {
            staged2 = 1;
            func_ov084_0212c960(buddy, 2);
            std::fprintf(stderr, "  [buddy] f%d talk over (camera cleared the "
                         "talk latch itself), buddy -> OpenCannon state; its main "
                         "runs the real OpenCannonInCurLevel\n", frame);
        }
    }
    if (std::getenv("SM64DS_TRACE_BUDDY")) {
        /* buddy state index +0x1e4, player state ptr +0x370, GetTalkState, and
           the player pressed-button word StartTalk's b==0 gate reads
           (data_0209f49e + idx*0x18 & 3) */
        extern int _ZN6Player12GetTalkStateEv(void *p);
        extern unsigned char data_0209f49e[];
        extern unsigned char data_020a0e40;
        int bstate = *(int *)(buddy + 0x1e4);
        void *pstate = *(void **)(player + 0x370);
        unsigned pressed = *(unsigned short *)(data_0209f49e +
                                               (int)data_020a0e40 * 0x18);
        int pstep = *(unsigned char *)(player + 0x6e3);   /* mStateStep */
        extern char *data_0209f318;   /* the Camera singleton */
        unsigned camflags = data_0209f318
            ? *(unsigned int *)(data_0209f318 + 0x154) : 0;
        std::fprintf(stderr, "  [buddy] f%d bstate=%d bsub=%d pstep=%d "
                     "talkstate=%d msg=%d pressed=%03x&3=%d cam154=%08x\n", frame,
                     bstate, *(int *)(buddy + 0x1e8), pstep,
                     _ZN6Player12GetTalkStateEv(player), (int)data_0209d660,
                     pressed, pressed & 3, camflags);
    }
}

/* TEMPORARY sign talk-entry proof, the SIGN half of the same StartTalk seam.
 *
 * The sign's planted-state main (func_ov002_020bb9fc) calls func_ov002_020bb520,
 * which reads the sign's own talk-trigger fields -- +0x344 (the id of the actor
 * on its read trigger), +0x340 & 0x8000000 (the trigger-active flag) and +0x58e
 * (the read-ready flag) -- confirms that actor is the player (0xbf), checks the
 * player is facing the sign, and calls the REAL Player::StartTalk. Those trigger
 * fields are set by the sign's collision cylinder detecting the player, which
 * the port does not drive for a headless spawn-on-top, so this stands in for
 * JUST that detection: it sets the three fields to name the real player and
 * calls func_ov002_020bb520 directly, which runs the real StartTalk. The sign
 * then enters its read state and hands the player to ST_TALK exactly as the
 * buddy does. Everything from func_ov002_020bb520 down is matched code.
 *
 *   SM64DS_SIGN_TRIGGER=1  -- arm the sign's talk from frame 60 on.
 */
extern "C" {
extern int func_ov002_020bb520(void *sign);   /* the sign's planted talk check */
}
extern "C" int Vec3_HorzDist(void *a, void *b);
static int port_sign_prev_dist;

extern "C" void port_input_probe_sign_trigger(int frame)
{
    if (!std::getenv("SM64DS_SIGN_TRIGGER")) return;
    if (frame < 60) return;
    static int entered;
    /* TEMPORARY sign read-state trace: once the talk is entered, watch the
       message-active flag, the box state and GetTalkState each frame, so a
       headless run can see the box open, scroll and close. */
    if (std::getenv("SM64DS_TRACE_SIGN")) {
        char *sg = (char *)find_actor_by_class(184);
        char *pl = (char *)find_actor_by_class(0xbf);
        /* +0x58d is the read state's own sub-state (0 turn, 1 walk, 2 face and
           animate) and the player's yaw at +0x8e is what the loop drives. Both
           are needed to tell "waiting for the box", which is correct, from
           "turning him forever", which is the reported soft-lock. */
        if (sg && pl) {
            int d = Vec3_HorzDist(pl + 0x5c, sg + 0x5c);
            /* `step` is how much of the 0xa000 the walk ACTUALLY achieved last
               frame. Vec3_ApproachHorz writes the player's x/z directly and is
               collision-blind, so the ONLY way it can fail to arrive is
               something writing his position back. A step short of 0xa000 is
               that happening, and it is the whole hazard in one number. */
            std::fprintf(stderr,
                "  [sign] f%d state=%d sub=%d talk=%d msgActive=%d box=%d "
                "yaw=%04x dist=%d step=%d spd=%d sign=(%d,%d,%d) param=0x%x\n",
                frame, *(int *)(sg + 0x354),
                (int)*(unsigned char *)(sg + 0x58d),
                _ZN6Player12GetTalkStateEv(pl), (int)data_0209d660,
                (int)data_0209d6bc,
                (unsigned)(unsigned short)*(short *)(pl + 0x8e),
                d, port_sign_prev_dist ? port_sign_prev_dist - d : 0,
                /* the player's own horizontal speed. Every earlier approach
                   started from a standing start, so this was 0 and the walk
                   had nothing to contend with -- which is why "nothing fights
                   the write" was measured on a case that could not fight. */
                *(int *)(pl + 0x98),
                *(int *)(sg + 0x5c) >> 12, *(int *)(sg + 0x60) >> 12,
                *(int *)(sg + 0x64) >> 12, *(int *)(sg + 8));
            port_sign_prev_dist = d;
        }
    }
    if (entered) return;
    char *sign = (char *)find_actor_by_class(184);
    char *player = (char *)find_actor_by_class(0xbf);
    if (!sign || !player) return;
    if (_ZN6Player12GetTalkStateEv(player) >= 0) { entered = 1; return; }
    unsigned int pid = *(unsigned int *)(player + 0x4);
    /* name the player on the sign's read trigger and mark it active + ready */
    *(unsigned int *)(sign + 0x344) = pid;
    *(unsigned int *)(sign + 0x340) |= 0x8000000;
    *(unsigned char *)(sign + 0x58e) = 1;
    /* the read-state main reads the player pointer from the sign at +0x598 */
    *(unsigned int *)(sign + 0x598) = (unsigned int)(size_t)player;
    /* face the sign toward the player so the 0x4000 angle gate in 020bb520
       passes (its check is AngleDiff(signYaw, angToPlayer) <= 0x4000) */
    extern short Vec3_HorzAngle(void *a, void *b);
    *(short *)(sign + 0x8e) = Vec3_HorzAngle(sign + 0x5c, player + 0x5c);
    int idx = (int)data_020a0e40; if (idx < 0 || idx > 3) idx = 0;
    *(unsigned short *)(data_0209f49e + idx * 0x18) |= 0x1;   /* A pressed */
    int r = func_ov002_020bb520(sign);   /* -> real StartTalk */
    if (_ZN6Player12GetTalkStateEv(player) >= 0) {
        entered = 1;
        std::fprintf(stderr, "  [sign] f%d func_ov002_020bb520 returned %d, "
                     "StartTalk ENTERED the talk: GetTalkState=%d\n", frame, r,
                     _ZN6Player12GetTalkStateEv(player));
    }
}

/* DS KEYINPUT bit layout: bit0 A, bit1 B, bit2 Select, bit3 Start, bit4 Right,
   bit5 Left, bit6 Up, bit7 Down, bit10 X, bit11 Y, bit8 L, bit9 R. */
enum {
    KEY_A = 0x0001, KEY_B = 0x0002, KEY_SELECT = 0x0004, KEY_START = 0x0008,
    KEY_RIGHT = 0x0010, KEY_LEFT = 0x0020, KEY_UP = 0x0040, KEY_DOWN = 0x0080,
    KEY_L = 0x0100, KEY_R = 0x0200, KEY_X = 0x0400, KEY_Y = 0x0800
};

struct ProbeStep { int frame0, frame1; unsigned short bits; };

static ProbeStep g_steps[64];
static int g_step_count = -1;   /* -1 = not parsed yet, 0 = off */

static unsigned short name_to_bit(const char *tok, int len)
{
    /* one token, already trimmed; compare case-insensitively */
    char b[8];
    int n = 0;
    for (int i = 0; i < len && n < 7; ++i) b[n++] = (char)std::toupper((unsigned char)tok[i]);
    b[n] = 0;
    if (!std::strcmp(b, "A")) return KEY_A;
    if (!std::strcmp(b, "B")) return KEY_B;
    if (!std::strcmp(b, "X")) return KEY_X;
    if (!std::strcmp(b, "Y")) return KEY_Y;
    if (!std::strcmp(b, "L")) return KEY_L;
    if (!std::strcmp(b, "R")) return KEY_R;
    if (!std::strcmp(b, "U") || !std::strcmp(b, "UP")) return KEY_UP;
    if (!std::strcmp(b, "D") || !std::strcmp(b, "DOWN")) return KEY_DOWN;
    if (!std::strcmp(b, "LEFT")) return KEY_LEFT;
    if (!std::strcmp(b, "RIGHT")) return KEY_RIGHT;
    if (!std::strcmp(b, "START")) return KEY_START;
    if (!std::strcmp(b, "SELECT")) return KEY_SELECT;
    std::fprintf(stderr, "  [inprobe] unknown button \"%s\"\n", b);
    return 0;
}

static void parse_once(void)
{
    g_step_count = 0;
    const char *s = std::getenv("SM64DS_PROBE_INPUT");
    if (!s || !*s) return;
    /* comma-separated frame[:-frame1]:BTN[+BTN...] */
    while (*s && g_step_count < 64) {
        char *end;
        long f0 = std::strtol(s, &end, 10);
        if (end == s) { std::fprintf(stderr, "  [inprobe] bad frame at \"%s\"\n", s); return; }
        long f1 = f0;
        s = end;
        if (*s == '-') { f1 = std::strtol(s + 1, &end, 10); s = end; }
        if (*s != ':') { std::fprintf(stderr, "  [inprobe] expected ':' at \"%s\"\n", s); return; }
        ++s;
        /* button group up to ',' */
        unsigned short bits = 0;
        while (*s && *s != ',') {
            const char *tokstart = s;
            while (*s && *s != ',' && *s != '+') ++s;
            bits |= name_to_bit(tokstart, (int)(s - tokstart));
            if (*s == '+') ++s;
        }
        g_steps[g_step_count].frame0 = (int)f0;
        g_steps[g_step_count].frame1 = (int)f1;
        g_steps[g_step_count].bits = bits;
        ++g_step_count;
        if (*s == ',') ++s;
    }
    std::fprintf(stderr, "  [inprobe] SM64DS_PROBE_INPUT: %d scripted press%s\n",
                 g_step_count, g_step_count == 1 ? "" : "es");
    for (int i = 0; i < g_step_count; ++i)
        std::fprintf(stderr, "  [inprobe]   f%d..%d bits 0x%03x\n",
                     g_steps[i].frame0, g_steps[i].frame1, g_steps[i].bits);
}

/* the DS pad bits scripted for this frame, or 0 if none. Public so the harness
   can also fold them into its own button word if a consumer reads the remapped
   Ctrl block rather than the raw mirror. */
extern "C" unsigned short port_input_probe_bits(int frame)
{
    if (g_step_count < 0) parse_once();
    unsigned short bits = 0;
    for (int i = 0; i < g_step_count; ++i)
        if (frame >= g_steps[i].frame0 && frame <= g_steps[i].frame1)
            bits |= g_steps[i].bits;
    return bits;
}

/* OR the scripted bits into the raw DS pad mirror for the local player, both
   the held word (data_020a0e58[idx*2]) and the pressed word (both the mirror's
   pressed field data_020a0e58[idx*2+1] AND the split symbol data_020a0e5a[idx*2]
   the message code reads). Call once per frame BEFORE Stage::CheckInput so the
   remap and the direct readers both see the press. The pressed word is an edge:
   only ON the first frame of a hold does it carry the bit, matching hardware. */
extern "C" void port_input_probe_apply(int frame)
{
    if (g_step_count < 0) parse_once();
    if (g_step_count == 0) return;
    unsigned short now = port_input_probe_bits(frame);
    unsigned short before = port_input_probe_bits(frame - 1);
    unsigned short edge = (unsigned short)(now & ~before);
    if (!now && !edge) return;

    int idx = (int)data_020a0e40;
    if (idx < 0 || idx > 3) idx = 0;

    data_020a0e58[idx * 2] |= now;         /* held */
    data_020a0e58[idx * 2 + 1] |= edge;    /* pressed (edge) */
    /* the split symbol the message box / IsButtonInputValid read: same DS
       memory as the mirror's pressed field, decoupled on host, so mirror it */
    *(unsigned short *)((char *)data_020a0e5a + idx * 4) |= edge;
}

/* ===================================================================
 * SPIN INVESTIGATION PROBES (env-gated, test rig only).
 *
 * 1. SM64DS_ALCHECK=1
 *    Calls the LINKED C-linkage face _Z14ApproachLinearRsss with angle
 *    pairs the ROM's own ApproachLinear converges on in <= 41 frames, and
 *    reports how many frames the linked face actually takes. This is the
 *    negative control: it FAILS on a tree where the face is mis-seated.
 *
 * 2. SM64DS_SIGN_YAW=<decimal>
 *    Once the sign's read state is entered, hold the sign's yaw (+0x8e) at
 *    this value every frame, so a headless run can model a sign whose
 *    PLACEMENT yaw is the given value. The existing trigger overwrites
 *    +0x8e with "point at the player", which destroys the one input the
 *    read loop's sub-2 target is built from (target = yaw + 0x8000).
 *
 * 3. SM64DS_CHOMP_FREE=<frame>, SM64DS_TRACE_CHOMP=1
 *    Force the chain chomp's break-free transition by calling the ROM's own
 *    func_ov014_02111f54 (the same call state 1 and state 2 make every
 *    frame), and trace its state machine. Reproduces the outcome of pounding
 *    the post without needing to navigate to it.
 * =================================================================== */
extern "C" {
extern int _Z14ApproachLinearRsss(short *x, short target, short step);
extern int func_ov014_02111f54(void *chomp);
extern short _ZN5Actor18HorzAngleToCPlayerEv(void *self);
}

extern "C" void port_probe_alcheck(void)
{
    if (!std::getenv("SM64DS_ALCHECK")) return;
    static int done; if (done) return; done = 1;
    /* {start, target, step, ROM frames} -- ROM figures from an exhaustive
       sweep of src/_Z14ApproachLinearRsss.cpp over all 65536 start angles. */
    static const struct { short x, t, s; int rom; const char *who; } K[] = {
        {  0x0000, (short)0x8000, 0x800, 16, "SIGN  read sub-2, sign yaw 0"    },
        {  0x4000, (short)0x8000, 0x800, 16, "SIGN  read sub-2, sign yaw 0"    },
        { (short)0xC000, (short)0x8000, 0x800, 16, "SIGN read sub-2, yaw 0"    },
        {  0x1000, (short)0x8200, 0x800, 16, "SIGN  read sub-2, sign yaw 512"  },
        {  0x0000,        0x7fff, 0x320, 41, "CHOMP free sub-0, player at 180" },
    };
    int fails = 0;
    std::fprintf(stderr, "[alcheck] linked _Z14ApproachLinearRsss:\n");
    for (unsigned i = 0; i < sizeof(K)/sizeof(K[0]); ++i) {
        short x = K[i].x; int f = 0;
        for (f = 1; f <= 4000; ++f)
            if (_Z14ApproachLinearRsss(&x, K[i].t, K[i].s)) break;
        int bad = (f > 4000) || (f > K[i].rom);
        if (bad) ++fails;
        std::fprintf(stderr,
            "[alcheck]   %-34s x=%6d t=%6d step=%4d : ROM %2d fr, linked %s -> %s\n",
            K[i].who, K[i].x, K[i].t, K[i].s, K[i].rom,
            (f > 4000) ? "NEVER CONVERGED" : "converged", bad ? "FAIL" : "ok");
    }
    std::fprintf(stderr, "[alcheck] RESULT: %s (%d of %d failed)\n",
                 fails ? "FAIL" : "PASS", fails, (int)(sizeof(K)/sizeof(K[0])));
}

extern "C" void port_probe_sign_yaw(void)
{
    const char *e = std::getenv("SM64DS_SIGN_YAW");
    if (!e) return;
    char *sg = (char *)find_actor_by_class(184);
    if (!sg) return;
    if (*(int *)(sg + 0x354) != 1) return;      /* only while in the read state */
    *(short *)(sg + 0x8e) = (short)std::atoi(e);
}

extern "C" void port_probe_chomp(int frame)
{
    char *ch = (char *)find_actor_by_class(219);
    if (!ch) return;
    /* the post the chomp spawned and chained itself to: its id is at +0x608.
       func_ov014_02111f54 (the break-free, called by state 1 AND state 2 every
       frame) refuses while the post's +0x31e is non-zero, so that byte is the
       pounded-the-post gate. Clearing it is what pounding it does. */
    char *post = (char *)_ZN5Actor10FindWithIDEj(*(unsigned *)(ch + 0x608));
    const char *fr = std::getenv("SM64DS_CHOMP_FREE");
    if (fr && post && frame >= std::atoi(fr) && *(unsigned char *)(post + 0x31e)) {
        std::fprintf(stderr, "  [chomp] f%d post +0x31e %d -> 0 (post pounded)\n",
                     frame, (int)*(unsigned char *)(post + 0x31e));
        *(unsigned char *)(post + 0x31e) = 0;
    }
    if (!std::getenv("SM64DS_TRACE_CHOMP")) return;
    /* print only when something changes, plus a heartbeat */
    static int p_state = -99, p_sub = -99; static int last;
    int st = *(int *)(ch + 0x610), sub = (int)*(unsigned char *)(ch + 0x604);
    if (st == p_state && sub == p_sub && frame - last < 60) return;
    p_state = st; p_sub = sub; last = frame;
    std::fprintf(stderr,
        "  [chomp] f%d state=%d sub=%d yaw=%6d tgtAng=%6d landed=%d "
        "y=%d floor=%d freed=%d post31e=%d\n",
        frame, st, sub, (int)*(short *)(ch + 0x8e),
        (int)_ZN5Actor18HorzAngleToCPlayerEv(ch),
        (int)*(unsigned char *)(ch + 0x61c),
        *(int *)(ch + 0x60) >> 12, *(int *)(ch + 0x5f0) >> 12,
        (int)*(unsigned char *)(ch + 0x605),
        post ? (int)*(unsigned char *)(post + 0x31e) : -1);
}

/* TEMPORARY rabbit-catch trigger, the RABBIT half of the same seam the buddy
 * and the sign already stand in for.
 *
 * func_ov085_0212a828 is the rabbit's own grab check, called unconditionally
 * from Rabbit::Behavior (src/_ZN6Rabbit8BehaviorEv.c:204, reloc 0x0212c778 ->
 * 0x0212a828). It reads the rabbit's grab-trigger fields -- +0x134 (the id of
 * the actor on its collision cylinder) and +0x130 & 0x1000 (the trigger flag) --
 * confirms that actor is the player (0xbf), runs the REAL Player::TryGrab, and
 * on success writes the player at rabbit+0x45c and hands the rabbit to its
 * caught state. Those two trigger fields are set by the collision cylinder
 * detecting a punch, which the port does not drive for a headless run, so this
 * stands in for JUST that detection. Everything from func_ov085_0212a828 down
 * -- TryGrab, the +0x45c write, the caught dialogue, the key spawn -- is the
 * matched code, in the ROM's own order.
 *
 *   SM64DS_RABBIT_TRIGGER=<frame>   arm from that frame (default 60)
 *
 * It also parks the rabbit on the player, because the headless player never
 * walks to it, and it reports the refusal reason on the first few attempts so a
 * run that cannot grab says why instead of going quiet.
 */
extern "C" {
extern void func_ov085_0212a828(void *rabbit);
extern int _ZN6Player7IsStateERNS_5StateE(void *p, void *st);
extern char data_ov002_02110574, data_ov002_0211067c, data_ov002_021105bc;
/* the four gates St_HoldLight_Main's carry-bit write sits behind */
extern int func_ov002_020c0434(void *p);
extern int _ZN6Player6IsAnimEj(void *p, unsigned int anim);
extern int _ZN6Player12FinishedAnimEv(void *p);
extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void *p, unsigned int a, char b);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int f);
/* the nine Player::State objects StartTalk's b==1 path tests */
extern char data_ov002_0211046c, data_ov002_0211013c, data_ov002_02110154,
            data_ov002_021104e4, data_ov002_02110514, data_ov002_02110364,
            data_ov002_02110424, data_ov002_021105a4, data_ov002_0211043c;
/* the rabbit's four caught-side State objects */
extern char data_ov085_021306ac, data_ov085_021306bc, data_ov085_021306cc,
            data_ov085_0213068c;
/* the caught state's own pair: Init 0x0212b3fc, Main 0x0212ae08. Printing their
   host addresses beside the words the live State object carries is what tells a
   "the dialogue refused" reading apart from "a different function is seated". */
extern int func_ov085_0212ae08(char *c);
extern int func_ov085_0212b3fc(char *c);
/* the two globals Rabbit::Behavior's own top-of-body gate reads */
extern unsigned char data_0209f2f8;   /* the current level word */
extern int data_0209caa0[];           /* the save/global flag block */
}

extern "C" void port_probe_rabbit_trigger(int frame)
{
    const char *e = std::getenv("SM64DS_RABBIT_TRIGGER");
    if (!e) return;
    int from = std::atoi(e);
    if (from <= 0) from = 60;
    if (frame < from) return;

    char *rb = (char *)find_actor_by_class(187);
    char *player = (char *)find_actor_by_class(0xbf);
    if (!rb || !player) return;

    /* THE RABBIT-HUNT-IS-ON FLAG, the second half of this stand-in.
     *
     * Rabbit::Behavior returns before its own state dispatch unless
     * data_0209caa0[2] & 0x20000 is set (src/_ZN6Rabbit8BehaviorEv.c:50-65). On
     * a level other than 0x32 the rabbit sits on the +0x428 == 1 arm and only
     * leaves it once that bit is on, so with the bit clear the actor is inert:
     * measured, its Animation at +0x350 (30 frames, speed 0x1000) never advances
     * across a 900-frame run, before or after a grab.
     *
     * The ROM's writer of the bit is func_ov085_0212e2ec, the LakituBro state
     * seated at 0x021302a4 (ov085 relocs.txt:1238) -- the cameraman's intro that
     * starts the rabbit hunt. Driving that whole actor is a separate chain and
     * has nothing to do with the timing question this rig asks, so the trigger
     * sets the bit directly, exactly as walk_window.cpp:6149 already seats
     * data_0209caa0[2] |= 0x80 for its own stand-in. This makes the rabbit LIVE;
     * everything after it -- the grab, the caught dialogue, the talk, the key
     * spawn -- is the matched code in the ROM's own order. */
    if ((data_0209caa0[2] & 0x20000) == 0) {
        data_0209caa0[2] |= 0x20000;
        std::fprintf(stderr, "  [rabbit] f%d set data_0209caa0[2] |= 0x20000 "
                     "(stands in for the LakituBro intro func_ov085_0212e2ec; "
                     "without it Rabbit::Behavior returns at its own line 53 and "
                     "the actor never ticks)\n", frame);
    }

    /* SM64DS_RABBIT_PARAM / SM64DS_RABBIT_GLOW re-dress this rabbit as a COURSE
     * (glowing) rabbit instead of the castle key rabbit the level table gave it.
     *
     * Both fields are ordinary spawn parameters of this same actor -- +0x43c is
     * the variant word the level's object table supplies (7 is the castle key
     * rabbit; a course rabbit carries its course number) and +0x429 is the
     * glowing marker. Nothing else is touched.
     *
     * WHY IT MATTERS. func_ov085_0212ae08's +43c == 7 arm spawns the key and
     * returns. Every OTHER variant falls through to the tail, where the arm at
     * "flag_path" -- taken when +0x429 != 0 and SaveData::NumGlowingRabbitsFound()
     * != 8 -- spawns the key and IN THE SAME FRAME hands the rabbit to state
     * 0213068c, whose Main func_ov085_0212a904 opens message 0x148 on its very
     * first frame (case 0). The key's own descent needs about ten frames
     * (func_ov085_0212d24c sets +0x100 = 10 and func_ov085_0212d108 only leaves
     * on that countdown), so that second dialogue is up well before the key
     * arrives -- which is the overlap this lane is looking for, reached with no
     * injected key at all. */
    if (const char *pe = std::getenv("SM64DS_RABBIT_PARAM")) {
        static int done;
        if (!done) {
            done = 1;
            *(int *)(rb + 0x43c) = (int)std::strtol(pe, 0, 0);
            std::fprintf(stderr, "  [rabbit] f%d rabbit+0x43c := %d (course "
                         "variant instead of the castle key rabbit)\n",
                         frame, *(int *)(rb + 0x43c));
        }
    }
    if (std::getenv("SM64DS_RABBIT_GLOW")) {
        static int done;
        if (!done) {
            done = 1;
            *(unsigned char *)(rb + 0x429) = 1;
            std::fprintf(stderr, "  [rabbit] f%d rabbit+0x429 := 1 (glowing "
                         "marker)\n", frame);
        }
    }

    static int caught, tries;

    /* SM64DS_RABBIT_REGRAB=1 re-arms the stand-in once the rabbit has been let
     * go again (func_ov085_0212ae08's +43c==7 arm clears rabbit+0x45c in the
     * same breath as it spawns the key). Re-grabbing the rabbit is an ordinary
     * player action -- the rabbit is standing right there and Rabbit::Behavior
     * puts it back on its catchable arm (src/_ZN6Rabbit8BehaviorEv.c:154-162) --
     * and it is the only NATURAL way found so far to put the player back under
     * mIsNoControl while the key is still flying to him. That is the overlap
     * lane C could only reach by injecting a key with SM64DS_KEY_SPAWN_AT. */
    if (caught && *(void **)(rb + 0x45c) == 0 && std::getenv("SM64DS_RABBIT_REGRAB")) {
        static int announced;
        if (!announced) {
            announced = 1;
            std::fprintf(stderr, "  [rabbit] f%d rabbit released (+0x45c = 0), "
                         "re-arming the grab stand-in\n", frame);
        }
        caught = 0;
    }

    if (caught) {
        /* post-grab: follow the rabbit's own caught states, so a run that never
           reaches the key spawn says where it stopped instead of going quiet. */

        /* CARRY-BIT GATE (lane C2). func_ov085_0212ae08.c:65-69 opens the caught
           dialogue only when rabbit+0xb0 & 0x4000 is set, and the ROM's writer of
           that bit is Player::St_HoldLight_Main (ov002 0x020d1a1c,
           src/_ZN6Player17St_HoldLight_MainEv.cpp:61-68), which needs FOUR things
           true on the same frame: not the func_ov002_020c0434 early-out,
           mStateStep == 0, the pickup anim (0x2f or 0x86) playing and not
           finished, and the anim cursor crossing frame 6. This prints all four
           beside the bit so a run that never sets it says WHICH gate it missed
           instead of only that the dialogue stayed shut. Every call here is a
           read: Animation::WillHitFrame is const and pure
           (src/_ZNK9Animation12WillHitFrameEi.cpp), IsAnim/FinishedAnim compare
           fields, and func_ov002_020c0434 is the state's own test. */
        {
            static int hl_left = 60;   /* per-frame for the pickup window */
            if (hl_left > 0) {
                --hl_left;
                int held = *(int *)(player + 0x358) != 0;
                unsigned mdl = _ZNK6Player14GetBodyModelIDEjb(
                                   player, (unsigned)(*(int *)(player + 8) & 0xff), 0);
                char *anim = *(char **)(player + 0xdc + mdl * 4);
                std::fprintf(stderr,
                    "  [hold] f%d st370=%p step6e3=%d held358=%d 0c0434=%d "
                    "isanim2f=%d isanim86=%d finished=%d willhit6=%d rb0xb0=0x%08x "
                    "carried=%d\n",
                    frame, *(void **)(player + 0x370),
                    (int)*(unsigned char *)(player + 0x6e3), held,
                    func_ov002_020c0434(player),
                    _ZN6Player6IsAnimEj(player, 0x2f),
                    _ZN6Player6IsAnimEj(player, 0x86),
                    _ZN6Player12FinishedAnimEv(player),
                    anim ? _ZNK9Animation12WillHitFrameEi(anim + 0x50, 6) : -1,
                    (unsigned)*(int *)(rb + 0xb0),
                    (*(int *)(rb + 0xb0) & 0x4000) != 0);

                /* STARTTALK GATE. With the carry bit set the rabbit's caught
                   dialogue calls Player::StartTalk(pl, c, 1) every frame and
                   only writes rabbit+0x41c when it returns nonzero. StartTalk's
                   b==1 path returns 0 only when the player is in NONE of eight
                   allowed states, or is in 02110424 / 021105a4. Print which of
                   the nine (talk + the eight) the player's +0x370 names, so a
                   refusal says which. Reads only. */
                std::fprintf(stderr,
                    "  [talkgate] f%d p370=%p talk46c=%d s13c=%d s154=%d s4e4=%d "
                    "s514=%d s364=%d s424=%d s5a4=%d s43c=%d noctl709=%d kind70a=%d\n",
                    frame, *(void **)(player + 0x370),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_0211046c),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_0211013c),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_02110154),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_021104e4),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_02110514),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_02110364),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_02110424),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_021105a4),
                    _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_0211043c),
                    (int)*(unsigned char *)(player + 0x709),
                    (int)*(unsigned char *)(player + 0x70a));

                /* WHICH RABBIT STATE IS LIVE. rabbit+0x364 is the State object
                   Rabbit::Behavior dispatches through (its .b pair at +8 is the
                   Main). Only 021306ac's Main is func_ov085_0212ae08, the caught
                   dialogue that spawns the key, so print the four candidates'
                   addresses beside the live pointer and the Main word the
                   dispatch will actually call. */
                if (hl_left == 59) {
                    std::fprintf(stderr,
                        "  [rbstate] 06ac=%p 06bc=%p 06cc=%p 068c=%p\n",
                        (void *)&data_ov085_021306ac, (void *)&data_ov085_021306bc,
                        (void *)&data_ov085_021306cc, (void *)&data_ov085_0213068c);
                }
                {
                    /* +0x350 is the rabbit's Animation (numFramesAndFlags +0x04,
                       currFrame +0x08, speed +0x0c, per include/Animation.h).
                       Rabbit::Behavior calls Animation::Advance on it every
                       tick, so a MOVING currFrame is proof Behavior itself is
                       running and the state Main really is being dispatched --
                       without which every other reading here is vacuous. */
                    int *sp = *(int **)(rb + 0x364);
                    std::fprintf(stderr,
                        "  [rbstate] f%d +364=%p is06ac=%d mainfn=0x%08x "
                        "maindelta=%d animcur=%d animspd=%d anim100=%d\n",
                        frame, (void *)sp,
                        (void *)sp == (void *)&data_ov085_021306ac,
                        sp ? (unsigned)sp[2] : 0u, sp ? sp[3] : 0,
                        *(int *)(rb + 0x350 + 0x08), *(int *)(rb + 0x350 + 0x0c),
                        (int)*(short *)(rb + 0x100));
                    if (hl_left == 59)
                        std::fprintf(stderr,
                            "  [rbstate] &func_ov085_0212ae08=0x%08x "
                            "&func_ov085_0212b3fc=0x%08x animlen=0x%08x\n",
                            (unsigned)(size_t)&func_ov085_0212ae08,
                            (unsigned)(size_t)&func_ov085_0212b3fc,
                            (unsigned)*(int *)(rb + 0x350 + 0x04));
                }
            }
        }

        static void *p_state; static int p_step = -99, p_talk = -99, p_last;
        void *st = *(void **)(rb + 0x364);
        int step = *(int *)(rb + 0x41c);
        int talk = _ZN6Player12GetTalkStateEv(player);
        if (st == p_state && step == p_step && talk == p_talk && frame - p_last < 120)
            return;
        p_state = st; p_step = step; p_talk = talk; p_last = frame;
        std::fprintf(stderr, "  [rabbit] f%d state=%p step=%d talk=%d +426=%d "
                     "+427=%d +429=%d +43c=%d +45c=%p d660=%d\n", frame, st, step,
                     talk, (int)*(unsigned char *)(rb + 0x426),
                     (int)*(unsigned char *)(rb + 0x427),
                     (int)*(unsigned char *)(rb + 0x429),
                     *(int *)(rb + 0x43c), *(void **)(rb + 0x45c),
                     (int)data_0209d660);
        return;
    }
    if (*(void **)(rb + 0x45c) != 0) {          /* the real grab took */
        caught = 1;
        std::fprintf(stderr, "  [rabbit] f%d GRABBED: Player::TryGrab succeeded, "
                     "rabbit+0x45c = %p (the field the stale comment in "
                     "Ov085_Rabbit_b8dc.cpp says nothing writes)\n", frame,
                     *(void **)(rb + 0x45c));
        return;
    }

    /* park the rabbit on the player: the headless player never runs it down, and
       the grab check itself is distance-free (the cylinder is what carries the
       distance, and the cylinder is what this stands in for). */
    *(int *)(rb + 0x5c) = *(int *)(player + 0x5c);
    *(int *)(rb + 0x60) = *(int *)(player + 0x60);
    *(int *)(rb + 0x64) = *(int *)(player + 0x64);

    *(unsigned int *)(rb + 0x134) = *(unsigned int *)(player + 0x4); /* player uid */
    *(int *)(rb + 0x130) |= 0x1000;                                  /* trigger */

    if (++tries <= 5 || (tries % 60) == 0) {
        /* animcur is the rabbit's Animation::currFrame (+0x350+0x08). Rabbit::
           Behavior advances it every tick, so printing it BEFORE the grab as
           well as after says whether the actor was ever ticked at all -- the
           difference between "the caught dialogue refused" and "the caught
           dialogue never ran". */
        std::fprintf(stderr, "  [rabbit] f%d arming grab (try %d): holding=%d "
                     "grabbable=%d b0=0x%08x animcur=%d "
                     "lvlf2f8=%d caa0_2=0x%08x bit20000=%d rb428=%d rb440=%d "
                     "pl6d9=%d "
                     "st574=%d st67c=%d st5bc=%d p6e2=%d\n", frame,
                     tries, *(int *)(player + 0x358) != 0,
                     (*(int *)(rb + 0xb0) & 0x80) != 0,
                     (unsigned)*(int *)(rb + 0xb0),
                     *(int *)(rb + 0x350 + 0x08),
                     /* THE BEHAVIOR GATE. src/_ZN6Rabbit8BehaviorEv.c:50-65
                        returns before the state dispatch unless the level word
                        data_0209f2f8 and these fields agree. Off level 0x32 the
                        only question is rabbit+0x428; on level 0x32 it is the
                        0x20000 save bit plus rabbit+0x440 == player+0x6d9. */
                     (int)data_0209f2f8, (unsigned)data_0209caa0[2],
                     (data_0209caa0[2] & 0x20000) != 0,
                     (int)*(unsigned char *)(rb + 0x428),
                     *(int *)(rb + 0x440),
                     (int)*(unsigned char *)(player + 0x6d9),
                     _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_02110574),
                     _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_0211067c),
                     _ZN6Player7IsStateERNS_5StateE(player, &data_ov002_021105bc),
                     (int)*(unsigned char *)(player + 0x6e2));
    }
    func_ov085_0212a828(rb);        /* the rabbit's own real grab check */
}

/* TEMPORARY frame-scheduled RABBIT_KEY spawn.
 *
 * SM64DS_SPAWN_ACTOR only fires at boot, and the whole question about this
 * actor is a TIMING one: on the ROM the key is spawned by the rabbit's own
 * caught-dialogue state (src/func_ov085_0212ae08.c:235) in the same breath as
 * the talk ending, so it reaches its caught state while the player may still be
 * flagged mIsNoControl. A boot spawn always lands on an idle player and can
 * never ask that question. This drops a key at a chosen frame instead.
 *
 *   SM64DS_KEY_SPAWN_AT=<frame>[:<param>]   param default 0
 *
 * It uses the harness's own port_debug_spawn_at, which goes through the level's
 * real Actor::Spawn -- the same call func_ov085_0212ae08 makes, with the same
 * class id 0xe5 and the same param word.
 */
extern "C" {
extern void *port_debug_spawn_at(unsigned id, unsigned param,
                                 int x, int y, int z, int yaw, int area);
}

extern "C" void port_probe_key_spawn(int frame)
{
    const char *e = std::getenv("SM64DS_KEY_SPAWN_AT");
    if (!e) return;
    static int fired;
    if (fired) return;
    int at = std::atoi(e);
    unsigned param = 0;
    const char *colon = std::strchr(e, ':');
    if (colon) param = (unsigned)std::strtoul(colon + 1, 0, 0);
    if (at <= 0 || frame < at) return;
    char *player = (char *)find_actor_by_class(0xbf);
    if (!player) return;
    fired = 1;
    std::fprintf(stderr, "  [rkey] f%d spawning RABBIT_KEY param 0x%x at the "
                 "player (mIsNoControl=%d kind=%d, d660=%d)\n", frame, param,
                 (int)*(unsigned char *)(player + 0x709),
                 (int)*(unsigned char *)(player + 0x70a), (int)data_0209d660);
    /* The player's area is mAreaId, the s8 at Actor+0xcc -- the same byte
       hal/level_boot.cpp's port_actor_area and hal/editor_channel.cpp's
       OFF_ACTOR_AREA read. +0x10, which this line used to read, is the
       boolean AfterInitResources keeps, not an area. */
    port_debug_spawn_at(0xe5, param, *(int *)(player + 0x5c),
                        *(int *)(player + 0x60) + 0x32000,
                        *(int *)(player + 0x64),
                        (int)*(short *)(player + 0x8e),
                        (int)*(signed char *)(player + 0xcc));
}

/* TEST SCAFFOLDING: frame-scheduled two-player overlap fixture.
 *
 *   SM64DS_VS_OVERLAP_AT=<frame>[:<units>]   units default 40
 *
 * Places slot 1's body at slot 0's position plus <units> on x, once, at the
 * chosen frame -- the fixture port/tools/mp3_proof.py's collision rungs need.
 * It exists because the fixture those rungs USED to lean on is gone for a
 * good reason: they measured the solver against the fabricated stand-in
 * spawn that put the pair 40 units apart, and since the fc5c width fix the
 * ROM's own entrance loop seats every player on its real start -- castle
 * grounds' records 0 and 1 are 229 units apart, so two correct spawns never
 * touch and a rung that waits for a spawn-time overlap measures nothing.
 * Forcing the overlap at a chosen frame keeps the rungs measuring the thing
 * they were built for (CylinderClsn::Process pushing two live bodies apart)
 * without depending on where any level puts its starts.
 *
 * Writes mPosX/Y/Z only (+0x5c/+0x60/+0x64), before the actor tick like
 * every probe in this block, so the same frame's Behavior carries the forced
 * position into its own pre-clsn snapshot and cylinder update. One-shot, and
 * it says so on stderr so a harness can anchor its post-overlap window on
 * the line rather than on trust.
 */
extern "C" {
extern void *data_0209f394[];            /* the per-slot Player pointers */
}

extern "C" void port_probe_vs_overlap(int frame)
{
    const char *e = std::getenv("SM64DS_VS_OVERLAP_AT");
    if (!e) return;
    static int fired;
    if (fired) return;
    int at = std::atoi(e);
    int units = 40;
    if (const char *colon = std::strchr(e, ':'))
        units = std::atoi(colon + 1);
    if (at <= 0 || frame < at) return;
    char *p0 = (char *)data_0209f394[0];
    char *p1 = (char *)data_0209f394[1];
    if (!p0 || !p1) return;
    fired = 1;
    *(int *)(p1 + 0x5c) = *(int *)(p0 + 0x5c) + (units << 12);
    *(int *)(p1 + 0x60) = *(int *)(p0 + 0x60);
    *(int *)(p1 + 0x64) = *(int *)(p0 + 0x64);
    std::fprintf(stderr, "  [vsfix] f%d slot1 placed at slot0 + %d units on x "
                 "(SM64DS_VS_OVERLAP_AT)\n", frame, units);
}

/* TEMPORARY rabbit-key teardown trace.
 *
 * RABBIT_KEY (229, ov085) hovers over the player's head after it is collected
 * and follows them for the rest of the session (two independent 0.2.13
 * reporters, TRIAGE14 report 2). The hover itself is the ROM's own caught
 * animation -- func_ov085_0212cd80 pins the key to the player's X/Z at his
 * Y + 0xc8000 every frame -- so what a run has to show is the state machine
 * BESIDE the hover: whether the caught state ever leaves case 1.
 *
 * That machine's only exits run through data_0209d684, the answer to the choice
 * box func_ov085_0212d038 opens, and data_0209d660, the message-active flag.
 * Both are globals, and the key's own step word is +0x194, so one line carries
 * the whole question:
 *
 *   SM64DS_TRACE_RABBITKEY=1
 *     step   the key's +0x194 (0/1 caught-wait, 2/3 answered, 6 the two-option
 *            variants, 10 the HasFinishedTalking wait; gone = destroyed)
 *   +0x198   the "came from the minigame door" flag the gb==3 arm reads
 *   +0x19c   the spawn param func_ov085_0212d038 switches the message id on
 *     d684   the choice answer (0 = unanswered, 1..3 = the picked option)
 *     d660   1 while a message box is up
 *    noctl   the player's mIsNoControl (+0x709) and mNoCtrlKind (+0x70a):
 *            SetNoControlState REFUSES for kind <= 3 while mIsNoControl is
 *            already set, and the ov085 caller ignores that refusal.
 *
 * Change-driven with a 60-frame heartbeat, and it announces the destroy, so a
 * headless run shows the cleanup completing rather than only the hover.
 */
extern "C" void port_probe_rabbit_key(int frame)
{
    if (!std::getenv("SM64DS_TRACE_RABBITKEY")) return;
    char *k = (char *)find_actor_by_class(229);
    char *player = (char *)find_actor_by_class(0xbf);
    static int seen, gone_announced;
    static int p_step = -99, p_d684 = -99, p_d660 = -99, p_noctl = -99;
    static int last;

    if (!k) {
        if (seen && !gone_announced) {
            gone_announced = 1;
            std::fprintf(stderr, "  [rkey] f%d KEY DESTROYED: no RABBIT_KEY on "
                         "the live-actor list -- the caught state reached "
                         "func_ov085_0212cd0c -> MarkForDestruction\n", frame);
        }
        return;
    }
    seen = 1;

    int step  = *(int *)(k + 0x194);
    int f198  = *(int *)(k + 0x198);
    int f19c  = *(int *)(k + 0x19c);
    int d684  = (int)data_0209d684;
    int d660  = (int)data_0209d660;
    int noctl = player ? (int)*(unsigned char *)(player + 0x709) : -1;
    int kind  = player ? (int)*(unsigned char *)(player + 0x70a) : -1;

    /* while the key is still descending (its own step word 0) print EVERY frame:
       the descent is only about ten frames and the whole question is which of it
       and the rabbit's second dialogue lands first. */
    /* ...and print EVERY frame while a message is up too (d660 != 0). That is
       the window lane C4 measures: the ROM freezes the key's Behavior for its
       whole length, so cd100/ky/kvy must not move across it. A change-driven
       line cannot show a value NOT changing. */
    /* SM64DS_RKEY_EVERY=1 drops the change filter entirely. A stall is a value
       NOT changing, so a change-driven line cannot show one. */
    static int every = -1;
    if (every < 0) every = std::getenv("SM64DS_RKEY_EVERY") != 0;

    if (!every && step != 0 && d660 == 0 &&
        step == p_step && d684 == p_d684 && d660 == p_d660 && noctl == p_noctl
        && frame - last < 60)
        return;
    p_step = step; p_d684 = d684; p_d660 = d660; p_noctl = noctl; last = frame;

    /* +0x100 is the key's DESCENT COUNTDOWN. func_ov085_0212d24c (the spawn
       init) sets it to 10 and func_ov085_0212d108 refuses to home or hand over
       to the caught state while it is non-zero, so it is the whole clock on the
       race between the key arriving and the rabbit's SECOND dialogue opening.
       +0x60 is the key's Y and +0xa8 its vertical speed: the handover test is
       purely geometric, player.y + 0x64000 > key.y, with no control-state check
       anywhere in it. */
    /* b454/b464 ARE THE FREEZE MASK, lane C4's whole subject. data_0209b454 is
       the persistent freeze-request word the message code ORs 0x800000 into
       (func_ov002_020c8540.c:51 and friends); data_0209b464 is the per-frame
       copy Stage::Behavior latches from it (_ZN5Stage8BehaviorEv.cpp:105) and
       the ONLY word Actor::BeforeBehavior reads
       (_ZN5Actor14BeforeBehaviorEv.cpp:74). kf is the key's own mFlags & 0x800000
       -- its opt-out of the freeze, which it does not have. So the three
       together say whether this frame's key Behavior should have run at all. */
    std::fprintf(stderr, "  [rkey] f%d step=%d +198=%d +19c=0x%x d684=%d "
                 "d660=%d noctl=%d kind=%d cd100=%d ky=%d kvy=%d "
                 "b454=%08x b464=%08x kf800=%d d6bc=%d kx=%d px=%d ticked=%d "
                 "e430=%d b490=%x b49c=%x\n",
                 frame, step, f198, f19c, d684, d660, noctl, kind,
                 (int)*(unsigned short *)(k + 0x100),
                 *(int *)(k + 0x60), *(int *)(k + 0xa8),
                 (unsigned)data_0209b454[0], (unsigned)data_0209b464,
                 (*(unsigned *)(k + 0xb0) & 0x800000) ? 1 : 0,
                 (int)data_0209d6bc,
                 /* kx/px: the caught state's Main copies the PLAYER's x and z
                    into the key's own +0x5c/+0x64 on every call
                    (func_ov085_0212cd80.c:33-35). So kx == px is a direct
                    read-out of "the key's Behavior ran this frame", which is
                    what the freeze is supposed to stop. */
                 *(int *)(k + 0x5c),
                 player ? *(int *)(player + 0x5c) : 0,
                 (player && *(int *)(k + 0x5c) == *(int *)(player + 0x5c)) ? 1 : 0,
                 /* the three globals Sound::PlaySub reads. Case 6 of the key's
                    caught state (func_ov085_0212cd80.c:95) is gated on
                    PlaySub(0x28, 0x12, 0x7f, ...), which on the ld4 path
                    returns false unless data_0208e430 == 0x28 (=40) and the two
                    fade words have converged to b490 == 0x12000 and
                    b49c == 0x7f000. Case 1 has no such gate, which is why the
                    param-0 key tears down and the glowing one does not. */
                 (int)data_0208e430, (unsigned)data_0209b490[0],
                 (unsigned)data_0209b49c[0]);
}
