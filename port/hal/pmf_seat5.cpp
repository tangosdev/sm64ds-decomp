/* run link100, lane SEAT5 -- the bridge file for census batch B5 (PMF-SINGLES).
 *
 * Two kinds of line live here and nothing else:
 *
 *  1. THE FLAT C NAME for a seated row whose matched TU is a real C++ member.
 *     src/_ZN5Unagi8BehaviorEv.cpp compiles to ?Behavior@Unagi@@QAEHXZ while the
 *     actor class table calls _ZN5Unagi8BehaviorEv, so the port needs the same
 *     one-line forwarder lane FWD writes in port/hal/fwd_forwarders.cpp for the
 *     twelve PMFB7 Behaviors. Kept in this lane's own file so the fold is
 *     additive.
 *
 *  2. THE C++-LINKAGE DATA SPELLINGS a seated row needs. A //cpp TU whose
 *     `extern ...;` declarations sit at file scope OUTSIDE decl_common.h's
 *     extern "C" block gets each one MANGLED WITH THE TYPE THAT TU CHOSE, while
 *     the overlay mount emits ONE C-named object per symbol. Every LHS below is
 *     a mangled name defined nowhere in the link, read off the linker's own
 *     LNK2019 lines / the TU's own /FAsc EXTRN list
 *     (runs/link100/out/SEAT5/emit_seat5_out.txt), so no alias can be defeated
 *     and alternatename_guard has nothing to refuse. The ov030 / ov032 / ov034 /
 *     ov047 / ov055 recipe.
 */
#include "pmf_seat5.h"
#include "Unagi.h"

/* src/_ZN5Unagi8BehaviorEv.cpp: `extern void* data_ov016_02114dbc;` at file
   scope -> ?data_ov016_02114dbc@@3PAXA. The cell is ov016 bss; the mount emits
   _data_ov016_02114dbc. */
#pragma comment(linker, "/alternatename:?data_ov016_02114dbc@@3PAXA=_data_ov016_02114dbc")

/* src/func_ov102_021498e0.cpp: `extern PMF data_ov102_0214e870[][4];` and its
   sibling, at file scope -> the array-of-4-pointers-to-member spellings below.
   Both tables are ov102 bss; the mount emits the C names.
   (?data_02082214@@3PAFA and ?data_0209f2f8@@3CA, the other two mangled
   spellings that TU needs, are ALREADY bound in port/hal/cxx_aliases.cpp:744
   and :746 -- measured, not assumed, so they are not repeated here.) */
#pragma comment(linker, "/alternatename:?data_ov102_0214e870@@3PAY03P8C@@AEXXZA=_data_ov102_0214e870")
#pragma comment(linker, "/alternatename:?data_ov102_0214e8c0@@3PAY03P8C@@AEXXZA=_data_ov102_0214e8c0")

extern "C" int _ZN5Unagi8BehaviorEv(void *self)
{
    return ((Unagi *)self)->Unagi::Behavior();
}

/* ---------------------------------------------------------------------------
 * THE DISPATCH CENSUS (run link100's DISPATCH CHANGES rule). Every face this
 * lane installs bumps one counter here, and the whole table is printed once per
 * process at exit -- but ONLY when SM64DS_SEAT5_LOG names a file, so an
 * ordinary battery or proof run sees no extra output at all. This is PMFB8's
 * SM64DS_C07E8_LOG pattern and intro_kuppa_dispatch.cpp's kuppa_census_dump
 * pattern, one table for the three rows because they land in one gate.
 *
 * A row whose counter is ZERO on a run that reaches its actor is the finding
 * that says the face is NOT the thing being dispatched; a row with a positive
 * counter and a clean run is the seat working through the ROM's own table.
 * ------------------------------------------------------------------------- */
#include <cstdio>
#include <cstdlib>

static unsigned g_seat5_counts[PORT_SEAT5_COUNTERS];
static const char *const g_seat5_names[PORT_SEAT5_COUNTERS] = {
    "unagi_d8c_main_021119ec", "unagi_d9c_main_02111758",
    "unagi_dac_main_021118b4", "unagi_dbc_main_021115c0",
    "unagi_d7c_main_02111534",
    "qb_021494cc", "qb_02149478", "qb_02149428", "qb_02149384",
    "qb_021493dc", "qb_021492d4", "qb_02149288", "qb_02149220",
    "mirrorluigi_tick_02111288",
};

extern "C" void port_seat5_count(int i)
{
    if ((unsigned)i < (unsigned)PORT_SEAT5_COUNTERS)
        ++g_seat5_counts[i];
}

static void seat5_census_dump(void)
{
    const char *path = std::getenv("SM64DS_SEAT5_LOG");
    if (!path)
        return;
    std::FILE *f = std::fopen(path, "a");
    if (!f)
        f = stderr;
    std::fprintf(f, "[seat5] scene=%s level=%s",
                 std::getenv("SM64DS_SCENE") ? std::getenv("SM64DS_SCENE") : "-",
                 std::getenv("SM64DS_LEVEL") ? std::getenv("SM64DS_LEVEL") : "-");
    for (int i = 0; i < PORT_SEAT5_COUNTERS; ++i)
        std::fprintf(f, " %s=%u", g_seat5_names[i], g_seat5_counts[i]);
    std::fputc(10, f);
    if (f != stderr)
        std::fclose(f);
}

namespace {
struct Seat5CensusInstall {
    Seat5CensusInstall() { std::atexit(seat5_census_dump); }
};
Seat5CensusInstall g_seat5_census_install;
}  /* namespace */
