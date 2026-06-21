/* func_ov002_020e496c at 0x020e496c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int _ZNK6Player14GetBodyModelIDEjb(void *thisp, unsigned int a, int b);

int func_ov002_020e496c(unsigned char *p) {
    int id = _ZNK6Player14GetBodyModelIDEjb(p, p[0x6db], 1);
    return *(int*)(p + 0xdc + (id << 2));
}
