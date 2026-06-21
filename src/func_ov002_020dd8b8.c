/* func_ov002_020dd8b8 at 0x020dd8b8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int _ZN6Player7IsStateERNS_5StateE(void *self, void *state);
extern char data_ov002_021105a4;

int func_ov002_020dd8b8(char *r0) {
    char *r4 = r0;
    if (_ZN6Player7IsStateERNS_5StateE(r4, &data_ov002_021105a4) == 0) {
        return 0;
    }
    return *(unsigned char*)(r4 + 0x6e3) == 0 ? 1 : 0;
}
