struct S {
    char pad[0x90];
    int field_0x90;
    char pad2[0x9c - 0x90 - 4];
    int field_0x9c;
};

void func_020089d8(struct S *p) {
    int limit = p->field_0x9c + 0x25bc8;
    if (p->field_0x90 < limit) {
        p->field_0x90 = limit;
    }
}
