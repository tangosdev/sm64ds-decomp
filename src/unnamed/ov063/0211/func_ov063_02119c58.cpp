//cpp
struct dActor_c;
extern "C" {
dActor_c *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, dActor_c *);
void func_ov063_02119c58(char *c);
}
void func_ov063_02119c58(char *c) {
    unsigned int id;
    dActor_c *a;
    if (*(int *)(c + 0x490) != 0) return;
    id = 0xd2;
    a = 0;
    for (;;) {
        a = _ZN8dActor_c15FindWithActorIDEjPS_(id, a);
        if (a == 0) return;
        unsigned char t = *((unsigned char *)a + 0x5cf);
        if (t == 0xe) break;
        if (t == 0xd) break;
    }
    *(int *)(c + 0x490) = *(int *)((char *)a + 4);
}
