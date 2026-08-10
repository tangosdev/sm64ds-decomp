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

static int port_faults_fatal(void)
{
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_FAULTS_FATAL") != 0;
    return v;
}

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
    const char *dir = port_crash_dir_get();
    FILE *f;
#ifdef _WIN32
    SYSTEMTIME st;
#else
    /* Linux: mirror SYSTEMTIME's fields so the fprintf below is byte-identical. */
    struct { unsigned short wYear, wMonth, wDay, wHour, wMinute, wSecond; } st;
#endif
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
    /* freeze this instance (option b) unless the class is already latched --
       then it is covered class-wide and needs no per-instance slot */
    if ((id >= PORT_Q_IDS || !port_q_class_skip[id]) &&
        port_q_frozen_n < PORT_Q_MAX) {
        /* de-dup */
        int have = 0;
        for (int i = 0; i < port_q_frozen_n; ++i)
            if (port_q_frozen[i] == actor) { have = 1; break; }
        if (!have)
            port_q_frozen[port_q_frozen_n++] = actor;
    }
    port_q_log(actor, id, code, off);
    std::fprintf(stderr,
        "[quarantine] actor %p id %u (%s) faulted (code %08x +%08x) -- "
        "FROZEN, frame continues%s\n",
        actor, id, port_q_class(id),
        code, off, class_latched_now ? " (class latched off, rate-limited)"
                                     : "");
    std::fflush(stderr);
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
       rich dump with the actor context. Under FAULTS_FATAL the tag says so. */
    port_rich_dump_ex(ep, *code,
                      port_faults_fatal() ? "quarantine-fatal" : "quarantine");
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

/* Dispatch one actor's phase callback under the quarantine net. Kept in its own
   function because MSVC forbids __try/__except in a function that also needs
   C++ object unwinding; this one has none. Returns 1 if the actor faulted and
   was quarantined, 0 on a clean call. */
static int port_dispatch_guarded(PortListFn fn, void *actor)
{
    unsigned code = 0, off = 0;
    if (port_q_is_frozen(actor))
        return 1;               /* frozen: never dispatch again (leak until exit) */
#ifdef _WIN32
    __try {
        fn(actor);
    } __except (port_q_filter(GetExceptionInformation(), &code, &off)) {
        /* only reached when the filter chose to swallow (not FAULTS_FATAL) */
        port_quarantine_actor(actor, code, off);
        return 1;
    }
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
extern "C" void port_quarantine_reset(void)
{
    port_q_frozen_n = 0;
    for (int i = 0; i < PORT_Q_IDS; ++i) {
        port_q_class_count[i] = 0;
        port_q_class_skip[i] = 0;
    }
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
