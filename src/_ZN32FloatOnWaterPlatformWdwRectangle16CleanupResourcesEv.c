/* Long-branch veneer: this in r0, data ptr in r1, tail to shared cleanup. */
extern void func_ov002_020b66a8(void);
extern void *data_ov029_02113fd4;

asm void _ZN32FloatOnWaterPlatformWdwRectangle16CleanupResourcesEv(void)
{
    ldr ip, [pc, #4]
    ldr r1, [pc, #4]
    bx ip
    dcd func_ov002_020b66a8
    dcd data_ov029_02113fd4
}
