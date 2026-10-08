struct S {
    char pad_0[0x16c];
    int field_0x16c;
    char pad_170[0x198 - 0x170];
    short field_0x198;
};

int func_020094a8(struct S *p) {
    p->field_0x198 = 0;
    p->field_0x16c = 0;
    return 1;
}
