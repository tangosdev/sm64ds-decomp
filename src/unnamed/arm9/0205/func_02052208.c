/* func_02052208 at 0x02052208
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern void func_0204ff9c(void *o);
extern void func_0205212c(void *o);

void func_02052208(void *obj) {
    int v = *(int *)((char *)obj + 0xf0);
    if ((v << 30) >> 31) func_0204ff9c(obj);
    func_0205212c(obj);
}
