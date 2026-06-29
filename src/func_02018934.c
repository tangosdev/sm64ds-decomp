/* func_02018934 at 0x02018934
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef unsigned int u32;

extern void func_0205d874(int *s);
extern int func_02018d98(int *s, int a);
extern void *_ZN4Heap9_AllocateEji(void *heap, u32 size, int align);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);
extern int func_02018d48(int *s, void *buf, int n);
extern int func_0204ee40(void *a, void *b, void *c);
extern void _ZN4Heap11_DeallocateEPv(void *heap, void *ptr);
extern void func_0205d4cc(int *s);

void *func_02018934(void *arg0, int arg1, void *heap)
{
    int buf[0x11];
    void *result = 0;
    void *ptr;
    void *p60;
    int diff, aligned;

    func_0205d874(buf);
    func_02018d98(buf, arg1);
    diff = buf[9] - buf[8];
    aligned = (diff + 0xf) & ~0xf;
    ptr = _ZN4Heap9_AllocateEji(heap, aligned + 0x60, -0x10);
    p60 = (char *)ptr + 0x60;
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)p60, aligned);
    if (func_02018d48(buf, p60, diff) == diff &&
        func_0204ee40(ptr, arg0, p60)) {
        result = ptr;
    } else {
        _ZN4Heap11_DeallocateEPv(heap, ptr);
    }
    func_0205d4cc(buf);
    return result;
}