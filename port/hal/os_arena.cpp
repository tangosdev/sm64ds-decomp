// Host OS arena: the memory region SetupRootHeap carves the root heap from.
//
// On the DS, data_020a0ea4 is the OS globals object and the func_02058*
// family are the arena accessors (get-low, get-high, align-allocate,
// set-low). On host the arena is one big malloc'd block, initialized on
// first use so the smoke needs no setup call.
#include <stdlib.h>

typedef unsigned int u32;

enum { HOST_ARENA = 8 << 20 };   /* 8 MB: larger than the DS main-RAM arena */

static char *g_lo, *g_hi;

/* THE ARENA HAS TO BE DETERMINISTIC. It was a plain malloc, which left two
   things different on every run: the contents (whatever the host allocator
   last had there) and the absolute base. Both reach the game. Anything that
   reads a field before writing it picks up the old contents, and the carve in
   func_02058cd0 rounds `lo` up against the ABSOLUTE address, so a base that
   is not aligned to at least the largest alignment the game asks for shifts
   every later allocation by a different amount.
   The symptom was a walk_window selftest that produced five different final
   frames in six identical runs -- Mario ended up somewhere else each time,
   which makes any before/after comparison of the renderer meaningless.
   calloc zeroes it (the DS arena is cleared main RAM) and the 64K alignment
   makes every carve offset identical from one run to the next. */
enum { ARENA_ALIGN = 0x10000 };

/* The aligned base of the arena, i.e. g_lo as it stood the instant arena_init
   ran, before any carve advanced it. g_lo itself is the live carve cursor and
   moves; g_base does not. The save-state code (hal/lk6_savestate.cpp) needs the
   fixed base and the fixed ceiling to bound the region it snapshots, plus the
   live cursor as a value it captures and restores like any other allocator
   state. */
static char *g_base;

static void arena_init(void)
{
    if (!g_lo) {
        /* soaks and tools can ask for more than the DS ever had */
        const char *env = getenv("SM64DS_HOST_ARENA_MB");
        size_t mb = env ? (size_t)atoi(env) : 0;
        size_t size = mb ? mb << 20 : (size_t)HOST_ARENA;
        char *base = (char *)calloc(size + ARENA_ALIGN, 1);
        if (!base) return;
        g_lo = (char *)(((size_t)base + (ARENA_ALIGN - 1))
                        & ~(size_t)(ARENA_ALIGN - 1));
        g_hi = g_lo + size;
        g_base = g_lo;
    }
}

extern "C" {
/* Read-only arena window accessors for the save-state layer. base and end are
   fixed for the process lifetime; cursor is the live low-water carve pointer
   that func_02058cd0/func_02058d58 move. All three return 0 before the first
   carve forces arena_init, which is why the save-state code calls
   port_arena_base() (it runs arena_init) before reading any of them. */
void *port_arena_base(void)   { arena_init(); return g_base; }
void *port_arena_end(void)    { arena_init(); return g_hi; }
void *port_arena_cursor(void) { arena_init(); return g_lo; }
void  port_arena_set_cursor(void *p) { g_lo = (char *)p; }
}

extern "C" {
// the OS globals object; the accessors ignore it, but the address must exist
char data_020a0ea4[4];

/* PORT_HOST_ABI: DS OS-arena globals live at unmapped 0x27ffda0; host substitutes
   a deterministic calloc'd arena. */
int func_02058ea0(void *) { arena_init(); return (int)(size_t)g_lo; }   /* arena lo */
/* PORT_HOST_ABI: DS OS-arena globals live at unmapped 0x27ffdc4; host arena. */
int func_02058eb4(void *) { arena_init(); return (int)(size_t)g_hi; }   /* arena hi */

/* align `lo` up by `align`, bounded by hi -- mirrors OS_AllocFromArenaLo's
 * pre-alignment step as SetupRootHeap uses it
 * PORT_HOST_ABI: DS OS-arena state (0x27ffda0) unmapped; host arena carve. */
int func_02059040(void *, int lo, int hi, int align)
{
    (void)hi;
    return (lo + align - 1) & ~(align - 1);
}

/* PORT_HOST_ABI: DS OS-arena globals live at unmapped 0x27ffda0; host arena. */
void func_02058d58(void *, int newLo) { g_lo = (char *)(size_t)newLo; }  /* set lo */

/* carve `size` bytes aligned `align` from the low side
   PORT_HOST_ABI: DS OS-arena state (0x27ffda0) unmapped; host arena carve. */
void *func_02058cd0(void *, int size, int align)
{
    arena_init();
    char *p = (char *)(((size_t)g_lo + align - 1) & ~(size_t)(align - 1));
    if (p + size > g_hi)
        return 0;
    g_lo = p + size;
    return p;
}
}
