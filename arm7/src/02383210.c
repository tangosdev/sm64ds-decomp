/* storage 0x02383210 runtime 0x037FB0A8 size 0x18 wram ARM.
 * Trampoline: zero-extend halfword arg, tail-call fn_0238322C(0xC000, a1).
 * One ABS32 pool word (in-pack callee). */

extern int fn_0238322C(int, int);

int fn_02383210(int a1) {
    return fn_0238322C(0xC000, (unsigned short)a1);
}
