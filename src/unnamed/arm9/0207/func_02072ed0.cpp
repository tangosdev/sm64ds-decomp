#include "MWCPlusLib.h"

/* EH runtime: binary search of count index entries for the one whose
   [start, start + (sizeFlags & ~1)] range holds key. Returns 0 when none does.
   func_02072dac calls it. */
struct EHIndexEntry *func_02072ed0(struct EHIndexEntry *base, int count, unsigned int key) {
    int lo = 0, hi = count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        struct EHIndexEntry *e = &base[mid];
        unsigned int v = e->start;
        if (key < v) {
            hi = mid - 1;
        } else if (key > v + (e->sizeFlags & ~1u)) {
            lo = mid + 1;
        } else {
            return e;
        }
    }
    return 0;
}
