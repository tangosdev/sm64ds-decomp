// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x0238FBE4 size 0x18 (runtime 0x027C1B5C, main)
/* Hand-asm: same BX-through-pool thunk shape as 0238FBCC.
   Pools +0x10/+0x14: literal 0x281, fn_0238FA3C. */
extern int fn_0238FA3C(short *, int, int, int, int, int);

asm int fn_0238FBE4(short *a1, int a2, int a3, int a4, int a5, int a6)
{
    ldr r1, =0x281
    mov r2, #4
    ldr ip, =fn_0238FA3C
    bx ip
}
