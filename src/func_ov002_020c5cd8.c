/* func_ov002_020c5cd8 at 0x020c5cd8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int func_ov002_020c5dec(void *this, int x);

int func_ov002_020c5cd8(void *this, int val) {
    if (func_ov002_020c5dec(this, 6)) {
        *(int *)((char *)this + 0x364) = val;
        return 1;
    }
    return 0;
}
