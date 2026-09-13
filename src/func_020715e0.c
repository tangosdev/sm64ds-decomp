#include "types.h"
#include "Decimal.h"

// func_02071698 is still spelled the way its own definition is, over a raw byte
// pointer and its own offsets; recovering that interface is a separate slice.
extern int func_02071698(void *value, int length);
extern void func_02071644(Decimal *value, int length);

void func_020715e0(Decimal *value, int length)
{
    int r;
    if (length <= 0) return;
    if (length >= value->sig.length) return;
    r = func_02071698(value, length);
    value->sig.length = length;
    if (r < 0) return;
    func_02071644(value, length);
}
