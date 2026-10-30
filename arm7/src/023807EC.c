/* storage 0x023807EC runtime 0x037F8684 size 0x80 + 20 pool wram ARM.
 * Four pool data refs (the stride-12 tables) plus one CONSTANT pool
 * (0x0380FFF8, byte-exact, no reloc): spelling it as a constant puts the
 * address in R1 and the value in R0, matching the ROM's LDR/ORR/STR order;
 * an extern puts them the other way round. One BL (fn_02380884, in-pack)
 * and one indirect call (dequeue-and-call through the cleared slot). The
 * three stride-12 tables are one int[3] per entry; only field 0 is touched. */

extern unsigned short tab_3805DE0[];
extern int tab_3806DE4[][3];
extern int tab_3806DEC[][3];
extern int tab_3806DE8[][3];
extern int fn_02380884(int a1);

int fn_023807EC(int a1) {
    int v1 = 1 << tab_3805DE0[a1];
    int v2 = tab_3806DE4[a1][0];
    int result;
    tab_3806DE4[a1][0] = 0;
    if (v2 != 0)
        ((void (*)(int))v2)(tab_3806DEC[a1][0]);
    *(volatile int *)0x380FFF8 |= v1;
    result = tab_3806DE8[a1][0];
    if (result == 0)
        result = fn_02380884(v1);
    return result;
}
