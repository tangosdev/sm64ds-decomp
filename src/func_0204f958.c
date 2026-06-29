/* func_0204f958 at 0x0204f958
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef unsigned char u8;
extern u8 *_ZN18NestedHeapIterator4NextEP13HeapAllocator(void *iter, void *alloc);
extern void func_0204f558(u8 *thiz, int val);
extern char data_020a4d6c[];
void func_0204f958(int idx, int val) {
    u8 *cur;
    u8 *next;
    char *iter = &data_020a4d6c[idx * 0x1c];
    cur = _ZN18NestedHeapIterator4NextEP13HeapAllocator(iter, 0);
    if (cur == 0) return;
    do {
        next = _ZN18NestedHeapIterator4NextEP13HeapAllocator(iter, cur);
        func_0204f558(cur, val);
        cur = next;
    } while (cur != 0);
}
