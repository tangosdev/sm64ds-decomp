/* run link100 wave 7, lane B1SEAT -- the four CleanupResources seats.
 *
 * WHAT THIS FILE WAS COMMISSIONED FOR, AND WHAT IT ACTUALLY HOLDS
 *
 * The lane brief expected this file to DEFINE, at ROM span and inside the
 * .dsstate bracket, the nine .bss SharedFilePtr cells the four seated bodies
 * release. It defines none of them, because all nine are already defined and
 * already at ROM span: each of the four overlays carries a per-symbol mount
 * (port/ov045_syms.txt, port/ov063_syms.txt, port/ov022_syms.txt,
 * port/ov060_syms.txt) that emits the named arrays, and the host thunks this
 * lane retires were already releasing through those same names. Defining them
 * again here would be a duplicate symbol, not a seat. The lane report pastes
 * the map lines for all nine.
 *
 * The reason the port could not bind them BEFORE was never storage. It was the
 * spelling in src/: the four TUs named their file pointers with the delinker's
 * POSITIONAL placeholders G0/G1/G2, which are single global names, so
 * hal/cxx_aliases.cpp had bound one ?G0@@3PAHA / ?G1@@3PAHA pair for the whole
 * binary and pointed it at SignPost's ov002 pointers. Linked as written,
 * PoleLift's teardown would have Released SignPost's live files. The four TUs
 * now name their own module's cells, which decl_common.h declares inside its
 * extern "C" block, so they arrive as the plain C names the mounts define.
 *
 * WHAT IS LEFT IS THE DISPATCH PROOF. Moving a vtable word off a host thunk
 * onto a matched TU is exactly the change class the battery, tail2 and the six
 * proofs are blind to (COMMON's DISPATCH CHANGES block: CTOR2's C1c shipped a
 * call to address 0 through every one of them). So each of the four seated
 * faces calls the probe below and a muted capture of that class's own level
 * shows the line. It is off unless SM64DS_B1SEAT_PROBE=1 is set in the
 * environment, prints once per class, and is the same shape as
 * hal/actor_classes.cpp's SM64DS_ACTOR_PROBE.
 */
#include <cstdio>
#include <cstdlib>

extern "C" void port_b1seat_cleanup_probe(const char *cls)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_B1SEAT_PROBE") != 0;
    if (!on) return;
    /* the four classes plus headroom; the pointers are string literals from
       the four call sites, so identity comparison is the right test */
    static const char *said[8];
    static int n;
    for (int i = 0; i < n; ++i)
        if (said[i] == cls) return;
    if (n < 8) said[n++] = cls;
    std::printf("[b1seat] %s CleanupResources: vtable slot 3 dispatched the "
                "matched TU\n", cls);
    std::fflush(stdout);
}
