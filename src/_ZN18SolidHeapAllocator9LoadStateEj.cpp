//cpp
// @symbol _ZN18SolidHeapAllocator9LoadStateEj
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::LoadState(u32 id) at 0x0204ea3c -- uses `this`.
 *
 * Pops back to a restore point pushed by SaveState. `id == 0` means "the most recent
 * one" and skips the search entirely -- note the search loop is inside `if (id != 0)`,
 * so a zero id never compares against a record's id even if some record carries 0.
 * Returns 0 when the chain is empty or no record matches, 1 on success.
 *
 * Restoring rewinds begin, end and the chain head together, which reclaims the record
 * itself along with everything allocated after it -- the records live in the heap they
 * describe (see SaveState).
 */
struct State { u32 id; void *head; void *tail; struct State *prev; };

s32 SolidHeapAllocator::LoadState(u32 id)
{
    struct State *st;
    void *base = (char *)this + 0x24;
    st = *(struct State **)((char *)base + 8);
    if (id != 0) {
        while (st != 0) {
            if (st->id == id) break;
            st = st->prev;
        }
    }
    if (st == 0) return 0;
    *(void **)base = st->head;
    *(void **)((char *)base + 4) = st->tail;
    *(void **)((char *)base + 8) = st->prev;
    return 1;
}
