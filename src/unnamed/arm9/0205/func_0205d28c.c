/* func_0205d28c at 0x0205d28c
 * Same pattern as func_020426f8: reorders args then calls 0x0205a61c,
 * returns 0. Identical structure but different bl encoding (different distance).
 */

extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_0205d28c(int arg0, void *dst, const void *src, unsigned int size)
{
    CpuCopy8(src, dst, size);
    return 0;
}
