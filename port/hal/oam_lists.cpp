// THE OAM SPRITE-TEMPLATE GUARD.
//
// OAM::Render takes an OamAttr* and walks it until it reads a3 == 0xffff:
//
//     ...
//     if (attr->a3 == 0xffff) return;
//     attr++;
//
// There is no other exit for an entry that gets CULLED. The `*pCount >= 0x80`
// guard at the top only stops a list that is EMITTING, and the four cull
// branches (x + w < 0, x > 0x100, y + h < 0, y > 0xc0) take the `attr++;
// continue` path without touching pCount. So a list with no terminator is a
// bounded walk while its sprites are on screen and an UNBOUNDED one the moment
// they go off, and the DS's flat memory is what the walk runs through.
//
// Every one of those lists comes from a POINTER TABLE in overlay data, and on
// the host every one of those pointers has to be rebased off its DS address
// onto the host mount (port/tools/ovdata.py's pointer pass, plus the
// cross-overlay fixups in hal/sub_actors.cpp). A pointer the rebase MISSES
// still holds its DS address, and the port reserves DS main RAM at 0x02000000
// as committed zeroed pages (ntr/io.cpp), so the miss does not trap where it
// happens. It reads as an all-zero OamAttr list -- no terminator anywhere --
// and the fault lands 2.9 MB later at the end of the reservation, inside
// OAM::Render, with nothing on the stack to say which table it came from. If
// the reservation is not mapped at all (VirtualAlloc loses the address to the
// host allocator, which io_init treats as non-fatal) the same miss faults on
// the first read instead.
//
// So: check the tables, once, up front, where the answer is still legible.
// This is a static check with no runtime cost after the first frame, and it
// names the table and the index rather than leaving a bare access violation
// at an address in nobody's module.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

extern "C" {

/* The minimap's, from Minimap::Render. */
extern void *_ZN3OAM15MM_PLAYER_ICONSE[];
extern void *_ZN3OAM18MM_VS_PLAYER_ICONSE[];
extern void *_ZN3OAM20MM_VS_PLAYER_ICONS_SE[];
extern void *_ZN3OAM15MM_STAR_MARKERSE[];
extern void *_ZN3OAM12MM_STAR_KEYSE[];
extern void *data_ov002_0210c748[];
extern void *data_ov002_0210cac8[];
/* The HUD's, from HUD::RenderHealthMeter and the VS timer. */
extern void *data_ov002_0210c230[];
extern void *_ZN3OAM17VS_YELLOW_NUMBERSE[];
/* ov001's digit table, which HUD::RenderCoinCount and its siblings index. */
extern void *_ZN3OAM7NUMBERSE[];

/* the WALKING overload, which is the one every sprite-list caller reaches:
   OAM::RenderSub forwards straight to it. */
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, void *attr, int x,
                                                 int y, int pal, int prio,
                                                 int sx, int sy, int rot,
                                                 int mode);
extern int data_0209e664;      /* the main shadow's entry counter */

int hal_oam_templates_check(void);
int hal_oam_walk_probe(void);

}

namespace {

/* The DS main RAM window the port reserves. A template pointer landing in it
   is a pointer the rebase did not reach: host mounts are ordinary globals and
   live in the image, nowhere near 0x02000000. */
const unsigned kDsLo = 0x02000000u;
const unsigned kDsHi = 0x02400000u;

struct Table {
    const char *name;
    void **tbl;
    int count;          /* entries the render code can actually index */
};

/* Sizes are the ROM's own (next-symbol deltas in config/arm9/overlays/ovNNN/
   symbols.txt), clipped to the range the callers index:
     MM_PLAYER_ICONS   dsd sizes it 76 bytes = 19 words, but Minimap::Render
                       forms `character + capState * 4`, so 16 is the reach.
     MM_STAR_MARKERS   icon = flag + kind * 2, kinds 0..3, so 8.
     MM_STAR_KEYS      sel is 0 or 1.
     0210c748/0210cac8 12 bytes = 3, indexed by data_0209f370[i].
     0210c230          36 bytes = 9, HUD::RenderHealthMeter's segments.
     NUMBERS           80 bytes in ov001; ten digit pointers plus padding. */
const Table kTables[] = {
    {"OAM::MM_PLAYER_ICONS",       _ZN3OAM15MM_PLAYER_ICONSE,     16},
    {"OAM::MM_VS_PLAYER_ICONS",    _ZN3OAM18MM_VS_PLAYER_ICONSE,  16},
    {"OAM::MM_VS_PLAYER_ICONS_S",  _ZN3OAM20MM_VS_PLAYER_ICONS_SE, 16},
    {"OAM::MM_STAR_MARKERS",       _ZN3OAM15MM_STAR_MARKERSE,      8},
    {"OAM::MM_STAR_KEYS",          _ZN3OAM12MM_STAR_KEYSE,         2},
    {"data_ov002_0210c748",        data_ov002_0210c748,            3},
    {"data_ov002_0210cac8",        data_ov002_0210cac8,            3},
    {"data_ov002_0210c230",        data_ov002_0210c230,            9},
    {"OAM::VS_YELLOW_NUMBERS",     _ZN3OAM17VS_YELLOW_NUMBERSE,   10},
    {"OAM::NUMBERS",               _ZN3OAM7NUMBERSE,              10},
};

/* An OamAttr is eight bytes and the fourth halfword is the one the walk tests.
   A real template is a handful of entries; 64 is far past any of them and far
   short of a walk that would leave its own page. */
const int kMaxEntries = 64;

int terminated(const void *p)
{
    const unsigned short *a = (const unsigned short *)p;
    for (int i = 0; i < kMaxEntries; ++i)
        if (a[i * 4 + 3] == 0xffff) return 1;
    return 0;
}

}  // namespace

/* Returns the number of BAD entries, and prints one line per offender.
   Safe to call more than once; it only reports the first time. */
extern "C" int hal_oam_templates_check(void)
{
    static int done, bad;
    if (done) return bad;
    done = 1;

    const int quiet = std::getenv("SM64DS_NO_OAM_CHECK") != 0;
    for (unsigned t = 0; t < sizeof kTables / sizeof kTables[0]; ++t) {
        const Table &T = kTables[t];
        for (int i = 0; i < T.count; ++i) {
            void *e = T.tbl[i];
            const unsigned v = (unsigned)(unsigned long)e;
            const char *why = 0;
            if (e == 0)
                why = "NULL";
            else if (v >= kDsLo && v < kDsHi)
                why = "RAW DS ADDRESS (rebase missed it)";
            else if (!terminated(e))
                why = "NO 0xffff TERMINATOR in 64 entries";
            if (why) {
                ++bad;
                if (!quiet)
                    std::fprintf(stderr, "  [oam] %s[%d] = %08x -- %s\n",
                                 T.name, i, v, why);
            }
        }
    }
    if (!quiet)
        std::fprintf(stderr, "  [oam] sprite templates: %d bad entries\n", bad);
    return bad;
}

/* SM64DS_OAM_WALK_PROBE=1: the fault itself, made deterministic.
 *
 * One zeroed OamAttr list exactly fills a committed page whose successor is
 * RESERVED and unreadable, so a walk that runs off the end faults at a known
 * address instead of 2.9 MB into the DS main-RAM reservation. Both halves of
 * the bug are shown against that one list:
 *
 *   ON SCREEN   every entry emits, the counter reaches 0x80, Render returns.
 *               128 entries of a 512-entry list, so the walk is bounded by the
 *               counter and never nears the page end. This is why a missed
 *               rebase was intermittent rather than constant.
 *   OFF SCREEN  xOff past 0x100 culls every entry, so nothing emits, the
 *               counter never moves, the `*pCount >= 0x80` guard never fires,
 *               and the walk leaves the page.
 *
 * Returns 0 when both halves behave as described. */
#ifndef _WIN32
/* Linux: this probe characterizes an intermittent OAM off-screen walk fault
   using Win32 guard pages (VirtualAlloc/VirtualQuery) and SEH (__try/__except).
   Both are Windows-only; the real signal/mprotect translation is a later lane.
   The probe is a DIAGNOSTIC, not a boot prerequisite, so it is a no-op that
   reports clean here -- the frame loop runs identically without it. */
extern "C" int hal_oam_walk_probe(void)
{
    std::fprintf(stderr, "  [oam] walk probe: skipped on Linux "
                 "(Win32 guard-page/SEH diagnostic; later lane)\n");
    return 0;
}
#else
extern "C" int hal_oam_walk_probe(void)
{
    const int kEntries = 4096 / 8;
    int bad = 0;

    unsigned char *pg =
        (unsigned char *)VirtualAlloc(0, 8192, MEM_RESERVE, PAGE_NOACCESS);
    if (!pg || !VirtualAlloc(pg, 4096, MEM_COMMIT, PAGE_READWRITE)) {
        std::fprintf(stderr, "  [oam] walk probe: no guarded page\n");
        return 1;
    }
    std::memset(pg, 0, 4096);

    const int saved = data_0209e664;

    data_0209e664 = 0;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, pg, 64, 64, 2, 1,
                                                0x1000, 0x1000, 0, -1);
    const int on_screen = data_0209e664;
    if (on_screen != 0x80) ++bad;
    std::fprintf(stderr, "  [oam] walk probe, list at %08lx, %d entries, no "
                 "terminator\n", (unsigned long)pg, kEntries);
    std::fprintf(stderr, "  [oam]   on screen  (x=64):    %d entries emitted, "
                 "returned\n", on_screen);

    data_0209e664 = 0;
    int faulted = 0;
    unsigned long at = 0;
    __try {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, pg, 0x108, 64, 2, 1,
                                                    0x1000, 0x1000, 0, -1);
    } __except (at = (unsigned long)GetExceptionInformation()
                         ->ExceptionRecord->ExceptionInformation[1],
                GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
                    ? EXCEPTION_EXECUTE_HANDLER
                    : EXCEPTION_CONTINUE_SEARCH) {
        faulted = 1;
    }
    const int off_screen = data_0209e664;
    std::fprintf(stderr, "  [oam]   off screen (x=0x108): %d entries emitted, "
                 "%s%s%08lx\n", off_screen,
                 faulted ? "ACCESS VIOLATION at " : "returned", "", at);
    if (!faulted || off_screen != 0 ||
        at != (unsigned long)(pg + 4096)) ++bad;

    VirtualFree(pg, 0, MEM_RELEASE);

    /* AND THE REAL ONE. 0x0210c648 is where data_ov002_0210c230[0] pointed
       before the rebase reached it -- the first of HUD::RenderHealthMeter's
       nine sprite lists, as a bare DS address. It is inside the reservation
       ntr/io.cpp maps at 0x02000000, so it reads as committed zeroed pages and
       the walk runs to the end of the region. This is the fault that was
       landing about once in 230 runs, driven through the game's own Render at
       an off-screen offset. */
    {
        void *const was = (void *)0x0210c648u;
        MEMORY_BASIC_INFORMATION mbi;
        const int mapped = VirtualQuery(was, &mbi, sizeof mbi) &&
                           mbi.State == MEM_COMMIT;
        std::fprintf(stderr, "  [oam]   data_ov002_0210c230[0]'s old DS value "
                     "%08lx: %s\n", (unsigned long)was,
                     mapped ? "COMMITTED ZEROED PAGES (the walk runs)"
                            : "not mapped (the walk faults on the first read)");
        data_0209e664 = 0;
        int hit = 0;
        unsigned long real_at = 0;
        __try {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, was, 0x108, 64, 2, 1,
                                                        0x1000, 0x1000, 0, -1);
        } __except (real_at = (unsigned long)GetExceptionInformation()
                                  ->ExceptionRecord->ExceptionInformation[1],
                    GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
                        ? EXCEPTION_EXECUTE_HANDLER
                        : EXCEPTION_CONTINUE_SEARCH) {
            hit = 1;
        }
        std::fprintf(stderr, "  [oam]   walking it off screen: %s%08lx"
                     " (%lu entries in)\n",
                     hit ? "ACCESS VIOLATION at " : "returned, no fault at ",
                     real_at,
                     hit ? (real_at - 0x0210c648u) / 8u : 0u);
        /* Where the walk actually stopped, and what stopped it. The
           reservation runs to 0x02400000 and is zeroed, so nothing inside it
           terminates the list: the walk leaves it. What is on the other side
           is the host allocator's business and differs run to run, which is
           the whole of the intermittency. If the next pages are committed the
           walk finds a halfword that happens to read 0xffff almost at once and
           returns having emitted nothing; if they are not, this is the access
           violation. */
        const unsigned short *a = (const unsigned short *)was;
        long n = 0;
        int scan_hit = 0;
        __try {
            while (a[n * 4 + 3] != 0xffff) ++n;
        } __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
                        ? EXCEPTION_EXECUTE_HANDLER
                        : EXCEPTION_CONTINUE_SEARCH) {
            scan_hit = 1;
        }
        /* THE COIN FLIP. Nothing inside the reservation stops the walk, so
           what decides the run is whether the page at 0x02400000 -- the one
           immediately past it, which belongs to the host allocator and not to
           the port -- happens to be committed. Committed: the walk reads on
           and finds a halfword that reads 0xffff within a few dozen entries,
           returns having emitted nothing, and the frame looks fine. Not
           committed: access violation, inside OAM::Render, 2.9 MB from the
           pointer that caused it. */
        MEMORY_BASIC_INFORMATION past;
        const int past_committed =
            VirtualQuery((void *)0x02400000u, &past, sizeof past) &&
            past.State == MEM_COMMIT;
        std::fprintf(stderr, "  [oam]   reservation ends %ld entries in at "
                     "02400000, page past it %s; %s\n",
                     (long)((0x02400000u - 0x0210c648u) / 8u),
                     past_committed ? "COMMITTED" : "not committed",
                     scan_hit
                         ? "no 0xffff before the walk left mapped memory"
                         : "0xffff found past it");
        if (!scan_hit)
            std::fprintf(stderr, "  [oam]   first 0xffff %ld entries in, at "
                         "%08lx -- %ld PAST the reservation, in whatever the "
                         "host allocator put there\n", n,
                         (unsigned long)(0x0210c648u + (unsigned)n * 8u),
                         n - (long)((0x02400000u - 0x0210c648u) / 8u));
    }

    data_0209e664 = saved;
    std::fprintf(stderr, "  [oam] walk probe: %s\n", bad ? "UNEXPECTED" : "as described");
    return bad;
}
#endif /* _WIN32 */
