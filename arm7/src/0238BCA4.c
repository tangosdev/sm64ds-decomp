/* storage 0x0238BCA4 runtime 0x03803B3C size 0x4C (listed 0x40 + pool) wram ARM.
 * IRQ guard: if *0x0380FFFC == fn_0238BB20 (in-pack), lock via fn_02382AC4,
 * *0x0380FFFC = word_38098A8[3], return fn_02382AD8(v1); else return the
 * function address (R0 still holds the pool constant, no reload).
 * Three pools (0x0380FFFC x2 loads shared, in-pack 0x038039B8, 0x038098A8),
 * two BLs (0x037FA95C, 0x037FA970). IDA audit confirms both targets. */

extern int fn_02382AC4(void);
extern void *fn_02382AD8(int a1);
extern int dword_38098A8[];

void *fn_0238BCA4(void) {
    if (*(void **)0x0380FFFC == (void *)0x038039B8) {
        int v1 = fn_02382AC4();
        *(int *)0x0380FFFC = dword_38098A8[3];
        return fn_02382AD8(v1);
    }
    return (void *)0x038039B8;
}
