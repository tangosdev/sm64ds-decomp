/* Decimal digit-string increment with carry (BCD odometer over u8 digits at
 * obj+5..obj+4+len; carry bumps the s16 at obj+2). Symbol map previously carved
 * the dead trailing bx lr (mwcc always appends after for(;;)/goto whose exits
 * are early returns) into "func_02071694"; real extent is 0x54 (notes 9a(3)).
 * Lever: near-miss tip (int return + goto-loop + 6h u64 launder on halfword RMW). */
#define LNDR(p) ((long long)(int)(p) & 0xffffffffffffffffLL)
typedef unsigned char u8;
typedef short s16;

int func_02071644(u8 *obj, int len)
{
    u8 *first = obj + 5;
    u8 *p = first + len - 1;
loop:
    if (*p < 9) {
        int v = *p + 1;
        *p = v;
        return v;
    }
    if (p == first) {
        s16 *h = (s16 *)(int)LNDR(obj + 2);
        int v;
        *p = 1;
        v = *h + 1;
        *h = v;
        return v;
    }
    *p = 0;
    p--;
    goto loop;
}
