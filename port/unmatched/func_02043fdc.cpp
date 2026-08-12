/* HOST COPIES of src/func_02043fdc.cpp and src/func_020441cc.cpp -- the two
 * processing-list walks, with the mwcc pointer-to-member-function they
 * dispatch through read as a plain function pointer.
 *
 * Each list head carries its callback as an eight-byte PMF pair that
 * __sinit_02075154 copies out of arm9's five statics at 0x02099f48..0x02099f70.
 * All five are NONVIRTUAL PMFs -- { function address, 0 } -- naming
 * func_020432e4, func_0204335c, func_02043880, func_0204322c and
 * func_02043288, the four Process wrappers plus the scene-tree housekeeping
 * pass. MSVC has no representation for an mwcc PMF, so the port seats the
 * host function in the first word and reads it back as a function pointer;
 * hal/actor_registry.cpp's port_actor_lists_seat is the sinit's other half.
 *
 * Control flow is the matched sources', unchanged. func_02043fdc reads the
 * successor BEFORE the callback runs (a Process that destroys the actor
 * unlinks the node under it) and publishes the node it is on in
 * data_020a4b68, which is what func_020440e8/func_02044104 clear when a
 * destructor takes the walk's own cursor out. func_020441cc walks the scene
 * tree instead, whose successor comes from func_0203b394.
 */
extern "C" {

typedef int (*PortListFn)(void *self);

void *func_0203b394(void *node);
extern int data_020a4b68[];        /* the walk's published cursor
                                      (storage: hal/player_bridges.cpp) */

void port_scene_canary(const char *where);
}
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
/* Linux: the HAL shim supplies BOOL/HANDLE/DWORD and GetModuleHandleA (returns
   nullptr, so the module base is null and offsets read as absolute -- fine for
   this lane, the offsets feed the crash-dump forensics which is a later lane). */
#include "../hal/host_platform_linux.h"
/* IsBadReadPtr is a Win32-only pointer-validity probe; a real probe is a later
   lane, so on Linux assume readable. Arity matches the call sites below,
   IsBadReadPtr(ptr, size). */
static inline int IsBadReadPtr(const void *, unsigned long) { return 0; }
#include <ctime>   /* port_q_log fills the SYSTEMTIME fields from localtime_r */
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* The test-hook switch, hoisted here because TWO places in this file compile
   against it and the second one (port_quarantine_actor's stderr line) comes
   well before the hook itself. 0 means the SM64DS_TEST_QUARANTINE fault
   injector is not in the object file at all, which is what every build that is
   not an explicit proof run gets. See the long note on the hook near the bottom
   of this file, and the PORT_TEST_HOOKS option in port/CMakeLists.txt. */
#ifndef PORT_TEST_HOOKS
#define PORT_TEST_HOOKS 0
#endif

extern "C" {
/* SM64DS_SCENE_CANARY=1: walk the scene tree and report the first node whose
   owner back-pointer is not node-0x14.
 *
 * Every ActorBase writes its own address into its SceneNode's owner slot and
 * never touches it again, so that one invariant is a cheap tripwire for a
 * stray write into the actor heap -- which is what an eighteen-slot class
 * filled like a twenty-slot one produces (see hal/sub_actors.cpp). The walk
 * that dereferences the owner is func_020441cc below, and it faults a long way
 * from whoever actually did the damage; this says which phase to look in.
 * Off by default, and free when it is off. */
extern int data_020a4b6c[];
void port_scene_canary(const char *where)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_SCENE_CANARY") != 0;
    if (!on) return;
    int *node = (int *)(size_t)data_020a4b6c[0];
    int n = 0;
    while (node != 0 && n++ < 4000) {
        char *o = (char *)(size_t)node[4];
        if ((char *)node != o + 0x14) {
            std::printf("[canary] %s: node=%p owner=%p (skew %d)\n", where,
                        (void *)node, (void *)o, (int)((char *)node - o));
            std::fflush(stdout);
            return;
        }
        node = (int *)func_0203b394(node);
    }
    std::printf("[canary] %s: clean (%d nodes)\n", where, n);
    std::fflush(stdout);
}

/* SM64DS_FADER_WATCH=1: after every Process dispatch, check the installed
   fader (data_0209f5bc) still carries a vptr, and name the ACTOR whose
   dispatch left it broken. The reader that faults on a broken install
   (HUD::Behavior's IsAtStart) runs a long way down the same list from
   whoever installed it; this points at the writer. Off by default, free
   when off. */
extern void *data_0209f5bc;
void port_fader_watch(void *actor)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_FADER_WATCH") != 0;
    if (!on) return;
    static void *last;
    static void *last_vptr;
    void *f = data_0209f5bc;
    void *vptr = f ? *(void **)f : 0;
    if (f != last) {
        std::fprintf(stderr, "[fwatch] actor %p (id %u) set fader %p vptr %p",
                     actor,
                     actor ? *(unsigned *)((char *)actor + 8) & 0xffff : 0u,
                     f, vptr);
        if (f) {
            unsigned *w = (unsigned *)f;
            std::fprintf(stderr, "  words[%08x %08x %08x %08x %08x %08x]",
                         w[0], w[1], w[2], w[3], w[4], w[5]);
        }
        if (actor && (*(unsigned *)((char *)actor + 8) & 0xffff) == 0)
            std::fprintf(stderr, "  player-state %p",
                         *(void **)((char *)actor + 0x370));
        std::fprintf(stderr, "%s\n", vptr ? "" : "  <-- NULL VPTR");
    } else if (vptr != last_vptr) {
        std::fprintf(stderr, "[fwatch] fader %p VPTR CHANGED %p -> %p after "
                     "actor %p (id %u)\n", f, last_vptr, vptr, actor,
                     actor ? *(unsigned *)((char *)actor + 8) & 0xffff : 0u);
    }
    last = f;
    last_vptr = vptr;
}

/* ===========================================================================
 * PER-ACTOR FAULT QUARANTINE (Tango's ask #2)
 *
 * The two walks below call each actor's phase callback through fn(actor). A
 * bad actor -- a stale vtable, a half-initialised pointer, a resource that did
 * not load -- faults inside that call and, without a net, takes the whole
 * process down mid-frame. This wraps the ONE per-actor dispatch in SEH so a
 * fault inside a single actor's Behavior/Render/Cleanup does not kill the
 * frame: the actor is QUARANTINED and the walk moves on to the next node.
 *
 * DEFAULT ON. SM64DS_FAULTS_FATAL=1 turns it off (the __except filter declines,
 * the fault reaches the UEF, the process dies hard) for proof runs that want to
 * see the raw crash. The crash.txt/rich-dump write happens either way -- it is
 * done in the filter, before the decline.
 *
 * QUARANTINE SEMANTICS -- the hard part, and the source of last night's six
 * teardown bugs. Two options exist:
 *   (a) REMOVE the actor via the same path level-exit teardown uses
 *       (ActorBase::AfterCleanupResources: scene unlink, cleanup-list unlink,
 *       heap destroy, D1 dtor, free). Correct, but the D1 dtor is ACTOR CODE and
 *       may be exactly what faulted -- so removal must tolerate a second fault.
 *   (b) FREEZE: leave the node linked, set a skip flag the walk honours, and
 *       never dispatch this actor again. Its resources leak until level exit.
 *
 * This code DEFAULTS TO (b) FREEZE, always. A freeze runs no actor code at all
 * -- it only records the actor pointer in a skip set and checks that set before
 * each dispatch -- so there is NO double-fault surface and NO list left
 * half-linked. A deliberate leak of one dead actor's heap until the level
 * unloads is strictly safer than a half-completed teardown that corrupts a list
 * every other actor still walks. Option (a) is deliberately NOT taken here for
 * that reason; if a future change wants it, it must wrap AfterCleanupResources
 * in its own nested __try and fall back to (b) on the second fault. The freeze
 * set is cleared at level exit by port_quarantine_reset (called from the level
 * teardown), so the leak is bounded to one level's lifetime.
 *
 * RATE LIMIT: a class that quarantines twice earns a class-level skip for the
 * rest of the session -- every instance of that id is frozen on sight, with ONE
 * log line, not one per instance per frame. Stops a class that faults on every
 * frame from filling the log.
 *
 * LOG: every quarantine appends one line to %TEMP%\sm64ds-crashes\quarantine.log
 * (timestamp, level, actor id/class, fault code, module offset). The full rich
 * dump lands beside it under the same directory.
 * =========================================================================== */

extern "C" signed char data_0209f2f8;                 /* current level */
/* class-name lookup goes through the same weak DATA resolver fault_probe.h uses
   (set by hal/actor_registry.cpp in walk_window; null in a bare smoke, which
   then prints "?"). Declaring it here avoids a hard dependency on the HAL that
   defines port_actor_class_name -- the walker links into smokes without it. */
typedef const char *(*port_classname_fn)(unsigned id);
extern "C" port_classname_fn port_classname_resolver;
static const char *port_q_class(unsigned id)
{
    return (id != 0xffffu && port_classname_resolver)
           ? port_classname_resolver(id) : "?";
}

/* The rich dump + crash-dir helpers live in fault_probe.h, whose internals are
   static. walk_window.cpp (the one TU that installs the probe) opts in with
   PORT_FAULT_PROBE_DEFINE_EXPORTS and exposes these two external seams. We
   declare them here (not include the header) so this plain-C actor TU does not
   pull the VEH/detour installers. A bare smoke that links this walker without
   walk_window gets the weak stubs below -- quarantine still works there, it
   just writes no rich dump and logs into the exe directory. */
struct _EXCEPTION_POINTERS;
extern "C" void port_rich_dump_ex(struct _EXCEPTION_POINTERS *ep, unsigned code,
                                  const char *reason);
extern "C" const char *port_crash_dir_get(void);

/* Plain (non-selectany) stub bodies: this .cpp is compiled exactly once into
   any binary, so a single definition never collides. selectany is illegal on
   functions in MSVC; the alternatename directive supplies the fallback only
   when the real export (from walk_window) is absent. */
#ifdef _WIN32
extern "C"
void port_rich_dump_ex_stub(struct _EXCEPTION_POINTERS *, unsigned,
                            const char *) {}
#pragma comment(linker, \
    "/alternatename:_port_rich_dump_ex=_port_rich_dump_ex_stub")
extern "C" const char *port_crash_dir_get_stub(void)
{ return ""; }
#pragma comment(linker, \
    "/alternatename:_port_crash_dir_get=_port_crash_dir_get_stub")
#else
/* Linux/g++: alternatename is MSVC-only. The gcc equivalent of "fallback used
   only when the real export is absent" is a WEAK definition of the real symbol:
   walk_window's strong PORT_FAULT_PROBE_DEFINE_EXPORTS definitions override
   these, a bare smoke links against them. */
extern "C" __attribute__((weak))
void port_rich_dump_ex(struct _EXCEPTION_POINTERS *, unsigned, const char *) {}
extern "C" __attribute__((weak)) const char *port_crash_dir_get(void)
{ return ""; }
#endif

/* --- the freeze set + class rate limit ------------------------------------ */
#define PORT_Q_MAX     256      /* frozen instances this level (bounded leak) */
#define PORT_Q_IDS     512      /* actor id space for the class rate limit */

static void *port_q_frozen[PORT_Q_MAX];
static int   port_q_frozen_n;
static unsigned char port_q_class_count[PORT_Q_IDS]; /* quarantines per id */
static unsigned char port_q_class_skip[PORT_Q_IDS];  /* id-level skip latch */

/* --- THE DECLINE TARGET: who the net is supposed to freeze ------------------

   The comment below says the raise "lets the quarantine net freeze just that
   actor". It did not. port_dispatch_guarded's __except froze the WALKER -- the
   actor whose phase callback the list walk was inside -- and an interaction
   slot is dispatched from the ATTACKER's callback, not the receiver's. So every
   trapped interaction slot on a piece of level furniture froze the PLAYER:

     Player::Behavior -> ground pound / punch / head bonk
       -> receiver's vtable slot 21 / 23 / 28
         -> ac_trap_report -> port_actor_slot_decline -> RaiseException
           -> __except in port_dispatch_guarded, walking the PLAYER's node
             -> port_quarantine_actor(THE PLAYER)

   Measured, not reasoned: thirty-one PLAYER-walker quarantines in the live
   report index across four shipped builds and ten levels, twenty-six of them
   followed in the same playlog by

     [lvl] TEARDOWN DID NOT CONVERGE: 1 actors still live after 16 rounds
     [lvl] teardown failed; the change is declined and the level stands

   which is the second half of the same bug: a frozen actor is never dispatched
   again, so it is never destroyed, so no level change can ever complete for the
   rest of the session. One punch on one brick ends the run.

   The fix is attribution, not a seat. port_actor_slot_decline_for latches the
   actor that actually declined; the __except prefers that latch over the
   walker. A trapped brick then freezes the BRICK -- it stops rendering and
   stops responding, which is the honest presentation of an unhosted slot -- and
   the player walks away. Call sites that cannot name a receiver keep the old
   entry point and the old walker attribution.

   THE RAISE IS NOT OPTIONAL and must not be turned into a plain return. These
   trap thunks are two-parameter __fastcall and emit a bare `ret` that pops
   nothing, while several of the slots they sit on are dispatched by callers
   that push arguments. Their `ret` is only safe because control never reaches
   it. Suppressing the raise for an already-frozen target would make it
   reachable and desync the caller's frame. Repeats are made cheap in the FILTER
   instead (no second rich dump), where nothing about the return path changes.

   Single-threaded: the game loop is one thread, the latch is set microseconds
   before the raise and cleared by the handler that consumes it. */
static void *port_q_decline_target;

/* Shared "per-actor decline" for the HAL trap sites that used to std::abort()
   on an unhosted vtable slot or an unhosted actor state. Those declines fire
   INSIDE fn(actor) -- within the quarantine __try -- but abort() is an orderly
   exit that SEH does not catch, so it took the whole process down for one bad
   actor. Raising a real exception instead lets the quarantine net freeze just
   that actor and keep the level running. Under SM64DS_FAULTS_FATAL the old hard
   abort() is preserved for proof runs. `what` is logged for the operator.

   NOT for structural/boot declines (bad mount bytes, missing level, out of file
   slots): those stay std::abort() at their call sites -- see the audit in the
   crashh report. This is only wired to the RUNTIME per-actor slot/state traps. */
extern "C" void port_actor_slot_decline(const char *what)
{
    if (std::getenv("SM64DS_FAULTS_FATAL")) {
        std::fprintf(stderr, "  (SM64DS_FAULTS_FATAL: %s -> hard abort)\n",
                     what ? what : "actor decline");
        std::fflush(stderr);
        std::abort();
    }
    std::fprintf(stderr, "  (quarantining actor: %s)\n",
                 what ? what : "actor decline");
    std::fflush(stderr);
#ifdef _WIN32
    /* raise a catchable AV; the enclosing port_dispatch_guarded __except sees
       it, writes the dump, freezes the actor, and the walk continues. */
    RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, 0);
#else
    /* TODO(linux-port): no SEH to catch a raised exception, and the enclosing
       port_dispatch_guarded is a PASSTHROUGH on Linux for Phase 1 -- there is
       nothing to quarantine the actor. Just log and return so the caller can
       continue if it can; real signal/setjmp containment is a later lane. Do
       NOT abort here: a passthrough should not turn a soft decline fatal. */
#endif
}

/* Same decline, but NAMING the actor that declined. Trap sites that know their
   receiver call this one; the __except then freezes the receiver instead of
   whichever actor's phase callback the walk happened to be inside. Call sites
   that genuinely cannot name a receiver keep port_actor_slot_decline above and
   keep the walker attribution, which for them is the correct answer. */
extern "C" void port_actor_slot_decline_for(void *actor, const char *what)
{
    port_q_decline_target = actor;
    port_actor_slot_decline(what);
}

static int port_faults_fatal(void)
{
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_FAULTS_FATAL") != 0;
    return v;
}

/* Non-zero for exactly the window of a fault the SM64DS_TEST_QUARANTINE hook at
   the bottom of this file raised on purpose, and never at any other time.

   IT IS A SUPPRESSION FLAG, not a label. Tagging a harness dump "testhook" and
   writing it into the crash directory anyway was the wrong answer: that
   directory is the player report intake, it keeps only the newest five dumps,
   and fault_probe.h's first-chance VEH claims a one-shot "first crash wins"
   latch the moment any access violation goes past it. A forced fault that lands
   there costs a real report its slot and burns the latch that would have caught
   the crash the operator was actually hunting. So while this is set, NOTHING is
   written to disk: no rolling dump from the quarantine filter, no quarantine.log
   line, and no crash.txt or dump from the VEH/UEF either -- fault_probe.h reads
   the same flag (see port_crash_write_file). The stderr line carries everything
   a proof run needs.

   Shared with fault_probe.h through a pair of identical selectany definitions:
   nothing defines it strongly, so the two COMDATs fold to one object and both
   sides read and write the same int. It has to live here and not behind
   PORT_TEST_HOOKS because fault_probe.h reads it in every build.

   Cleared as soon as the quarantine is recorded. It deliberately stays set when
   SM64DS_FAULTS_FATAL makes the filter decline: the process is dying from a
   fault the harness manufactured, and that is still not a crash report. */
/* selectany on MSVC, weak on GCC: the same "this file and fault_probe.h both
   define it, the linker folds them into one object, and a target linking either
   alone still resolves it" contract in each toolchain's spelling. */
#ifdef _MSC_VER
extern "C" __declspec(selectany) int port_fault_synthetic = 0;
#else
extern "C" __attribute__((weak)) int port_fault_synthetic = 0;
#endif

/* Is this actor currently frozen? Either it is in the instance set, or its
   whole class is latched off. Cheap linear scan -- the set is tiny and this
   runs once per node per phase.

   THE GUARD (playlog 041729, the level 6 -> 2 star-exit warp): this check runs
   BEFORE the __try in port_dispatch_guarded, so the id read below was the one
   unguarded dereference in the whole dispatch. A scene node whose owner word
   had been stomped (0x62980, a garbage low pointer) faulted right here and
   took the process down with "reason exception" -- bypassing the quarantine
   net that exists precisely to survive a bad receiver. port_q_actor_id, the
   sibling that reads the same field for the log line, has carried this exact
   IsBadReadPtr guard since it was written. With the guard, a torn pointer
   reads as not-frozen, faults INSIDE the __try on dispatch, and is frozen by
   the net like any other bad actor; under SM64DS_FAULTS_FATAL the filter
   still declines and the proof run dies hard with the full dump.

   The instance-set scan moved ABOVE the guard: it is pure pointer identity,
   so it needs no deref, and it is what makes the freeze stick for a garbage
   pointer -- the first fault puts the bad value in the set, and every later
   frame skips it there instead of re-faulting through the guard's not-frozen
   path once per frame (measured: one quarantine line, not three hundred). */
static int port_q_is_frozen(void *actor)
{
    unsigned id;
    if (!actor)
        return 0;
    for (int i = 0; i < port_q_frozen_n; ++i)
        if (port_q_frozen[i] == actor)
            return 1;
    if (IsBadReadPtr(actor, 0x10))
        return 0;
    id = *(unsigned short *)((char *)actor + 0xc);
    if (id < PORT_Q_IDS && port_q_class_skip[id])
        return 1;
    return 0;
}

static unsigned port_q_actor_id(void *actor)
{
    if (!actor) return 0xffffu;
    /* a torn actor pointer can fault the read; guard it */
    if (IsBadReadPtr(actor, 0x10)) return 0xffffu;
    return *(unsigned short *)((char *)actor + 0xc);
}

/* Append one line to quarantine.log. Raw stdio is fine here -- we are NOT
   inside the faulting context any more (the __except body runs on a unwound
   stack), and the rich dump has already been written by the filter. */
static void port_q_log(void *actor, unsigned id, unsigned code, unsigned off)
{
    static char path[300];
    const char *dir;
    FILE *f;
#ifdef _WIN32
    SYSTEMTIME st;
#else
    /* Linux: mirror SYSTEMTIME's fields so the fprintf below is byte-identical. */
    struct { unsigned short wYear, wMonth, wDay, wHour, wMinute, wSecond; } st;
#endif
    if (port_fault_synthetic)
        return;      /* harness-raised: the crash directory never hears about it */
    dir = port_crash_dir_get();
    if (!dir || !dir[0])
        return;
    std::snprintf(path, sizeof path, "%s\\quarantine.log", dir);
    f = std::fopen(path, "a");
    if (!f) return;
#ifdef _WIN32
    GetLocalTime(&st);
#else
    {
        time_t t = time(0);
        struct tm tmv;
        localtime_r(&t, &tmv);
        st.wYear   = (unsigned short)(tmv.tm_year + 1900);
        st.wMonth  = (unsigned short)(tmv.tm_mon + 1);
        st.wDay    = (unsigned short)tmv.tm_mday;
        st.wHour   = (unsigned short)tmv.tm_hour;
        st.wMinute = (unsigned short)tmv.tm_min;
        st.wSecond = (unsigned short)tmv.tm_sec;
    }
#endif
    std::fprintf(f,
        "%04u-%02u-%02u %02u:%02u:%02u level %d actor %p id %u (%s) "
        "code %08x off +%08x\n",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
        (int)data_0209f2f8, actor, id, port_q_class(id), code, off);
    std::fclose(f);
}

/* Called from the __except body after the filter has written the dump: freeze
   the actor, bump the class count, latch the class off on the second hit. */
static void port_quarantine_actor(void *actor, unsigned code, unsigned off)
{
    unsigned id = port_q_actor_id(actor);
    int class_latched_now = 0;
    if (id < PORT_Q_IDS) {
        if (port_q_class_count[id] < 255) ++port_q_class_count[id];
        if (port_q_class_count[id] >= 2 && !port_q_class_skip[id]) {
            port_q_class_skip[id] = 1;      /* rate limit: whole class off */
            class_latched_now = 1;
        }
    }
    /* Freeze this instance. It used to be skipped once the class was latched,
       on the reasoning that a latched class is covered class-wide and needs no
       per-instance slot. It is covered for DISPATCH, which is what
       port_q_is_frozen answers -- but the level teardown also has to be able to
       name the individual objects it is reaping, and the freeze set is the only
       record of them. Recording it costs one pointer and closes the asymmetry
       that reopened the soft-lock on the SECOND fault of a class. */
    if (port_q_frozen_n < PORT_Q_MAX) {
        /* de-dup */
        int have = 0;
        for (int i = 0; i < port_q_frozen_n; ++i)
            if (port_q_frozen[i] == actor) { have = 1; break; }
        if (!have)
            port_q_frozen[port_q_frozen_n++] = actor;
    }
    port_q_log(actor, id, code, off);
    /* The TESTHOOK suffix is inside the #if, not behind a runtime test on
       port_fault_synthetic. A runtime test still compiles the literal into the
       binary, so a shipped build carried the string "TESTHOOK" in its .rdata
       for a hook that is not in it -- text that reads, to anyone who greps a
       release, as evidence that test hooks are enabled. The flag itself has to
       stay outside the #if because fault_probe.h reads it in every build; only
       the string goes. */
#if PORT_TEST_HOOKS
    std::fprintf(stderr,
        "[quarantine] actor %p id %u (%s) faulted (code %08x +%08x) -- "
        "FROZEN, frame continues%s%s\n",
        actor, id, port_q_class(id),
        code, off, class_latched_now ? " (class latched off, rate-limited)"
                                     : "",
        port_fault_synthetic ? " [TESTHOOK -- forced, nothing written to the "
                               "crash directory]" : "");
#else
    std::fprintf(stderr,
        "[quarantine] actor %p id %u (%s) faulted (code %08x +%08x) -- "
        "FROZEN, frame continues%s\n",
        actor, id, port_q_class(id),
        code, off, class_latched_now ? " (class latched off, rate-limited)"
                                     : "");
#endif
    std::fflush(stderr);
    port_fault_synthetic = 0;
}

/* The SEH filter: write the rich dump (once, with full context) and decide
   whether to swallow the fault. Under SM64DS_FAULTS_FATAL it declines so the
   fault propagates to the UEF and the process dies. It records the code and
   module-relative offset into the out params for the log line. */
#ifdef _WIN32
static int port_q_filter(EXCEPTION_POINTERS *ep, unsigned *code, unsigned *off)
{
    char *base = (char *)GetModuleHandleA(0);
    *code = ep->ExceptionRecord->ExceptionCode;
    *off = (unsigned)((char *)ep->ExceptionRecord->ExceptionAddress - base);
    /* the rich dump: full context, tagged so the file names the mechanism.
       The VEH already wrote crash.txt for this exception; this adds the rolling
       rich dump with the actor context. Under FAULTS_FATAL the tag says so.

       TWO reasons to write no file, and they are independent.

       A fault the test hook manufactured is not a crash report at all: see
       port_fault_synthetic. There is no "testhook" tag any more because there
       is no file to tag -- a dump that is not written cannot be misread.

       And a NAMED decline whose target is already frozen writes no SECOND dump.
       Attribution keeps the player alive, which means he can walk back and hit
       the same dead brick again, and every hit re-raises -- the raise cannot be
       suppressed (the trap thunks' `ret` is only safe because it is
       unreachable), but the FILE it produces can be, and must be: these dumps
       upload into the live player index. The freeze and the unwind are
       unchanged; only the write is skipped. */
    if (!port_fault_synthetic &&
        !(port_q_decline_target && port_q_is_frozen(port_q_decline_target)))
        port_rich_dump_ex(ep, *code,
                          port_faults_fatal() ? "quarantine-fatal"
                                              : "quarantine");
    if (port_faults_fatal())
        return EXCEPTION_CONTINUE_SEARCH;   /* let it die hard */
    return EXCEPTION_EXECUTE_HANDLER;       /* swallow -> quarantine + continue */
}
#else
/* Linux: the filter has no SEH to run in this lane (port_dispatch_guarded is a
   passthrough). Kept as a never-called stub so references still resolve; a real
   signal-based filter is a later lane. Use the forward-declared
   `struct _EXCEPTION_POINTERS *` (line 185) rather than the bare typedef, which
   this TU does not define on Linux; the body never dereferences it anyway. */
static int port_q_filter(struct _EXCEPTION_POINTERS *, unsigned *code,
                         unsigned *off)
{
    if (code) *code = 0;
    if (off)  *off = 0;
    return 0;
}
#endif

/* TEST HOOK -- SM64DS_TEST_QUARANTINE="<actorId>[@<frame>][,<actorId>@<frame>]".
   No behaviour unless the env var is set. When set, this force-freezes a live
   actor whose id (the u16 at +0xc) matches <actorId> once the frame loop
   reaches <frame>, by raising the SAME catchable AV a real per-actor decline
   raises (port_actor_slot_decline) from INSIDE port_dispatch_guarded's __try.
   That drives the exact quarantine path a real fault takes: the filter writes
   the dump, the __except body freezes the actor, the walk continues. <frame>
   is compared against port_last_frame (walk_window's frame counter; -1 in a
   harness without one, so this never fires in a bare smoke).

   COMMA-SEPARATED RULES fire independently, each once. That is what lets a
   proof drive the SECOND fault of a class -- "191@30,191@60" freezes one
   PLAYER instance, then trips the class rate limit on the next one -- which
   is the case the class-latch asymmetry above broke and which a
   one-rule-only hook could not reach at all.

   IT IS NOT IN A BUILD A PLAYER RUNS. PORT_TEST_HOOKS defaults to 0 and the
   whole hook -- parser, rule table, env read, call site -- is #if'd out of the
   object file; port_test_quarantine_should_fire compiles to a `return 0` the
   optimiser deletes at the call site. It defaulted ON in the first draft, on
   the reasoning that walk_window is the binary the proof commands drive, which
   is true and is beside the point: build-port.cmd configures
   CMAKE_BUILD_TYPE=Release, so "defaults on" meant a deliberate way to
   force-fault an arbitrary actor id, read out of the environment, shipped in
   every binary anyone builds. A proof needs it; a player must not have it, and
   the proof is the one that gets to pass a flag. Turn it on for a proof build
   with `build-port.cmd -DPORT_TEST_HOOKS=ON` (port/CMakeLists.txt), which also
   prints a build-log warning naming the binary as test-only.

   IT WRITES NOTHING TO THE CRASH DIRECTORY. The fault it raises sets
   port_fault_synthetic for its duration, and every disk artifact on the fault
   path -- the quarantine filter's rolling dump, quarantine.log, and
   fault_probe.h's crash.txt and VEH/UEF dump -- is suppressed while that is
   set. The first draft tagged the files "quarantine-testhook" and wrote them
   anyway; see port_fault_synthetic for why tagging was not enough. The proof
   evidence is the stderr transcript. The default lives at the top of this file
   because port_quarantine_actor, well above here, also has to compile a
   different string under it. */

#if PORT_TEST_HOOKS
/* port_last_frame is walk_window's frame counter. Carry the SAME weak fallback
   and the SAME alternatename directive fault_probe.h carries, rather than
   relying on some other TU in the link having included that header: this file
   is the actor walker and it links into targets (the bare smokes) that have no
   fault_probe.h TU at all. Both definitions are selectany and both directives
   are byte-identical, so a link that has the header too folds them. Without
   this the first target to link the walker without the probe fails with
   LNK2019 on _port_last_frame. */
extern "C" int port_last_frame;
extern "C" __declspec(selectany) int port_fault_no_frame = -1;
#pragma comment(linker, "/alternatename:_port_last_frame=_port_fault_no_frame")

enum { PORT_TESTQ_RULES = 8 };
static int port_test_q_armed = -1;      /* -1 unparsed, 0 off, 1 armed */
static int port_testq_id[PORT_TESTQ_RULES];
static int port_testq_frame[PORT_TESTQ_RULES];
static unsigned char port_testq_fired[PORT_TESTQ_RULES];
static int port_testq_n;

static void port_test_quarantine_reject(const char *why, const char *what)
{
    std::fprintf(stderr, "[testq] IGNORING SM64DS_TEST_QUARANTINE: %s (%s). "
                 "Expected <actorId>[@<frame>] rules, comma separated, e.g. "
                 "\"191@30\" or \"191@30,191@60\"; ids are 0..65535 decimal "
                 "(0x prefix accepted).\n", why, what ? what : "");
    std::fflush(stderr);
}

/* strtol, not atoi. atoi cannot report failure: it answers 0 for any
   non-numeric string, so a typo, a stray quote or an empty value silently
   armed the hook against ACTOR ID 0 -- a real id -- and force-faulted whatever
   that happened to be. Every field is now parsed with an end pointer, range
   checked, and the whole variable is rejected as one rather than half-applied,
   because a half-parsed rule is worse than none. */
static void port_test_quarantine_parse(void)
{
    const char *e = std::getenv("SM64DS_TEST_QUARANTINE");
    port_test_q_armed = 0;
    port_testq_n = 0;
    if (!e || !*e)
        return;
    while (*e && port_testq_n < PORT_TESTQ_RULES) {
        char *end = 0;
        long id, frame = 0;
        while (*e == ' ' || *e == '\t')
            ++e;
        id = std::strtol(e, &end, 0);
        if (end == e) { port_test_quarantine_reject("no actor id", e); return; }
        if (id < 0 || id > 0xffff) {
            port_test_quarantine_reject("actor id out of 0..65535", e);
            return;
        }
        e = end;
        if (*e == '@') {
            const char *fs = e + 1;
            char *fe = 0;
            frame = std::strtol(fs, &fe, 0);
            if (fe == fs) {
                port_test_quarantine_reject("no frame after '@'", e);
                return;
            }
            if (frame < 0) {
                port_test_quarantine_reject("negative frame", e);
                return;
            }
            e = fe;
        }
        port_testq_id[port_testq_n] = (int)id;
        port_testq_frame[port_testq_n] = (int)frame;
        port_testq_fired[port_testq_n] = 0;
        ++port_testq_n;
        if (*e == ',') { ++e; continue; }
        if (*e == 0) break;
        port_test_quarantine_reject("trailing junk", e);
        port_testq_n = 0;
        return;
    }
    if (*e != 0 && port_testq_n >= PORT_TESTQ_RULES) {
        port_test_quarantine_reject("more than 8 rules", e);
        port_testq_n = 0;
        return;
    }
    if (port_testq_n) {
        port_test_q_armed = 1;
        std::fprintf(stderr, "[testq] ARMED with %d rule(s):", port_testq_n);
        for (int i = 0; i < port_testq_n; ++i)
            std::fprintf(stderr, " id %d at frame %d", port_testq_id[i],
                         port_testq_frame[i]);
        std::fprintf(stderr, "  -- THIS IS A TEST HARNESS BUILD (PORT_TEST_HOOKS)"
                     " AND A TEST HARNESS RUN. The faults it raises write "
                     "NOTHING to the crash directory; this transcript is the "
                     "only record.\n");
        std::fflush(stderr);
    }
}

static int port_test_quarantine_should_fire(void *actor)
{
    if (port_test_q_armed == 0)          /* the shipped, unset path */
        return 0;
    if (port_test_q_armed < 0) {
        port_test_quarantine_parse();
        if (port_test_q_armed == 0)
            return 0;
    }
    if (!actor || IsBadReadPtr(actor, 0x10))
        return 0;
    {
        const int id = (int)*(unsigned short *)((char *)actor + 0xc);
        for (int i = 0; i < port_testq_n; ++i) {
            if (port_testq_fired[i] || port_testq_id[i] != id)
                continue;
            if (port_last_frame < port_testq_frame[i])
                continue;
            port_testq_fired[i] = 1;
            /* from here to the __except body every crash artifact is
               suppressed; the raise is ours, not the game's */
            port_fault_synthetic = 1;
            std::fprintf(stderr, "[testq] rule %d: force-quarantine actor %p "
                         "id %d at frame %d (the same AV path a real fault "
                         "takes)\n", i, actor, id, port_last_frame);
            std::fflush(stderr);
            return 1;
        }
    }
    return 0;
}
#else
static int port_test_quarantine_should_fire(void *) { return 0; }
#endif  /* PORT_TEST_HOOKS */

/* Dispatch one actor's phase callback under the quarantine net. Kept in its own
   function because MSVC forbids __try/__except in a function that also needs
   C++ object unwinding; this one has none. Returns 1 if the actor faulted and
   was quarantined, 0 on a clean call. */
static int port_dispatch_guarded(PortListFn fn, void *actor)
{
    unsigned code = 0, off = 0;
    void *victim;
    if (port_q_is_frozen(actor))
        return 1;               /* frozen: never dispatch again (leak until exit) */
    /* Clear before the call, not after: a latch left behind by an earlier
       decline must never be able to blame this actor's unrelated fault on
       somebody else. Only a decline raised inside THIS fn(actor) can set it. */
    port_q_decline_target = 0;
#ifdef _WIN32
    __try {
        /* test-only: raise the real per-actor decline AV so this actor is frozen
           through the exact path a genuine fault would take. No-op unless
           SM64DS_TEST_QUARANTINE is set (see port_test_quarantine_should_fire). */
        if (port_test_quarantine_should_fire(actor))
            RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, 0);
        fn(actor);
    } __except (port_q_filter(GetExceptionInformation(), &code, &off)) {
        /* only reached when the filter chose to swallow (not FAULTS_FATAL).
           A named decline freezes the actor that declined; anything else --
           a real access violation somewhere in this callback -- still freezes
           the actor being walked, which is the only thing we know about it. */
        victim = port_q_decline_target ? port_q_decline_target : actor;
        port_q_decline_target = 0;
        port_quarantine_actor(victim, code, off);
        return 1;
    }
    port_q_decline_target = 0;
    return 0;
#else
    /* TODO(linux-port): the SEH quarantine is a PASSTHROUGH on Linux for Phase 1
       -- a bad actor's fault in fn(actor) takes the whole process down, with no
       per-actor containment. The real translation (a SIGSEGV/SIGBUS handler that
       longjmp()s back here, freezes the actor, and continues the walk) is a
       SEPARATE later lane. For now the game boots and runs; a faulting actor is
       fatal instead of frozen. */
    (void)code; (void)off; (void)port_q_filter; (void)port_quarantine_actor;
    static int said;
    if (!said++)
        std::fprintf(stderr, "[quarantine] Linux passthrough: per-actor fault "
                             "containment is OFF (later lane)\n");
    fn(actor);
    return 0;
#endif
}

/* Public: clear the freeze set + class latches at level exit, so a leaked
   actor's slot is reclaimed when its heap is torn down anyway. Idempotent.

   INVARIANT: the freeze set holds raw actor POINTERS, valid only for the level
   that froze them, so it must be cleared on EVERY path between levels. Called
   from BOTH sides: exit (port_level_teardown, level_change.cpp) and load
   (port_stage_a_boot, level_boot.cpp) -- the load-side call is defensive, so a
   future exit path that bypasses teardown can never carry a stale pointer into
   a level where a new actor reuses the slot and gets wrongly skipped. */
/* How many actors the instance freeze set is holding. The level teardown asks
   so it can say, in the log, how many it reaped and whether the set it is about
   to clear had anything in it -- the difference between a run that proves the
   clear happened and one where the operator has to take it on faith. Reports
   the INSTANCE leg only: the class latch is a rate limit on ids, not a set of
   objects, and there is nothing to reap for it. */
extern "C" int port_quarantine_frozen_count(void)
{
    return port_q_frozen_n;
}

extern "C" void port_quarantine_reset(void)
{
    port_q_frozen_n = 0;
    for (int i = 0; i < PORT_Q_IDS; ++i) {
        port_q_class_count[i] = 0;
        port_q_class_skip[i] = 0;
    }
}

/* Public: is this actor currently frozen by the quarantine net? The level
   teardown asks so it can exclude a frozen actor from the live census it
   pumps to convergence. A frozen actor is NEVER dispatched (port_dispatch_
   guarded returns early for it), so its own CleanupResources can never run
   and it can never unlink itself -- which means a teardown that counts it
   can never reach zero, and the whole level change is declined (the
   "1 actor still live after 16 rounds" soft-lock). Excluding it here lets
   teardown converge; the frozen node is then reaped by name by the
   dangling-node drop at the tail of port_level_teardown, and its pointer is
   cleared from the freeze set by port_quarantine_reset on the same path.

   IT IS port_q_is_frozen, LITERALLY. The first version of this function was a
   second implementation that knew only the INSTANCE leg, and the two
   predicates then disagreed on exactly the case the rate limiter exists for.
   Once a class has faulted twice its latch is set, and from then on every
   instance of that id is frozen by the CLASS leg before dispatch -- it never
   faults, so it never reaches port_quarantine_actor and (before the change
   above) never entered the instance set. The dispatch side skipped it, the
   teardown side counted it, it never unlinked, and the teardown declined
   again: the same soft-lock the reap was written to remove, reachable from the
   second fault of any class onward. Delegating instead of duplicating is what
   makes that unrepresentable. The instance scan inside is pure pointer
   identity and runs before the guarded id read, so this is still safe to call
   on a torn actor pointer. */
extern "C" int port_quarantine_is_frozen(void *actor)
{
    return port_q_is_frozen(actor);
}

/* {head, tail, callback, 0}; node is {prev, next, owner, ...} */
void *func_02043fdc(void *listv)
{
    int *list = (int *)listv;
    PortListFn fn = (PortListFn)(size_t)list[2];
    int *node;
    if (fn == 0)
        return (void *)1;
    node = (int *)(size_t)list[0];
    while (node != 0) {
        int *next;
        data_020a4b68[0] = (int)(size_t)node;
        next = (int *)(size_t)node[1];
        /* per-actor quarantine boundary: a fault in this one actor's phase
           callback is caught, the actor frozen, and the walk continues at
           `next` -- which was read BEFORE the callback (matched behaviour: a
           Process that unlinks the node under itself is why). */
        port_dispatch_guarded(fn, (void *)(size_t)node[2]);
        port_fader_watch((void *)(size_t)node[2]);
        node = next;
    }
    data_020a4b68[0] = 0;
    return (void *)1;
}

/* {head, callback, 0}; scene node is 0x14 bytes with the owner at +0x10 */
void *func_020441cc(void *listv)
{
    int *list = (int *)listv;
    PortListFn fn = (PortListFn)(size_t)list[1];
    int *node;
    if (fn == 0)
        return (void *)1;
    node = (int *)(size_t)list[0];
    while (node != 0) {
        int *next = (int *)func_0203b394(node);
        /* the scene-tree pass dispatches on the owner at node[4]; same net.
           The successor comes from func_0203b394 and is read before dispatch,
           so a quarantined actor does not strand the walk. */
        port_dispatch_guarded(fn, (void *)(size_t)node[4]);
        node = next;
    }
    return (void *)1;
}

}
