struct S {
    char pad[0x28];
    int field_0x28;
};

int func_ov006_021146ac(struct S *p) {
    return (int)(((long long)p->field_0x28 * p->field_0x28 + 0x800) >> 12);
}
