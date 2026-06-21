/* func_ov002_020c7194 at 0x020c7194
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int _ZN6Player12FinishedAnimEv(void *self);
extern int func_ov002_020c44c4(void *self);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
extern char data_ov002_02110214;

void func_ov002_020c7194(char *r0) {
    char *r4 = r0;
    if (_ZN6Player12FinishedAnimEv(r4) == 0) {
        return;
    }
    *(unsigned char*)(r4 + 0x6e3) = 1;
    if (func_ov002_020c44c4(r4) != 0) {
        return;
    }
    _ZN6Player11ChangeStateERNS_5StateE(r4, &data_ov002_02110214);
}
