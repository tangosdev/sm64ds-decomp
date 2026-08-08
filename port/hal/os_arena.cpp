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
        /* soaks and tools can ask for more than the DS ever had */
        const char *env = getenv("SM64DS_HOST_ARENA_MB");
        size_t mb = env ? (size_t)atoi(env) : 0;
        size_t size = mb ? mb << 20 : (size_t)HOST_ARENA;
        g_lo = (char *)malloc(size);
        g_hi = g_lo + size;
    }
}

extern "C" {
// The arena id Heap::SetupRootHeap selects from. The accessors below ignore it,
// but the storage has to exist to link.
//
// Was spelled `char data_020a0ea4[4]'. Same word at 0x020a0ea4, which
// config/arm9/symbols.txt has always also named ROOT_HEAP_ARENA_ID; src/ now
// uses that name throughout and the address spelling appears nowhere, so this
// reference was left dangling and port_refcheck caught it.
//
// It is a u32 id, not a pointer: Heap::InitializeRootHeap stores 0 to select
// OS_ARENA_MAIN, and it is passed BY VALUE. The accessors took `void *' when
// the caller was a .c file spelling its own mangled name; the migrated caller
// declares them `u32', so they say u32 here too rather than relying on a
// 32-bit value and a 64-bit parameter agreeing on a host they do not.
u32 ROOT_HEAP_ARENA_ID;

int func_02058ea0(u32) { arena_init(); return (int)(size_t)g_lo; }   /* arena lo */
int func_02058eb4(u32) { arena_init(); return (int)(size_t)g_hi; }   /* arena hi */

/* align `lo` up by `align`, bounded by hi -- mirrors OS_AllocFromArenaLo's
 * pre-alignment step as SetupRootHeap uses it */
int func_02059040(u32, int lo, int hi, int align)
{
    (void)hi;
    return (lo + align - 1) & ~(align - 1);
}

void func_02058d58(u32, int newLo) { g_lo = (char *)(size_t)newLo; }  /* set lo */

/* carve `size` bytes aligned `align` from the low side */
void *func_02058cd0(u32, int size, int align)
{
    arena_init();
    char *p = (char *)(((size_t)g_lo + align - 1) & ~(size_t)(align - 1));
    if (p + size > g_hi)
        return 0;
    g_lo = p + size;
    return p;
}
}
