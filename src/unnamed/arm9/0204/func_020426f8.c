/* func_020426f8 at 0x020426f8
 * Wrapper that swaps arg2->r0 and arg3->r2 before calling 0x0205a61c
 * (an unaligned halfword copy), then returns 0.
 * Register trace: mov r0,r2; mov r2,r3; bl 0x0205a61c(src=arg2,dst=arg1,size=arg3)
 */

extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_020426f8(int arg0, void *dst, const void *src, unsigned int size)
{
    CpuCopy8(src, dst, size);
    return 0;
}
