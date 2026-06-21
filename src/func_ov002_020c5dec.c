/* func_ov002_020c5dec at 0x020c5dec
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern char data_ov002_02110124[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);

int func_ov002_020c5dec(char *self, int mode) {
    if (mode != 1) {
        if (*(unsigned char *)(self + 0x709) != 0) return 0;
    }
    *(unsigned char *)(self + 0x6e3) = (unsigned char)mode;
    _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_02110124);
    return 1;
}
