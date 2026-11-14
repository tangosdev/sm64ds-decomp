/* storage 0x02380B74 runtime 0x037F8A0C size 0x14 (+0xC pool) wram ARM.
 * Absolute tail call: fn_02380CA4(a1, &dword_27FFFE8, fn_02380B30, 1). */

extern int fn_02380CA4(int, int, int, int);
extern void fn_02380B30(void);
extern int dword_27FFFE8;

int fn_02380B74(int a1) {
    return ((int (*)(int, int, int, int))fn_02380CA4)(
        a1, (int)&dword_27FFFE8, (int)fn_02380B30, 1);
}
