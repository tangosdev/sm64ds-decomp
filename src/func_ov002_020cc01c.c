/* func_ov002_020cc01c at 0x020cc01c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern char data_ov002_021101b4[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);

int func_ov002_020cc01c(char *self) {
    if (*(unsigned short *)(self + 0x6b8) == 0) return 0;
    _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_021101b4);
    return 1;
}
