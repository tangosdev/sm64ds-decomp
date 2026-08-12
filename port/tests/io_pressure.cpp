// THE NEGATIVE CONTROL for the fixed-address reservation in ntr/io.cpp.
//
// A fix for a race you cannot make fail is not a fix. This file deliberately
// takes the DS main RAM range before the game asks for it, so the failure a
// player actually hit can be reproduced on demand, and so the change can be
// shown to survive the same pressure the old timing died under.
//
// It is inert unless SM64DS_IO_PRESSURE_AT is set, and nothing in the tree sets
// it. The switches:
//
//   SM64DS_IO_PRESSURE_AT=tls      take the range from a TLS callback in
//                                  .CRT$XLB, which the loader runs BEFORE
//                                  ntr/io.cpp's own callback in .CRT$XLC. This
//                                  is the case no startup ordering can win, and
//                                  it is how the failure reporting gets tested.
//   SM64DS_IO_PRESSURE_AT=static   take it from a static initialiser, which
//                                  runs after every TLS callback and before
//                                  main. This models the real thing: a module
//                                  or an allocation that lands in the range
//                                  during startup. The old timing (reserving
//                                  from main) loses to this; the new timing
//                                  does not.
//   SM64DS_IO_PRESSURE_BASE=hex    default 02000000, the address in the report.
//   SM64DS_IO_PRESSURE_BYTES=hex   default 800000. Eight megabytes from
//                                  0x02000000 covers both main memory and the
//                                  shared system block at 0x027ff000, which is
//                                  the shape the player's dump implies: it
//                                  printed the main RAM line AND then failed
//                                  io_init, and main RAM alone is not fatal.
//   SM64DS_IO_PRESSURE_HOLD_MS=n   release the range n milliseconds after
//                                  taking it, from a helper thread. This is the
//                                  transient occupant, the one the retry exists
//                                  for. Without it the range is held for good.
//
// Pre-CRT discipline: the TLS path runs before the CRT is initialised, so this
// file uses raw Win32 only. No getenv, no strtoul, no printf. The results are
// left in globals for walk_window to print once the CRT is up.
#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" {
/* 0 = never armed, 1 = took it at the TLS callback, 2 = at static init */
unsigned port_io_pressure_when = 0;
/* what it asked for, and whether it got it */
unsigned port_io_pressure_base = 0;
unsigned port_io_pressure_size = 0;
unsigned port_io_pressure_got = 0;
/* non-zero when a helper thread was armed to release it again */
unsigned port_io_pressure_hold_ms = 0;
}

namespace {

/* hex, no CRT. Returns the default when the variable is unset or unparsable. */
unsigned env_hex(const char *name, unsigned dflt) {
    char v[32];
    DWORD n = GetEnvironmentVariableA(name, v, sizeof v);
    if (n == 0 || n >= sizeof v) return dflt;
    unsigned out = 0;
    bool any = false;
    for (DWORD i = 0; i < n; ++i) {
        char c = v[i];
        unsigned d;
        if (c >= '0' && c <= '9') d = (unsigned)(c - '0');
        else if (c >= 'a' && c <= 'f') d = (unsigned)(c - 'a') + 10;
        else if (c >= 'A' && c <= 'F') d = (unsigned)(c - 'A') + 10;
        else break;
        out = out * 16 + d;
        any = true;
    }
    return any ? out : dflt;
}

unsigned env_dec(const char *name, unsigned dflt) {
    char v[32];
    DWORD n = GetEnvironmentVariableA(name, v, sizeof v);
    if (n == 0 || n >= sizeof v) return dflt;
    unsigned out = 0;
    bool any = false;
    for (DWORD i = 0; i < n; ++i) {
        if (v[i] < '0' || v[i] > '9') break;
        out = out * 10 + (unsigned)(v[i] - '0');
        any = true;
    }
    return any ? out : dflt;
}

/* "tls" or "static", matched without the CRT. 0 when unset or anything else. */
int env_stage() {
    char v[16];
    DWORD n = GetEnvironmentVariableA("SM64DS_IO_PRESSURE_AT", v, sizeof v);
    if (n == 0 || n >= sizeof v) return 0;
    if (n == 3 && v[0] == 't' && v[1] == 'l' && v[2] == 's') return 1;
    if (n == 6 && v[0] == 's' && v[1] == 't' && v[2] == 'a' &&
        v[3] == 't' && v[4] == 'i' && v[5] == 'c') return 2;
    return 0;
}

void *g_block;

DWORD WINAPI release_thread(LPVOID) {
    /* Started from a TLS callback this thread cannot run until the loader lock
       is released, so the release always lands after process startup. That is
       the point: it is what gives io_init's later retry something to win. */
    Sleep(port_io_pressure_hold_ms);
    if (g_block) {
        VirtualFree(g_block, 0, MEM_RELEASE);
        g_block = 0;
    }
    return 0;
}

void take_it(int stage) {
    if (port_io_pressure_when) return;             /* once */
    if (env_stage() != stage) return;

    port_io_pressure_base = env_hex("SM64DS_IO_PRESSURE_BASE", 0x02000000u);
    port_io_pressure_size = env_hex("SM64DS_IO_PRESSURE_BYTES", 0x00800000u);
    port_io_pressure_when = (unsigned)stage;

    /* BYTES=0 means "do not allocate anything", which is how the DLL squatter
       below is used on its own. */
    if (port_io_pressure_size) {
        g_block = VirtualAlloc((void *)(uintptr_t)port_io_pressure_base,
                               port_io_pressure_size,
                               MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        port_io_pressure_got = g_block ? 1u : 0u;
    }

    /* SM64DS_IO_PRESSURE_DLL=<path>: take the address with a real mapped IMAGE
       instead of an allocation, by loading a DLL that was linked to a fixed base
       inside the range. That is the case the failure report has to name, because
       a file name is something a player can act on. Static stage only: calling
       LoadLibrary from a TLS callback would re-enter the loader while it is
       calling us. */
    if (stage == 2) {
        char dll[MAX_PATH];
        DWORD dn = GetEnvironmentVariableA("SM64DS_IO_PRESSURE_DLL", dll,
                                           sizeof dll);
        if (dn > 0 && dn < sizeof dll) LoadLibraryA(dll);
    }

    port_io_pressure_hold_ms = env_dec("SM64DS_IO_PRESSURE_HOLD_MS", 0);
    if (g_block && port_io_pressure_hold_ms) {
        DWORD tid;
        HANDLE h = CreateThread(0, 0, release_thread, 0, 0, &tid);
        if (h) CloseHandle(h);
    }
}

/* static initialiser: after every TLS callback, before main */
struct StaticPressure { StaticPressure() { take_it(2); } };
StaticPressure g_static_pressure;

}  // namespace

extern "C" void NTAPI port_io_pressure_tls(PVOID, DWORD reason, PVOID) {
    if (reason == DLL_PROCESS_ATTACH) take_it(1);
}

/* .CRT$XLB sorts before ntr/io.cpp's .CRT$XLC, so this thief runs first. That
   ordering is the only reason the "no startup timing can win this" case is
   reproducible at all. */
#pragma section(".CRT$XLB", long, read)
extern "C" __declspec(allocate(".CRT$XLB"))
    PIMAGE_TLS_CALLBACK port_io_pressure_tls_entry = port_io_pressure_tls;

#ifdef _WIN64
#pragma comment(linker, "/INCLUDE:_tls_used")
#pragma comment(linker, "/INCLUDE:port_io_pressure_tls_entry")
#else
#pragma comment(linker, "/INCLUDE:__tls_used")
#pragma comment(linker, "/INCLUDE:_port_io_pressure_tls_entry")
#endif

#endif  // _WIN32
