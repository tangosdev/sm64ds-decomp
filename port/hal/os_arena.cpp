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
static void arena_init(void)
{
    if (!g_lo) {
        g_lo = (char *)malloc(HOST_ARENA);
        g_hi = g_lo + HOST_ARENA;
    }
}

extern "C" {
// the OS globals object; the accessors ignore it, but the address must exist
char data_020a0ea4[4];

int func_02058ea0(void *) { arena_init(); return (int)(size_t)g_lo; }   /* arena lo */
int func_02058eb4(void *) { arena_init(); return (int)(size_t)g_hi; }   /* arena hi */

/* align `lo` up by `align`, bounded by hi -- mirrors OS_AllocFromArenaLo's
 * pre-alignment step as SetupRootHeap uses it */
int func_02059040(void *, int lo, int hi, int align)
{
    (void)hi;
    return (lo + align - 1) & ~(align - 1);
}

void func_02058d58(void *, int newLo) { g_lo = (char *)(size_t)newLo; }  /* set lo */

/* carve `size` bytes aligned `align` from the low side */
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
