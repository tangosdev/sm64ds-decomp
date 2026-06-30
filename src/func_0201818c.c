/* func_0201818c at 0x0201818c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct Obj { char pad[0x20]; unsigned int cur; unsigned int end; char pad2[0x1c]; };

extern int func_020186c0(unsigned int val);
extern int func_02018568(int a);
extern unsigned func_02018ac4(unsigned *p);
extern int _ZN6Memory8AllocateEj(unsigned int);
extern void DecompressLZ16(void *src, void *dst);
extern void func_020185c0(void *buf, int x);
extern void func_0205d4cc(void *a);
extern int _ZN6Memory8AllocateEji(unsigned int, int);
extern void func_0205a61c(int a, int b, unsigned int c);
extern void func_02018770(void);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);
extern unsigned int func_02018a24(unsigned int i);
extern int func_0201834c(void *buf, int x);

int func_0201818c(unsigned int arg0, int arg1)
{
    struct Obj obj;
    char buf[0x44];
    int p = func_020186c0(arg0);
    if (p != 0) {
        unsigned int size;
        int mem;
        if (func_02018568(p)) {
            unsigned int *q = (unsigned int *)(p + 4);
            size = func_02018ac4(q);
            mem = _ZN6Memory8AllocateEj(size);
            DecompressLZ16(q, (void *)mem);
        } else {
            func_020185c0(&obj, arg0);
            size = obj.end - obj.cur;
            func_0205d4cc(&obj);
            mem = _ZN6Memory8AllocateEji(size, 0x20);
            func_0205a61c(p, mem, size);
        }
        func_02018770();
        if (arg1 != 0)
            _ZN4CP1514FlushDataCacheEjj(mem, size);
        return mem;
    }
    func_020185c0(buf, func_02018a24(arg0));
    return func_0201834c(buf, arg1);
}