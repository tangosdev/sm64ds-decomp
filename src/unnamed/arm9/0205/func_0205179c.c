#include "types.h"
extern int* _ZN5Sound17InfoSequenceEntry9GetWithIDEj(unsigned int id);
extern int* _ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj(unsigned int id);
extern int func_02051514(int a, int b);
extern void* func_02050b4c(unsigned int id);
extern int func_020514b4(int a, int b);
extern void func_0205b78c(int base, int index, int arg);
extern int func_020515d4(int a, int b);

struct Seq { int f0; u16 f4; };
struct Bank { int f0; u16 f4; };

int func_0205179c(unsigned int id, int flags, int arg2)
{
    struct Seq* seq;
    struct Bank* bank;
    int r = 0;
    int i;

    seq = (struct Seq*)_ZN5Sound17InfoSequenceEntry9GetWithIDEj(id);
    if (!seq) return r;
    bank = (struct Bank*)_ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj(seq->f4);
    if (!bank) return r;

    if (flags & 2) {
        r = func_02051514(bank->f0, arg2);
        if (!r) return 0;
    }
    if (flags & 4) {
        for (i = 0; i < 4; i++) {
            u16 id2 = (&bank->f4)[i];
            if (id2 != 0xffff) {
                void* p = func_02050b4c(id2);
                int q;
                if (!p) return 0;
                q = func_020514b4(*(int*)p, arg2);
                if (!q) return 0;
                if (r != 0) func_0205b78c(r, i, q);
            }
        }
    }
    if (flags & 1) {
        int s = func_020515d4(seq->f0, arg2);
        if (!s) return 0;
    }
    return 1;
}
