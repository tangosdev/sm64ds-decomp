/* func_0205d2b0 at 0x0205d2b0
 * Similar to func_0205d28c but also reads a field at offset 0x1c from arg0
 * and uses it as a base: dst = arg0->unk1c + arg2.
 * Calls 0x0205a61c(src=arg1, dst=arg0->unk1c+arg2, size=arg3), returns 0.
 */

struct Unk0205d2b0 {
    char pad[0x1c];
    void *unk1c;  /* 0x1c */
};

extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_0205d2b0(struct Unk0205d2b0 *thiz, const void *src, unsigned int offset, unsigned int size)
{
    CpuCopy8(src, (char *)thiz->unk1c + offset, size);
    return 0;
}
