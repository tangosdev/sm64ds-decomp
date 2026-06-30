/* func_0201834c at 0x0201834c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct Obj { char pad[0x20]; unsigned int cur; unsigned int end; };

extern void *_ZN6Memory8AllocateEji(unsigned int size, int align);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern int func_02018d48(struct Obj *a, void *b, unsigned int c);
extern int func_02018568(void *a);
extern unsigned int func_02018ac4(void *p);
extern void *_ZN6Memory8AllocateEj(unsigned int size);
extern void DecompressLZ16(void *src, void *dst);
extern void func_0205a61c(void *a, void *b, unsigned int c);
extern void _ZN6Memory10DeallocateEPv(void *p);
extern void Crash(void);
extern void func_0205d4cc(struct Obj *a);
extern void func_02018770(void);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);

unsigned int func_0201834c(struct Obj *self, int flag)
{
    unsigned int result = 0;
    unsigned int len = self->end - self->cur;
    if (len > 8) {
        void *buf = _ZN6Memory8AllocateEji(len, -4);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)buf, len);
        int r = func_02018d48(self, buf, len);
        if (r != len) return result;
        if (func_02018568(buf) != 0) {
            len = func_02018ac4((void *)((char *)buf + 4));
            result = (unsigned int)_ZN6Memory8AllocateEj(len);
            DecompressLZ16((void *)((char *)buf + 4), (void *)result);
        } else {
            result = (unsigned int)_ZN6Memory8AllocateEj(len);
            func_0205a61c(buf, (void *)result, len);
        }
        _ZN6Memory10DeallocateEPv(buf);
    } else {
        Crash();
    }
    func_0205d4cc(self);
    func_02018770();
    if (flag != 0) _ZN4CP1514FlushDataCacheEjj(result, len);
    return result;
}