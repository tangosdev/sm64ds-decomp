/* storage 0x0238BCF0 runtime 0x03803B88 size 0x6C (listed 0x5C + pool) wram ARM.
 * IRQ guard (inverted vs fn_0238BCA4): v0 = *0x0380FFFC once; the compare
 * reuses v0 (a second *P load emits an extra LDR). If != fn_0238BB20
 * (in-pack), init word_38098A8[0..3] = {0, 0x0380FE80, 64, v0}, lock via
 * fn_02382AC4, *0x0380FFFC = fn_0238BB20, return fn_02382AD8(v3).
 * Four pools (0x0380FFFC, in-pack 0x038039B8, 0x038098A8, 0x0380FE80
 * numeric), two BLs (0x037FA95C, 0x037FA970). IDA audit confirms. */

extern int fn_02382AC4(void);
extern void *fn_02382AD8(int a1);
extern int dword_38098A8[];

void *fn_0238BCF0(void) {
    int v0 = *(int *)0x0380FFFC;
    if (v0 != 0x038039B8) {
        int *v2 = dword_38098A8;
        *v2 = 0;
        v2[1] = 0x0380FE80;
        v2[2] = 64;
        v2[3] = v0;
        int v3 = fn_02382AC4();
        *(int *)0x0380FFFC = 0x038039B8;
        return fn_02382AD8(v3);
    }
    return (void *)0x038039B8;
}
