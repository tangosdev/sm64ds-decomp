/* func_ov002_020cabe0 at 0x020cabe0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct State;
extern struct State data_ov002_0211064c;
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, struct State *s);

void func_ov002_020cabe0(char *r0) {
    unsigned short f = *(unsigned short*)(r0 + 0x600 + 0xce) & 4;
    if (f) {
        _ZN6Player11ChangeStateERNS_5StateE(r0, &data_ov002_0211064c);
    }
}
