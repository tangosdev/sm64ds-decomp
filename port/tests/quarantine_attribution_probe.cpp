/* Quarantine attribution probe -- proves the two fault-attribution defects in
   the port's quarantine net (port/unmatched/func_02043fdc_hostcopy.cpp) are
   fixed, without a running game. Like gate1_probe.cpp, it links the REAL
   quarantine translation unit and drives the REAL list walker func_02043fdc,
   whose port_dispatch_guarded wraps each actor tick in the __try/__except that
   actually ships. Nothing here is synthetic: port_fault_synthetic is never set,
   PORT_TEST_HOOKS is left OFF, SM64DS_FAULTS_FATAL must be unset so the net
   swallows (the net only runs when FAULTS_FATAL is off).

   Two defects, both amplifying a single actor's fault into a multi-player
   softlock:

     DEFECT 1 -- THE CLASS LATCH. After two quarantines of one actor id, the net
       latched port_q_class_skip[id] and froze EVERY instance of that id on
       sight. Both versus players are id 0xbf, so the second fault from either
       froze BOTH. The fix exempts the player id from the class latch while still
       freezing the individual faulting instance -- and still latches OTHER ids.

     DEFECT 2 -- WALKER MIS-ATTRIBUTION. A genuine access violation (no named
       decline) froze the WALKER -- the actor whose callback was running, i.e.
       the player -- instead of the receiver the player was interacting with. The
       fix latches the receiver at the interaction dispatch veneers and prefers
       it over the walker, while a fault with no receiver still freezes the
       walker (safety preserved).

   This probe links against the FIXED net (the interaction seam
   port_actor_interaction_begin/end is always present there) and asserts both
   defects are gone. The before/after was taken by compiling this same logic
   against the PRE-FIX net with the seam stubbed out -- there the second id-0xbf
   fault latched the class and the receiver fault froze the walker; here neither
   does. No linker alias directive lives in this file: the alias guard scans
   every source in the tree, and a test-only alias reads to it as a real
   defeated one.

   Build (32-bit, matching the shipped exe), from the repo root:
     cl /nologo /EHa /O2 /Fe:qattr_probe.exe ^
        port/tests/quarantine_attribution_probe.cpp ^
        port/tests/gate1_globals.c ^
        port/unmatched/func_02043fdc_hostcopy.cpp ^
        /I port/tests /link /SUBSYSTEM:CONSOLE
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stddef.h>

extern "C" {
void *func_02043fdc(void *listv);
int  port_quarantine_is_frozen(void *actor);
int  port_quarantine_frozen_count(void);
void port_quarantine_reset(void);

/* The interaction-receiver seam the fixed net exports and the dispatch veneers
   call. Test 3 drives it the way a dispatch veneer does. */
void *port_actor_interaction_begin(void *receiver);
void  port_actor_interaction_end(void *prev);

/* The net looks class names up through this weak DATA resolver; null is fine
   (it prints "?"). fault_probe.h would define it, but this probe does not pull
   the probe header (it wants the net's weak rich-dump/crash-dir stubs, so no
   dump is written and no crash-dir line is logged). Define it here instead. */
typedef const char *(*port_classname_fn)(unsigned id);
port_classname_fn port_classname_resolver = 0;
}

/* ---- test actors: plain objects with an id at +0xc (the u16 the net reads) -- */
static int g_A[8], g_B[8], g_C[8];      /* Test 1: three id-0xbf instances     */
static int g_D[8], g_E[8], g_F[8];      /* Test 2: three id-0x50 instances     */
static int g_walker[8], g_recv[8];      /* Test 3: walker + interaction target */
static int g_walker2[8];                /* Test 4: lone walker, no interaction */

static void set_id(int *actor, unsigned id) { *(unsigned short *)((char *)actor + 0xc) = (unsigned short)id; }

static int g_ran;
static int cb_raise_av(void *)   { RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, 0); return 0; }
static int cb_set_ran(void *)    { g_ran = 1; return 0; }

/* Test 3's walker callback: it does what an interaction dispatch veneer does --
   name the receiver, then dispatch into it -- but the receiver "faults". The
   end() is never reached; the __except attributes the fault to the receiver. */
static int cb_interact_then_fault(void *)
{
    void *prev = port_actor_interaction_begin(&g_recv[0]);
    RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, 0);
    port_actor_interaction_end(prev);   /* unreachable */
    return 0;
}

/* Walk a one-node processing list whose callback is `cb`, dispatching `actor`.
   list = {head, tail, callback, 0}; node = {prev, next, owner, ...}. */
static void walk_one(void *actor, int (*cb)(void *))
{
    int node[4], list[4];
    node[0] = 0; node[1] = 0; node[2] = (int)(size_t)actor; node[3] = 0;
    list[0] = (int)(size_t)&node[0]; list[1] = 0;
    list[2] = (int)(size_t)cb; list[3] = 0;
    func_02043fdc(&list[0]);
}

static int g_fail;
static void check(const char *name, int cond)
{
    printf("  [%s] %s\n", cond ? "PASS" : "FAIL", name);
    if (!cond) g_fail = 1;
}

int main(void)
{
    set_id(g_A, 0xbf); set_id(g_B, 0xbf); set_id(g_C, 0xbf);
    set_id(g_D, 0x50); set_id(g_E, 0x50); set_id(g_F, 0x50);
    set_id(g_walker, 0x30); set_id(g_recv, 0x40); set_id(g_walker2, 0x31);

    /* ---- Test 1: DEFECT 1 -- the PLAYER class (0xbf) must NOT latch --------- */
    printf("Test 1: two quarantines of the player id 0xbf must not latch the class\n");
    port_quarantine_reset();
    walk_one(g_A, cb_raise_av);          /* first player fault  -> freeze A     */
    walk_one(g_B, cb_raise_av);          /* second player fault -> freeze B     */
    check("faulting player instance A is frozen", port_quarantine_is_frozen(&g_A[0]));
    check("faulting player instance B is frozen", port_quarantine_is_frozen(&g_B[0]));
    g_ran = 0;
    walk_one(g_C, cb_set_ran);           /* a fresh, healthy second player      */
    check("a fresh player instance C is NOT frozen on sight",
          !port_quarantine_is_frozen(&g_C[0]));
    check("the fresh player C's callback actually ran (class not latched)", g_ran == 1);

    /* ---- Test 2: DEFECT 1 regression -- a NON-player id still rate-limits --- */
    printf("Test 2: a non-player class (0x50) must still latch after two quarantines\n");
    port_quarantine_reset();
    walk_one(g_D, cb_raise_av);
    walk_one(g_E, cb_raise_av);          /* second fault of the class -> latch  */
    g_ran = 0;
    walk_one(g_F, cb_set_ran);           /* fresh instance of the latched class */
    check("a fresh instance of the latched class IS frozen on sight",
          port_quarantine_is_frozen(&g_F[0]));
    check("the latched class's fresh callback did NOT run", g_ran == 0);

    /* ---- Test 3: DEFECT 2 -- a receiver's fault must freeze the RECEIVER ---- */
    printf("Test 3: a genuine AV inside an interaction receiver must freeze the "
           "receiver, not the walker-player\n");
    port_quarantine_reset();
    walk_one(g_walker, cb_interact_then_fault);
    check("the interaction receiver is frozen", port_quarantine_is_frozen(&g_recv[0]));
    check("the walker (player) is NOT frozen", !port_quarantine_is_frozen(&g_walker[0]));

    /* ---- Test 4: DEFECT 2 regression -- a bare fault still freezes walker --- */
    printf("Test 4: a genuine AV with no interaction receiver still freezes the "
           "walker (safety preserved)\n");
    port_quarantine_reset();
    walk_one(g_walker2, cb_raise_av);
    check("the lone faulting walker is frozen", port_quarantine_is_frozen(&g_walker2[0]));

    printf("\n%s\n", g_fail ? "RESULT: FAIL (a defect is present)"
                            : "RESULT: PASS (both defects fixed, safety intact)");
    return g_fail;
}
