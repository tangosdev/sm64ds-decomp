/* func_02067b68 at 0x02067b68
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void func_0205a588(void *dst, int val, unsigned int size);
extern void func_0205a61c(void *a, void *b, unsigned int size);
struct S { unsigned int f0; unsigned char *f4; unsigned char *f8; unsigned int fc; };
void func_02067b68(struct S *s, unsigned int start, unsigned int end, int flag) {
    unsigned int f0 = s->f0;
    unsigned int fc = s->fc;
    unsigned int top;
    if (start < 0x4000) start = 0x4000;
    if (end > 0x8000) end = 0x8000;
    if (start < f0) start = f0;
    top = f0 + fc;
    if (end > top) end = top;
    if (start >= end) return;
    if (flag) func_0205a588(s->f8 + start, 0, end - start);
    else func_0205a61c(s->f4 + start, s->f8 + start, end - start);
}