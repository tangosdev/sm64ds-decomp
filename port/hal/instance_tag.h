/* PER-INSTANCE FILE NAMES. Run mg16, lane MP2.
 *
 * MP2 runs TWO COPIES OF THE GAME ON ONE MACHINE. Every file the port writes
 * was named on the assumption that there is only one, and a survey of the tree
 * found the collisions fall into three groups with three different answers:
 *
 *   1. WORKING-DIRECTORY RELATIVE -- playlog/, walk_window_selftest.bmp,
 *      sub_screen.bmp, the tex_*.ppm dumps. ALREADY SEPARABLE: give each
 *      instance its own working directory and they cannot collide. No code
 *      change, and the launcher does it.
 *
 *   2. %TEMP% RELATIVE -- the rich crash dumps under %TEMP%\sm64ds-crashes.
 *      The file names already carry a pid so they do not collide, but the
 *      directory is PRUNED TO FOUR AT EVERY BOOT (fault_probe.h), so instance
 *      two deletes instance one's dumps. ALSO ALREADY SEPARABLE: give each
 *      instance its own TEMP. No code change, and the launcher does it.
 *
 *   3. EXE-DIRECTORY RELATIVE -- startup_error.txt and savestate.bin here,
 *      plus crash.txt and exit.txt which are NOT handled (see the banner
 *      below). THESE ARE THE ONLY ONES THAT NEED CODE, because both instances
 *      are the same exe in the same folder and no amount of working directory
 *      or TEMP separates them. savestate.bin is the dangerous one:
 *      port/tests/smoke_persist.cpp:249 already names the exact hazard, "a
 *      separate process picking up the first one's savestate.bin".
 *
 * So this is the smallest change that makes group 3 safe, and it is one env
 * var and one suffix:
 *
 *   SM64DS_INSTANCE=p1   ->  startup_error.p1.txt, savestate.p1.bin, and
 *                            "[p1] " in front of the window title
 *   (unset)              ->  every name is EXACTLY what it has always been.
 *
 * HEADER-ONLY AND static ON PURPOSE. fault_probe.h is included by targets that
 * do not link the hal library, so a real TU here would be a link dependency
 * for the sake of one environment read. Each including TU gets its own cache of
 * the same immutable answer.
 *
 * ==========================================================================
 * WHAT IS DELIBERATELY *NOT* SUFFIXED, AND THE MEASUREMENT THAT DECIDED IT:
 * crash.txt AND exit.txt. DO NOT ADD THEM WITHOUT READING THIS.
 * ==========================================================================
 *
 * Those two are written by port/tests/fault_probe.h, and they belong in group
 * 3 by every argument above. They are still unsuffixed because EDITING
 * fault_probe.h AT ALL COST THE PORT ITS FIXED ADDRESS RANGES AT STARTUP --
 * on every path, with no instance set and the transport switched off:
 *
 *   [io] could not reserve the fixed DS ranges (first tried at process start,
 *        before the CRT, 8 attempt(s))
 *   [io]   LOST 02000000..02400000  main memory
 *   [io]   LOST 027ff000..02800000  the shared system block (required)
 *   [io]   in 02000000: 02000000..02da1000 reserved -> a mapped file with no
 *                       readable name
 *
 * BISECTED in one worktree, one build directory, within a few minutes:
 *   base commit, MP2 changes stashed ................ 5/5 pass, stage 1, 1 attempt
 *   full MP2 tree, fault_probe.h edited ............. 0/5 pass, 8 attempts, lost
 *   the same tree, ONLY fault_probe.h reverted ...... 5/5 pass, stage 1, 1 attempt
 *   fault_probe.h re-edited using raw Win32 only .... 0/5 pass, 8 attempts, lost
 *
 * SO IT IS NOT THE CRT. The first attempt used <stdlib.h> and getenv(), which
 * port/ntr/io.cpp explicitly forbids on the reservation path ("Raw Win32 only
 * -- the same discipline fault_probe.h keeps"), so that looked like the whole
 * answer. Rewriting this header to use GetEnvironmentVariableA -- kernel32,
 * no CRT state, exactly what io.cpp's own TLS callback uses -- DID NOT FIX IT.
 * Neither did removing the ws2_32 import, nor renaming the new .dsstate
 * section. What correlates is simply whether fault_probe.h was edited.
 *
 * The occupant's SIZE MOVES BETWEEN BUILDS (13.9, 14.3, 14.4 and 15.5 MB were
 * all observed), which says its placement depends on image layout rather than
 * on anything semantic in the edit. io.cpp's own doctrine already describes
 * this hazard: "Anything the process allocates before we ask ... can land in
 * the low address space and take part of a range ... down to load order and
 * ASLR." This lane found a build-layout-sensitive instance of it and could not
 * root-cause it further; ROOT-CAUSING IT IS ITS OWN LANE.
 *
 * THE CONSEQUENCE, stated plainly so nobody discovers it in a crash triage:
 * two instances on one machine SHARE crash.txt and exit.txt, so the second
 * instance to fault overwrites the first instance's report. Everything else in
 * group 3 is separated, and the two-window script gives each instance its own
 * TEMP, so the RICH crash dumps under %TEMP%\sm64ds-crashes are per-instance
 * and are the ones to read when two copies are running.
 *
 * The environment read below is raw Win32 anyway. It is the right discipline
 * for a header that was meant to reach the crash path -- where the CRT may
 * already be the thing that is broken -- and it costs nothing to keep.
 *
 * THE SANITISER IS NOT DECORATION. This value comes from the environment and
 * lands in a FILE NAME next to the executable. Anything outside
 * [A-Za-z0-9_-] is dropped, so no separator, no colon, no dot and no ".."
 * can travel through it -- a tag cannot aim a write at another directory. It
 * is truncated to kPortInstanceTagMax as well, so it cannot run a MAX_PATH
 * buffer off its end.
 */

#ifndef PORT_HAL_INSTANCE_TAG_H
#define PORT_HAL_INSTANCE_TAG_H

/* windows.h only. NO <stdlib.h>: see the banner above -- a CRT call reachable
   from fault_probe.h costs the port its fixed address ranges. */
#include <windows.h>

enum { kPortInstanceTagMax = 15 };

/* "" when SM64DS_INSTANCE is unset or sanitises away to nothing, otherwise
   ".<tag>" ready to be pasted in front of a file extension. */
static const char *port_instance_tag(void)
{
    static char tag[kPortInstanceTagMax + 2];
    static int  done = 0;
    char raw[64];
    DWORD got;
    const char *e;
    int n;

    if (done) return tag;
    done = 1;
    tag[0] = 0;

    /* Raw Win32. Returns 0 when unset, and the needed size (>= sizeof raw)
       when the value is too long -- either way there is nothing to use. */
    got = GetEnvironmentVariableA("SM64DS_INSTANCE", raw, (DWORD)sizeof raw);
    if (got == 0 || got >= (DWORD)sizeof raw) return tag;

    e = raw;
    tag[0] = '.';
    n = 1;
    for (; *e && n <= kPortInstanceTagMax; ++e) {
        const char c = *e;
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-')
            tag[n++] = c;
    }
    tag[n] = 0;
    if (n == 1) tag[0] = 0;      /* the tag was entirely punctuation */
    return tag;
}

#endif  /* PORT_HAL_INSTANCE_TAG_H */
