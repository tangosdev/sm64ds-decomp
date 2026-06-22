extern int func_0205d3d4(void *obj, void *buf, int size);
extern int func_0205d368(void *obj, int len, int sel);

struct Obj {
    char pad[0x20];
    int f20;    /* 0x20 */
    char pad24[0x28 - 0x24];
    int f28;    /* 0x28 */
};

struct G {
    char pad[0x2c];
    int f2c;    /* 0x2c */
    char pad30[0x3c - 0x30];
    int f3c;    /* 0x3c */
};

int func_02067e70(struct Obj *obj)
{
    char buf[0x60];
    struct G *g = 0;
    int len;
    if (obj != 0) {
        len = obj->f28 - obj->f20;
        if ((unsigned int)func_0205d3d4(obj, buf, 0x60) >= 0x60)
            g = (struct G*)buf;
        func_0205d368(obj, len, 0);
    } else {
        g = (struct G*)0x27ffe00;
    }
    if (g != 0)
        return g->f2c + 0x1e8 + g->f3c;
    return 0;
}
