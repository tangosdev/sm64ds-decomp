/* func_ov007_020b6cd0 at 0x020b6cd0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern int func_ov007_020ade14(int c);
extern int func_ov007_020adbc8(int c);
extern int func_ov007_020b73b4(int c);

typedef struct {
    char pad74[0x74];
    int f74;
    char pad78[0xa4 - 0x78];
    int fa4[9];
    int fc8[8];
} Foo;

extern Foo* data_ov007_0210342c;

void func_ov007_020b6cd0(void) {
    int i;
    for (i = 0; i < 9; i++) {
        func_ov007_020ade14(data_ov007_0210342c->fa4[i]);
    }
    for (i = 0; i < 8; i++) {
        func_ov007_020adbc8(data_ov007_0210342c->fc8[i]);
    }
    func_ov007_020b73b4(data_ov007_0210342c->f74);
}
