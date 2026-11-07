/* storage 0x02389F1C runtime 0x03801DB4 size 0xC + 8 pool wram ARM.
 * Tail-call through a pool with the arg preloaded from a global. Two
 * literal-pool refs: one data word, one code (callee) address. */

extern int unk_380973C; /* runtime 0x0380973C */
extern int fn_02380FE0(int *); /* runtime 0x037F8E78 */

int fn_02389F1C(void) {
    return fn_02380FE0(&unk_380973C);
}
