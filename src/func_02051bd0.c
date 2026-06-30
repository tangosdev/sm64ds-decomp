/* func_02051bd0 at 0x02051bd0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef unsigned short u16;
typedef unsigned char u8;

extern int *_ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj(unsigned int id);
extern int func_0204f63c(void **p, int idx, int r7);
extern int func_020509b0(unsigned int i);
extern void *func_02051a28(int index, void *unk);
extern void func_0204f630(int r7);
extern void *func_02051634(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void *owner);
extern void *func_02050b4c(unsigned int id);
extern void func_0205b78c(int base, int index, int arg);
extern int func_0204f600(int thiz, int p1, int p2, int p3);
extern void func_0204f914(void **p, unsigned char v);
extern void func_0204f89c(void **c, unsigned char v);
extern void func_0204f720(void *pp, int v);
extern void func_02051a68();
extern void func_02051a48();
extern void func_02051a88();

struct Bank { int f0; u16 ids[4]; };
struct A4 { int f0; u16 pad4; u8 b6; u8 b7; };

int func_02051bd0(void **p, int a1, unsigned int a2, int a3, struct A4 *a4, int a5)
{
    struct Bank *bank;
    void *node;
    int r7;
    void *r6 = 0;
    int r5;
    int i;
    void *np;

    bank = (struct Bank *)_ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj(a2);
    if (bank == 0) return 0;

    r7 = func_0204f63c(p, a1, a3);
    if (r7 == 0) return 0;

    r5 = func_020509b0(bank->f0);
    if (r5 == 0) {
        r6 = func_02051a28(a1, (void *)r7);
        if (r6 == 0) { func_0204f630(r7); return 0; }
        r5 = (int)func_02051634(bank->f0, (unsigned int)&func_02051a68, 0, 0, r6);
        if (r5 == 0) { func_0204f630(r7); return 0; }
    }

    for (i = 0; i < 4; i++) {
        u16 v = bank->ids[i];
        if (v != 0xffff) {
            int q;
            node = func_02050b4c(v);
            if (node == 0) { func_0204f630(r7); return 0; }
            q = func_020509b0(*(unsigned int *)node);
            if (q == 0) {
                if (r6 == 0) {
                    r6 = func_02051a28(a1, (void *)r7);
                    if (r6 == 0) { func_0204f630(r7); return 0; }
                }
                q = (int)func_02051634(*(unsigned int *)node, (unsigned int)&func_02051a48, 0, 0, r6);
                if (q == 0) { func_0204f630(r7); return 0; }
            }
            func_0205b78c(r5, i, q);
        }
    }

    np = (void *)func_020509b0((unsigned int)a4->f0);
    if (np == 0) {
        if (r6 == 0) {
            r6 = func_02051a28(a1, (void *)r7);
            if (r6 == 0) { func_0204f630(r7); return 0; }
        }
        np = func_02051634((unsigned int)a4->f0, (unsigned int)&func_02051a88, 0, 0, r6);
        if (np == 0) { func_0204f630(r7); return 0; }
    }

    func_0204f600(r7, (int)((char *)np + *(int *)((char *)np + 0x18)), 0, r5);
    func_0204f914(p, a4->b6);
    func_0204f89c(p, a4->b7);
    func_0204f720(p, a5);
    return 1;
}
