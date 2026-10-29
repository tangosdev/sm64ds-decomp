#include "types.h"
#include "Decimal.h"

/* MSL __num2dec: convert x to a Decimal of at most f->digits significant
 * digits (capped at 32), then turn the digit values into ASCII. decform is
 * MSL's layout, as in func_0206ece0.c (__float2str), which calls this. */
typedef struct {
    char style;
    char unused;
    short digits;
} decform;

void func_02070c68(void* out, double x);
void func_020715e0(void* thiz, int val);

#pragma opt_strength_reduction off
void func_02070b98(void* a0, double x, void* d)
{
    int n;
    Decimal *c = (Decimal *)d;
    int i;
    n = ((decform *)a0)->digits;
    func_02070c68(c, x);
    if (c->sig.text[0] > 9) /* inf or nan: no digits to round */
        return;
    if (n > 0x20) n = 0x20;
    func_020715e0(c, n);
    while (c->sig.length < n) {
        c->sig.text[c->sig.length++] = 0;
    }
    c->exp -= c->sig.length - 1;
    for (i = 0; i < c->sig.length; i++) {
        c->sig.text[i] += '0';
    }
}
