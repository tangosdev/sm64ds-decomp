/* func_ov002_020e0a64 at 0x020e0a64
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Obj {
    char pad[1];
};

extern int _ZN6Player6IsAnimEj(struct Obj *self, unsigned int anim);
extern int data_ov002_0210a5b4[];

int func_ov002_020e0a64(struct Obj *self) {
    if (!_ZN6Player6IsAnimEj(self, 0x54)) {
        *(unsigned short *)((char*)self + 0x600 + 0xa8) = 8;
    }
    if (*(unsigned char *)((char*)self + 0x703) != 0) {
        return 0x9b;
    }
    return data_ov002_0210a5b4[*(unsigned char *)((char*)self + 0x6e1)];
}
