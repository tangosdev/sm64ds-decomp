//cpp
struct E {
    virtual void v0();
    virtual void v1();
    char pad[0x24 - 4];
};
struct C {
    E elems[1];
    char pad[0x120 - 0x24];
    int count;
    unsigned char flag;
};
extern "C" int func_ov004_020b8f18(C* c);
int func_ov004_020b8f18(C* c) {
    int i;
    if (c->flag == 0) return 0;
    for (i = 0; i < c->count; i++) {
        c->elems[i].v1();
    }
    return 1;
}
