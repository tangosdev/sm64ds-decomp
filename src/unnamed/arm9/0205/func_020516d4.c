/* func_020516d4 at 0x020516d4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct Entry {
    int field0;
    unsigned short ids[4];
};

extern struct Entry *_ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj(unsigned int id);
extern int func_02051514(int a, int b);
extern int *func_02050b4c(unsigned int id);
extern int func_020514b4(int a, int b);
extern void func_0205b78c(int base, int index, int arg);

int func_020516d4(unsigned int id, int flags, int c) {
    struct Entry *entry;
    int base;
    int i;

    base = 0;
    entry = _ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj(id);
    if (entry == 0) return 0;

    if (flags & 2) {
        base = func_02051514(entry->field0, c);
        if (base == 0) return 0;
    }

    if (flags & 4) {
        for (i = 0; i < 4; i++) {
            if (entry->ids[i] != 0xffff) {
                int *p;
                int arg;
                p = func_02050b4c(entry->ids[i]);
                if (p == 0) return 0;
                arg = func_020514b4(*p, c);
                if (arg == 0) return 0;
                if (base != 0) {
                    func_0205b78c(base, i, arg);
                }
            }
        }
    }
    return 1;
}
