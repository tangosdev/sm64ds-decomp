/* func_ov002_020d6048 at 0x020d6048
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct State;
extern struct State data_ov002_02110064;
extern int _ZN6Player7IsStateERNS_5StateE(void *self, struct State *s);

int func_ov002_020d6048(char *r0) {
    char *r4 = r0;
    if (_ZN6Player7IsStateERNS_5StateE(r0, &data_ov002_02110064)) {
        if (*(unsigned char*)(r4 + 0x6e3) == 3) return 1;
    }
    return 0;
}
