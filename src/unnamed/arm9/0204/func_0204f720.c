struct S {
    char pad34[0x34];
    short a;
    short pad36;
    short b;
};
void func_0204f720(struct S **pp, short v) {
    struct S *p = *pp;
    if (p != 0) {
        p->a = 1;
        p = *pp;
        p->b = v;
    }
}
