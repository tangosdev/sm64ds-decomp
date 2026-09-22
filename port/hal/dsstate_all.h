/* Force-included (cl /FI) into the handful of MAIN TRANSLATION UNITS that own
 * hosted DS globals, so every file-scope object they define lands in the
 * captured .dsstate span.
 *
 * Run link100 wave 9c, lane LINK21. THE FIRST BUILD IN THIS CAMPAIGN THAT
 * REACHED A LINK ran dsstate_guard for the first time -- it reads
 * build/port/walk_window.map, which has been zero bytes for every wave -- and it
 * named 32 hosted DS symbols sitting outside [dsstate_lo, dsstate_hi). Thirty
 * one of them are in eleven src/ translation units, and they are there because
 * of the main to port sync rather than because of any lane: port/ov044_syms.txt
 * lines 91-97 and port/ov100_syms.txt lines 1-6 both record retiring the MOUNT
 * of those very objects, because the class TU main folded them into now defines
 * them itself. A mount emits its arrays inside the bracket (tools/ovdata.py puts
 * DSSTATE_BEGIN and DSSTATE_END around them and gives packed slots a $mmmNNNN
 * suffix); an ordinary file-scope definition in a src TU does not. So the
 * objects kept their names, kept their sizes and quietly left the snapshot.
 *
 * WHY A FORCE-INCLUDE AND NOT DSSTATE_BEGIN IN THE SOURCE. The definitions are
 * in src/, which is the byte-verified decomp and is not the port's to edit; the
 * port's own rule is to fix the transform or the host layer instead. /FI is a
 * per-source compile option, so it is set in port/CMakeLists.txt beside every
 * other per-source property and touches no decomp file.
 *
 * WHY IT IS SAFER THAN THE BRACKET, not just easier. hal/dsstate_seg.h's own
 * warning is that a later `extern` re-declaration of a name inside the bracket
 * silently puts that symbol back in ordinary .bss, because MSVC fixes a symbol's
 * segment from the declaration it saw last. Opening the segment for the WHOLE
 * translation unit removes the "back to default" moment that trap needs, so no
 * ordering inside the file can undo it.
 *
 * WHAT IT SWEEPS IN. Every file-scope and function-local static object of the
 * translation unit, not only the DS-named ones. String literals are unaffected
 * (they are .rdata and neither pragma names it). The cost is snapshot size, and
 * it is the same cost the mounts carried before the sync moved these objects.
 *
 * ONLY THE ELEVEN. This header is not included by anything; the list of sources
 * that carry it is the DSSTATE FORCE-INCLUDE block in port/CMakeLists.txt, and
 * dsstate_guard is what says whether the list is still right.
 */
#ifndef PORT_HAL_DSSTATE_ALL_H
#define PORT_HAL_DSSTATE_ALL_H

#include "hal/dsstate_seg.h"

DSSTATE_BEGIN

#endif /* PORT_HAL_DSSTATE_ALL_H */
