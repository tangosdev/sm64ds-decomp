/* storage 0x0238246C runtime 0x037FA304 size 0x48 + pool 0x50 wram ARM.
 * One-shot init guarded by the halfword flag. Two pool data refs
 * (word_3806FE0, dword_3806FE4), two calls. */

extern unsigned short word_3806FE0;
extern int dword_3806FE4[2];
extern void fn_02381E94(int);
extern int fn_02380884(int);

int fn_0238246C(void) {
    unsigned r = word_3806FE0;
    if (r == 0) {
        word_3806FE0 = 1;
        fn_02381E94(1);
        dword_3806FE4[0] = 0;
        dword_3806FE4[1] = 0;
        return fn_02380884(16);
    }
    return r;
}
