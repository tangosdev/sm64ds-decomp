/* func_02051a98 at 0x02051a98
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct Entry7 {
    unsigned int f0;
    unsigned short ids[6];
};

struct P4 {
    int f0;
    short pad;
    unsigned char f6;
    unsigned char f7;
};

struct P5 {
    char pad[0x18];
    int off18;
};

extern struct Entry7 *_ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj(unsigned int id);
extern void *func_0204f63c(void *p, void *idx, void *arg);
extern void *func_020509b0(unsigned int i);
extern void func_0204f630(void *p);
extern unsigned int *func_02050b4c(unsigned int id);
extern void func_0205b78c(void *base, int index, void *arg);
extern void func_0204f600(void *thiz, void *p, int v, void *node);
extern void func_0204f914(void *p, unsigned char v);
extern void func_0204f89c(void *p, unsigned char v);
extern void func_0204f6f8(void *o, int a, int b);

int func_02051a98(void *p0, void *p1, unsigned int p2, void *p3,
                  struct P4 *p4, struct P5 *p5, int p6, int p7)
{
    struct Entry7 *e7;
    void *r6;
    void *r5;
    int i;

    e7 = _ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj(p2);
    if (e7 == 0)
        return 0;
    r6 = func_0204f63c(p0, p1, p3);
    if (r6 == 0)
        return 0;
    r5 = func_020509b0(e7->f0);
    if (r5 == 0) {
        func_0204f630(r6);
        return 0;
    }
    for (i = 0; i < 4; i++) {
        unsigned int id = e7->ids[i];
        unsigned int *ent;
        void *node;
        if (id != 0xffff) {
            ent = func_02050b4c(id);
            if (ent == 0) {
                func_0204f630(r6);
                return 0;
            }
            node = func_020509b0(*ent);
            if (node == 0) {
                func_0204f630(r6);
                return 0;
            }
            func_0205b78c(r5, i, node);
        }
    }
    func_0204f600(r6, (char *)p5 + p5->off18, p4->f0, r5);
    func_0204f914(p0, p4->f6);
    func_0204f89c(p0, p4->f7);
    func_0204f6f8(p0, p6, p7);
    return 1;
}
