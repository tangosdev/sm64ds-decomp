/* storage 0x0239CC50 runtime 0x027CEBC8 size 0x5C (0x58 + pool) main ARM.
 * Flag set, table-limit checks around a u16 counter bump. The table pointer
 * is re-read through an address-taken local: each *pp deref is its own load,
 * which the u16 stores keep ordered (no hoisting). */

extern unsigned int dword_380FFF4;

struct In {
    char pad[16];
    unsigned short ctr;
};

struct Out {
    char pad[2];
    unsigned short flag;
};

int fn_0239CC50(struct In *a1, struct Out *a2) {
    unsigned int *pp;
    a2->flag = 1;
    pp = &dword_380FFF4;
    if (*(unsigned short *)(*pp + 784) > 0x20)
        return 1;
    ++a1->ctr;
    if ((unsigned int)a1->ctr > *(unsigned short *)(*pp + 740))
        return 5;
    *(unsigned short *)(*pp + 742) = a1->ctr;
    return 0;
}
