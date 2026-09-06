// DID THE ROM'S OWN THREAD CREATION ACTUALLY MAKE A THREAD, AND DID THAT
// THREAD RUN? Run link100, lane THREAD.
//
// ============================ WHY A PROBE ==================================
//
// port/tools/linkage.py will report src/func_02058200.c, src/func_02058538.c
// and src/func_0206002c.c as LINKED whether or not a single instruction of
// them ever executes, and that is exactly the failure this port has been
// bitten by before: a seat that links, never runs, and reads as progress. The
// same objection port/tools/thread_proof.py was written for applies one layer
// down, so this is the same shape of answer -- run the binary and ask whether
// the ROM's own creation produced a thread the ROM's own scheduler then ran.
//
// ============================ THE FOUR RUNGS ===============================
//
// Every rung is a fact about the ROM's own storage or the ROM's own control
// flow. Nothing here fabricates state; the probe reads and then calls one ROM
// function.
//
//   1. CREATED.  The record at data_020a81bc carries what src/func_02058200.c
//      writes, at the offsets it writes them: a slot id whose entry in the
//      ROM's own table data_020a6148 points BACK at the record, a priority,
//      a stack range at +0x84/+0x88 that is exactly `top - 0x400 .. top` with
//      top == &data_020a8760, the two guard words 0x7bf9dd5b and 0xfddb597d
//      at the two ends of that range, an entry pc at +0x40 that is
//      func_020602bc + 4, and &func_020581a8 in the lr word at +0x3c.
//
//      THE GUARD WORDS ARE THE LOAD-BEARING PART OF THIS RUNG. They are the
//      only evidence that the ROM's stack arithmetic landed on the ROM'S OWN
//      BYTES rather than a kilobyte of somebody else's: they can only be at
//      those two addresses if hal/globals_link100.cpp's grouped card span put
//      data_020a8760 exactly 1444 bytes above data_020a81bc, which is what
//      config/arm9/symbols.txt says the DS does.
//
//   2. ENTERED.  hal/boot2_thread.cpp adopted the record off those same words
//      and its fiber trampoline entered src/func_020602bc.cpp. Counted, not
//      inferred: the trampoline is the only thing that increments `entered`.
//
//   3. SLEPT.  The card thread's own first act is OS_SleepThread(0) -- ROM
//      code -- so after it is entered its state word at +0x64 is 0 and its
//      record pointer is parked at data_020a8180 + 0xd0, which is the line
//      src/func_020602bc.cpp writes immediately before the sleep. Control is
//      back on the game context, which is what the ROM's reschedule does with
//      a sleeping highest-priority thread.
//
//   4. WOKEN, THROUGH THE ROM'S RESCHEDULE. The probe calls one function and
//      it is the ROM's: src/func_02058048.c, `IRQ off; state = 1;
//      func_02057f54(); IRQ back`. That is the same call src/func_02060310.c
//      makes when the ARM7 answers a card command, and the same call
//      src/func_020603c8.c and src/func_02060228.c make. func_02057f54 then
//      picks the card thread (priority 4 sorts ahead of the game context's
//      0x10), switches to it, and the card thread's loop sleeps again --
//      so a successful wake shows up as TWO more switches and a state word
//      back at 0.
//
// ============================ THE NEGATIVE CONTROL =========================
//
// SM64DS_THREAD_NOCREATE=1 makes hal/boot2_thread.cpp's adoption refuse. Rung
// 1 still passes -- func_02058200 really did run and really did write the
// record, and the control must not be able to hide that -- and rungs 2, 3 and
// 4 all fail, which is the shape the control has to have: it removes the
// FIBER, not the ROM's creation. Bounded by construction, like every other
// refusal in that file: the reschedule backs out and returns.
//
// ============================ WHERE IT RUNS ================================
//
// hal/boot_os.cpp calls port_thread_create_proof() immediately after the card
// bring-up func_0206002c(), which is where func_02042f68 stands the card
// thread up in the ROM. If that call has not happened -- a target that does
// not run the boot spine, or a boot that stopped earlier -- the probe makes
// the SAME call func_0206002c makes, with func_0206002c's own arguments,
// transcribed from src/func_0206002c.c:43, so both ends stay ROM code either
// way. It says which of the two it did.
//
//   SM64DS_THREAD_CREATE_PROOF=1   run it and print the verdict line
//                                  port/tools/thread_create_proof.py reads
//   SM64DS_THREAD_NOCREATE=1       the negative control (boot2_thread.cpp)

#include <stdint.h>

#include <cstdio>
#include <cstdlib>

extern "C" {

// The ROM's card-driver storage, hosted as one grouped run by
// hal/globals_link100.cpp: the work object, the thread record 0x3c into it,
// and the stack top 1444 bytes above the record.
// Declared unsized on purpose: on the DS these three names are ONE object and
// the ROM's own bodies read across the boundaries (src/func_0206002c.c stores
// at data_020a8180 + 0xd4, which is inside data_020a81bc). hal/
// globals_link100.cpp is what makes that layout real; this file only reads it.
extern unsigned char data_020a8180[];
extern unsigned char data_020a81bc[];
extern unsigned char data_020a8760[];

// The ROM's sixteen-slot thread table (hal/cxx_aliases.cpp's grouped
// OSThreadInfo, data_020a6134 + 0x14).
extern int data_020a6148[16];

// ROM code. Every one of these is a row on port/slice_gate223.txt.
void func_02058200(char *self, int entry, int arg, int stack_top, int size,
                   int prio);
void func_02058048(void *self);
void func_020602bc(void);
void func_020581a8(void);

// hal/boot2_thread.cpp's counters.
void port_thread_sched_counts(unsigned long long *adopted,
                              unsigned long long *entered,
                              unsigned long long *restores,
                              unsigned long long *rejected,
                              unsigned long long *nocreate);
// Put the ROM manager's current-thread word back on the thread that is really
// running. src/func_02057f54.c stores the PICK into data_020a6134 + 8 BEFORE
// it calls ARMRestoreContext, so a refused restore -- which is the whole point
// of the negative control -- leaves that word naming a thread nobody switched
// to. hal/boot2_thread.cpp's own thread_proof repairs the same way and for the
// same reason: the control must not poison the rest of the run.
void port_thread_repair_current(void);

void port_thread_create_proof(void);

}  // extern "C"

namespace {

const uint32_t kGuardBase = 0x7bf9dd5bu;   // func_02058200 pool 0x02058408
const uint32_t kGuardTop  = 0xfddb597du;   // func_02058200 pool 0x02058404
const uint32_t kCardStack = 0x400u;        // src/func_0206002c.c:43
const uint32_t kCardPrio  = 4u;            // data_020a8180.f38, set two lines up

inline uint32_t rec32(unsigned off) {
    return *reinterpret_cast<const uint32_t *>(data_020a81bc + off);
}
inline uint32_t at32(uint32_t addr) {
    return *reinterpret_cast<const uint32_t *>(
        reinterpret_cast<const void *>(static_cast<uintptr_t>(addr)));
}
inline uint32_t addr_of(const void *p) {
    return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p));
}
inline uint32_t fn_addr(void (*f)(void)) {
    return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(f));
}

// Has src/func_02058200.c run over this record? The ROM's own admission test:
// the slot it allocated points back at the record it was given.
bool created() {
    const uint32_t id = rec32(0x6c);
    if (id >= 16) return false;
    return data_020a6148[id] == static_cast<int>(reinterpret_cast<intptr_t>(data_020a81bc));
}

}  // namespace

extern "C" void port_thread_create_proof(void)
{
    if (!std::getenv("SM64DS_THREAD_CREATE_PROOF")) return;

    const char *how = "func_0206002c (the ROM's own card bring-up)";
    if (!created()) {
        // src/func_0206002c.c's last two statements before the PXI
        // registration, verbatim, including the priority that body puts in
        // data_020a8180 + 0x38 two lines earlier. Both are ROM bodies; this
        // path exists so the probe still measures something on a target that
        // does not run hal/boot_os.cpp's boot spine.
        how = "this probe, with func_0206002c's own arguments";
        func_02058200(reinterpret_cast<char *>(data_020a81bc),
                      static_cast<int>(fn_addr(&func_020602bc)),
                      0,
                      static_cast<int>(addr_of(data_020a8760)),
                      static_cast<int>(kCardStack),
                      static_cast<int>(kCardPrio));
        func_02058048(data_020a81bc);
    }

    unsigned long long ad0, en0, sw0, rj0, nc0;
    port_thread_sched_counts(&ad0, &en0, &sw0, &rj0, &nc0);

    // ---- RUNG 1: what the ROM's own creation left in the ROM's own record --
    const uint32_t id    = rec32(0x6c);
    const uint32_t prio  = rec32(0x70);
    const uint32_t lo    = rec32(0x84);
    const uint32_t hi    = rec32(0x88);
    const uint32_t pc4   = rec32(0x40);
    const uint32_t lr    = rec32(0x3c);
    const uint32_t top   = addr_of(data_020a8760);
    const uint32_t want_pc4 = fn_addr(&func_020602bc) + 4;
    const uint32_t want_lr  = fn_addr(&func_020581a8);

    const bool slot_ok  = id < 16 &&
        data_020a6148[id] == static_cast<int>(reinterpret_cast<intptr_t>(data_020a81bc));
    const bool span_ok  = hi == top && lo == top - kCardStack;
    const bool guard_ok = span_ok && at32(lo) == kGuardBase &&
                          at32(hi - 4) == kGuardTop;
    const bool ctx_ok   = pc4 == want_pc4 && lr == want_lr && prio == kCardPrio;
    const bool created_ok = slot_ok && span_ok && guard_ok && ctx_ok;

    std::fprintf(stderr,
                 "[thrc] CREATED by %s: id=%u prio=%u stack=[%08x,%08x) "
                 "top=%08x guards=%08x/%08x pc4=%08x(want %08x) "
                 "lr=%08x(want %08x) slot=%d rung1=%s\n",
                 how, id, prio, lo, hi, top,
                 span_ok ? at32(lo) : 0u, span_ok ? at32(hi - 4) : 0u,
                 pc4, want_pc4, lr, want_lr,
                 id < 16 ? data_020a6148[id] : 0, created_ok ? "OK" : "FAIL");
    std::fflush(stderr);

    // ---- RUNG 2 and 3: entered, and asleep again ---------------------------
    const uint32_t st_before = rec32(0x64);
    const uint32_t parked    = *reinterpret_cast<const uint32_t *>(data_020a8180 + 0xd0);
    const bool entered_ok = en0 >= 1;
    const bool slept_ok   = entered_ok && st_before == 0 &&
                            parked == addr_of(data_020a81bc);

    // ---- RUNG 4: wake it through the ROM's own reschedule ------------------
    func_02058048(data_020a81bc);

    unsigned long long ad1, en1, sw1, rj1, nc1;
    port_thread_sched_counts(&ad1, &en1, &sw1, &rj1, &nc1);
    const unsigned long long switches = sw1 - sw0;
    const uint32_t st_after = rec32(0x64);
    const bool woken_ok = switches >= 2 && st_after == 0;

    const bool all = created_ok && entered_ok && slept_ok && woken_ok;
    std::fprintf(stderr,
                 "[thrc] PROOF created=%d entered=%d slept=%d woken=%d "
                 "adopted=%llu entries=%llu switches=%llu rejected=%llu "
                 "nocreate=%llu state_before=%u state_after=%u parked=%08x "
                 "VERDICT %s\n",
                 (int)created_ok, (int)entered_ok, (int)slept_ok,
                 (int)woken_ok, ad1, en1, switches, rj1, nc1,
                 st_before, st_after, parked,
                 all ? "THREAD-RAN" : (created_ok ? "NOT-ENTERED"
                                                  : "NOT-CREATED"));
    std::fflush(stderr);

    // THE CONTROL'S CLEAN-UP, and only the control's. When the adoption was
    // refused there was no switch, so two ROM writes are left standing that
    // describe a thread which never ran: func_02058048's `state = 1` on the
    // card record, and func_02057f54's store of the pick into the manager's
    // current-thread word. Undo exactly those two -- the card thread's real
    // state is "not runnable, never entered", which is what func_02058200 left
    // -- so the rest of the control run behaves like the port did before this
    // lane instead of rescheduling onto a thread with no fiber every frame.
    if (nc1 > nc0 || rj1 > rj0) {
        *reinterpret_cast<uint32_t *>(
            static_cast<void *>(data_020a81bc + 0x64)) = 0;
        port_thread_repair_current();
        std::fprintf(stderr, "[thrc] control: card thread put back to "
                     "not-runnable, manager current repaired\n");
        std::fflush(stderr);
    }
}
