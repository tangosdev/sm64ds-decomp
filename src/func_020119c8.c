typedef struct Entry {
    int field_0;
    unsigned char pad4[2];
    unsigned char field_6;
    unsigned char pad7[0xd];
} Entry;

typedef struct Base {
    unsigned char pad4[4];
    unsigned char field_4;
    unsigned char pad5[3];
    Entry entries[0x40];
} Base;

extern int func_02011654(Base *table, Entry *n);

void func_020119c8(Base *base)
{
    Entry *e = base->entries;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (e->field_0) {
            if (e->field_6 == 1) {
                e->field_6 = 0;
            } else {
                func_02011654(base, e);
                base->field_4 = (unsigned char)i;
            }
        }
        e++;
    }
}
