typedef struct Vec3 { int x, y, z; } Vec3;

extern Vec3 data_ov007_020d7dac;
extern void *func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c2390(char *t);

typedef struct Obj {
    void *f00;      // 0x00
    Vec3 v04;       // 0x04
    Vec3 v10;       // 0x10
    int f1c;        // 0x1c
    short f20;      // 0x20
    short f22;      // 0x22
    int f24;        // 0x24
} Obj;

Obj *func_ov007_020c241c(void *a) {
    Obj *o = (Obj *)func_ov007_020c3df4(0, 0x28);
    o->f00 = a;
    o->v10 = data_ov007_020d7dac;
    {
        Vec3 zero = {0, 0, 0};
        o->v04 = zero;
    }
    o->f22 = 0x7fff;
    o->f24 = 1;
    func_ov007_020c2390((char *)o);
    return o;
}
