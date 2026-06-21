/* func_ov002_020e2b6c at 0x020e2b6c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct State;
extern struct State data_ov002_021104cc;
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, struct State *s);

int func_ov002_020e2b6c(char *r0) {
    if (*(int*)(r0 + 0x68c) < 0xb000) {
        return 0;
    }
    _ZN6Player11ChangeStateERNS_5StateE(r0, &data_ov002_021104cc);
    return 1;
}
