extern void func_ov015_02111fb8(void *self, int idx);
void func_ov015_02111e80(int *c)
{
    /* The two spellings of +0x334 are load-bearing under 2004/b56 and must not be
       unified: the ROM keeps the decrement's address in a register (`add r2,r0,#0x334`)
       and then RE-READS the field as `ldr r1,[r0,#0x334]`. Written with one expression
       shape, b56 recognises the second read as the first lvalue and reuses r2. Every
       way of writing the decrement produces the ROM's form; only the re-read decides. */
    *(int *)((int)c + 0x334) -= 1;
    if (*(int *)((char *)c + 0x334) > 0)
        return;
    if (*(int *)((int)c + 0x32c) == 0)
        func_ov015_02111fb8(c, 3);
    else
        func_ov015_02111fb8(c, 4);
}
