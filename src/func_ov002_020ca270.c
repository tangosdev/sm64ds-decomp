/* func_ov002_020ca270 at 0x020ca270
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Obj {
    char pad[0x6e3];
    unsigned char f6e3;
};

extern int _ZN6Player7IsStateERNS_5StateE(struct Obj *self, void *state);
extern void *data_ov002_02110124;

int func_ov002_020ca270(struct Obj *self) {
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110124)) {
        if (self->f6e3 == 1) return 1;
    }
    return 0;
}
