typedef struct Entry {
    int field_0;
    unsigned char pad4[6];
    unsigned char field_a;
    unsigned char pad_b[0x11];
} Entry;

typedef struct Base {
    unsigned char pad0[4];
    unsigned char field_4;
    unsigned char pad5[3];
    Entry entries[0x40];
} Base;

extern int func_02021a04(Base *table, Entry *n);

void func_02021b98(Base *base)
{
    Entry *e = base->entries;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (e->field_0) {
            e->field_a = 0;
            func_02021a04(base, e);
            base->field_4 = (unsigned char)i;
        }
        e++;
    }
}
