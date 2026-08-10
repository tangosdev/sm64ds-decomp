/* HOST COPIES of src/func_ov100_02145550.cpp (daDoor_c::Behavior) and
 * src/func_ov100_021453d8.cpp (the callback installer it shares a table
 * with), plus the seat for the fifteen {function, delta} statics both
 * dispatch. The gate-16 pointer-to-member case for the sixth time, and the
 * first one where the table is a POINTER THE ACTOR CARRIES rather than an
 * index into a fixed array.
 *
 * ---- what the ROM actually does ------------------------------------------
 *
 * The door keeps a CALLBACK NODE pointer at +0x140. Each node is 0x10 bytes
 * and holds two mwcc pointers-to-member: one at +0, which the installer
 * dispatches the moment it seats the node, and one at +8, which Behavior
 * dispatches every frame. __sinit_ov100_02147698 builds nine of those nodes
 * out of the fifteen statics at ov100 0x021480d4..0x02148144 -- plus arm9's
 * data_02086b58, which is the NULL pair (both halves zero, which is what the
 * ROM mounts there), and both dispatch sites test word 0 against zero before
 * they will call.
 *
 * Read off the ROM at 0x02145570 and 0x02145400, the encoding is the pair
 * mwcc always emits:
 *
 *     fn    = word 0        delta = word 1
 *     this' = this + (delta >> 1)
 *     if (delta & 1)  fn is a BYTE OFFSET into this'-> vtable
 *     else            fn is the function address
 *
 * Both branches are spelled out below rather than asserted away. Every one of
 * the fifteen statics carries delta 0 in the ROM, so the virtual half is dead
 * on this level -- but it is one line, and a dead line that is right beats a
 * live assumption.
 *
 * ---- why these two are host copies ----------------------------------------
 *
 * Both source TUs write `struct C;` and form the pointer-to-member while C is
 * INCOMPLETE, which is the case MSVC answers with the four-word general
 * representation. That quadruples the node's stride and dispatches a
 * neighbour's body.
 *
 * ---- and why the statics are seated ---------------------------------------
 *
 * Those fifteen statics are the overlay image's own words -- DS CODE
 * ADDRESSES -- and the sinit copies them into the nodes. The seat rewrites
 * the STATICS before the sinit runs (the LakituBro reading: same guarantee,
 * one less mapping to get wrong), each checked against the ROM address its
 * host body was compiled from. ALL FIFTEEN ARE MATCHED SRC, so none of them
 * is trapped; the one trap at the bottom of this file is a different hole,
 * two calls further in.
 *
 * The dispatch is r0 = this, r1 = the argument, and the fifteen bodies do not
 * agree on how many arguments they admit to taking -- nine take two, six take
 * one. That is the ARM ride-through the port has met all through this gate:
 * the second argument is in r1 whether or not the callee reads it. Under
 * __cdecl the caller cleans the stack, so one call shape serves all fifteen.
 */
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

extern "C" {

void *func_ov100_02145370(void *self);

struct PortPmf { unsigned fn; int delta; };
extern PortPmf data_ov100_021480d4[], data_ov100_021480dc[],
    data_ov100_021480e4[], data_ov100_021480ec[], data_ov100_021480f4[],
    data_ov100_021480fc[], data_ov100_02148104[], data_ov100_0214810c[],
    data_ov100_02148114[], data_ov100_0214811c[], data_ov100_02148124[],
    data_ov100_0214812c[], data_ov100_02148134[], data_ov100_0214813c[],
    data_ov100_02148144[];

int func_ov100_02144730(void *, void *);
int func_ov100_02144ccc(void *, void *);
int func_ov100_02144c6c(void *, void *);
int func_ov100_02144bf4(void *, void *);
int func_ov100_02144c64(void *, void *);
int func_ov100_021449c8(void *, void *);
int func_ov100_02144a38(void *, void *);
int func_ov100_02144468(void *, void *);
int func_ov100_02144950(void *, void *);
int func_ov100_021446f8(void *, void *);
int func_ov100_02144528(void *, void *);
int func_ov100_021444e8(void *, void *);
int func_ov100_02144cf8(void *, void *);
int func_ov100_0214491c(void *, void *);

}  /* extern "C" */

typedef int (*PortDoorFn)(void *, void *);

static const struct { PortPmf *slot; unsigned rom; PortDoorFn host; }
g_door_callbacks[] = {
    {data_ov100_021480d4, 0x02144730, func_ov100_02144730},
    {data_ov100_021480dc, 0x02144ccc, func_ov100_02144ccc},
    {data_ov100_021480e4, 0x02144c6c, func_ov100_02144c6c},
    {data_ov100_021480ec, 0x02144bf4, func_ov100_02144bf4},
    {data_ov100_021480f4, 0x02144c64, func_ov100_02144c64},
    {data_ov100_021480fc, 0x021449c8, func_ov100_021449c8},
    {data_ov100_02148104, 0x02144730, func_ov100_02144730},
    {data_ov100_0214810c, 0x02144a38, func_ov100_02144a38},
    {data_ov100_02148114, 0x02144468, func_ov100_02144468},
    {data_ov100_0214811c, 0x02144950, func_ov100_02144950},
    {data_ov100_02148124, 0x021446f8, func_ov100_021446f8},
    {data_ov100_0214812c, 0x02144528, func_ov100_02144528},
    {data_ov100_02148134, 0x021444e8, func_ov100_021444e8},
    {data_ov100_0214813c, 0x02144cf8, func_ov100_02144cf8},
    {data_ov100_02148144, 0x0214491c, func_ov100_0214491c},
};

extern "C" void port_door_callbacks_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0;
         i < sizeof g_door_callbacks / sizeof g_door_callbacks[0]; ++i) {
        PortPmf *p = g_door_callbacks[i].slot;
        if (p->fn != g_door_callbacks[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Door callback %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_door_callbacks[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_door_callbacks[i].host;
    }
}

/* The dispatch itself, ROM encoding, shared by both call sites below.
   HARDENED 2026-08-07: real play reached the open flow for the first time and
   something dispatched a non-code pointer (arena address, playlog
   play_20260807_232848). Until that write is found, refuse to call anything
   outside the image and name the node instead of wild-calling. */
#ifdef _WIN32
extern "C" IMAGE_DOS_HEADER __ImageBase;
static int port_door_fn_in_image(unsigned fn)
{
    /* .text-only: the first check (whole image) passed arena/bss pointers,
       which is exactly what the stomped node held. Executable sections only. */
    static char *lo, *hi;
    char *base = (char *)&__ImageBase;
    if (!lo) {
        IMAGE_NT_HEADERS32 *nt = (IMAGE_NT_HEADERS32 *)(base +
            ((IMAGE_DOS_HEADER *)base)->e_lfanew);
        IMAGE_SECTION_HEADER *s = IMAGE_FIRST_SECTION(nt);
        for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++s) {
            if (!(s->Characteristics & IMAGE_SCN_MEM_EXECUTE))
                continue;
            char *a = base + s->VirtualAddress;
            char *b = a + s->Misc.VirtualSize;
            if (!lo || a < lo) lo = a;
            if (b > hi) hi = b;
        }
    }
    return (char *)(size_t)fn >= lo && (char *)(size_t)fn < hi;
}
#else
/* Linux: the Win32 PE .text-bounds guard against dispatching a stomped non-code
   pointer maps to a /proc/self/maps r-xp scan. Parsed once, cached. Falls open
   (accept) if maps is unreadable, which matches the pre-hardening behavior. */
#include <cstdio>
static int port_door_fn_in_image(unsigned fn)
{
    static int inited;
    static unsigned long lo, hi;   /* union span of all r-xp ranges */
    if (!inited) {
        inited = 1;
        FILE *m = std::fopen("/proc/self/maps", "r");
        if (m) {
            char line[512];
            while (std::fgets(line, sizeof line, m)) {
                unsigned long a, b;
                char perms[8];
                if (std::sscanf(line, "%lx-%lx %7s", &a, &b, perms) == 3 &&
                    perms[2] == 'x') {
                    if (!lo || a < lo) lo = a;
                    if (b > hi) hi = b;
                }
            }
            std::fclose(m);
        }
    }
    if (!lo) return 1;   /* maps unreadable: fall open */
    return (unsigned long)fn >= lo && (unsigned long)fn < hi;
}
#endif

/* Per-door snapshot of the seated node and its two pairs, so the frame the
   contents change is named instead of inferred. Four doors is plenty. */
struct DoorNodeSnap { void *door, *node; PortPmf pairs[2]; };
static DoorNodeSnap g_door_snaps[4];

static void port_door_watch(void *door, void *node)
{
    DoorNodeSnap *s = 0;
    for (unsigned i = 0; i < 4; ++i)
        if (g_door_snaps[i].door == door || (!s && !g_door_snaps[i].door))
            { s = &g_door_snaps[i]; if (g_door_snaps[i].door == door) break; }
    if (!s)
        return;
    const PortPmf *p = (const PortPmf *)node;
    if (s->door == door && s->node == node &&
        s->pairs[0].fn == p[0].fn && s->pairs[0].delta == p[0].delta &&
        s->pairs[1].fn == p[1].fn && s->pairs[1].delta == p[1].delta)
        return;
    if (s->door == door)
        std::fprintf(stderr, "[door] node CHANGED under door=%p: node %p->%p "
                     "lo %08x/%d -> %08x/%d  hi %08x/%d -> %08x/%d\n", door,
                     s->node, node, s->pairs[0].fn, s->pairs[0].delta,
                     p[0].fn, p[0].delta, s->pairs[1].fn, s->pairs[1].delta,
                     p[1].fn, p[1].delta);
    s->door = door; s->node = node; s->pairs[0] = p[0]; s->pairs[1] = p[1];
    std::fflush(stderr);
}

static int port_door_call(const PortPmf *p, char *self, void *arg,
                          const char *site)
{
    char *adj = self + (p->delta >> 1);
    PortDoorFn f;
    if (p->delta & 1)
        f = *(PortDoorFn *)(*(char **)adj + p->fn);   /* virtual: fn is an offset */
    else
        f = (PortDoorFn)(size_t)p->fn;
    if (!port_door_fn_in_image((unsigned)(size_t)f)) {
        std::fprintf(stderr, "[door] %s: REFUSED non-code callback: door=%p "
                     "node=%p fn=%08x delta=%d resolved=%p\n", site, self,
                     (const void *)p, p->fn, p->delta, (void *)f);
        std::fflush(stderr);
        return 1;
    }
    return f(adj, arg);
}

/* src/func_ov100_021453d8.cpp: seat the node at +0x140 and run its word-0
   pointer-to-member immediately. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
extern "C" int func_ov100_021453d8(void *selfv, void *node, void *arg)
{
    char *c = (char *)selfv;
    PortPmf *pairs = (PortPmf *)node;
    std::fprintf(stderr, "[door] node install: door=%p node=%p lo=%08x/%d "
                 "hi=%08x/%d\n", selfv, node, pairs[0].fn, pairs[0].delta,
                 pairs[1].fn, pairs[1].delta);
    std::fflush(stderr);
    *(void **)(c + 0x140) = node;
    {
        const PortPmf *p = (const PortPmf *)*(void **)(c + 0x140);
        if (p->fn == 0)
            return 1;
        return port_door_call(p, c, arg, "install");
    }
}

/* src/func_ov100_02145550.cpp: the per-frame half. func_ov100_02145370
   returns the closest player, which is the argument the callback is handed. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
extern "C" int func_ov100_02145550(void *selfv)
{
    char *c = (char *)selfv;
    void *res = func_ov100_02145370(c);
    port_door_watch(selfv, *(void **)(c + 0x140));
    {
        const PortPmf *p = (const PortPmf *)(*(char **)(c + 0x140) + 8);
        if (p->fn != 0)
            port_door_call(p, c, res, "frame");
    }
    return 1;
}

/* The hole that used to be here (Player::CanEnterDoor trapped by name while
 * its src body was NONMATCHING) closed 2026-08-07: the div=1 residue fell to
 * the C++-virtual-dispatch respelling of the +0x48 vcall (PR #1223, notes
 * 6bc), so the matched body now compiles from src/ via slice_gate22.txt like
 * any other function, and the trap that fired the moment real play reached a
 * door is gone. */
