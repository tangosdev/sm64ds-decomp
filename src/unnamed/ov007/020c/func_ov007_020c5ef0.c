struct Obj { int f0; int f4; int f8; int fc; int f10; int f14; int f18; };
struct Inner { struct Obj** f0; int f4; };

struct O {
    char pad0[8];
    int f8;            /* 0x8 */
    char pad_c[0x30];
    struct Inner** f3c; /* 0x3c */
};

extern void func_ov007_020c6af0(struct O* c);
extern void func_ov007_020c6b58(struct O* c);
extern void func_ov007_020c64c4(struct O* c, int b);
extern void func_ov007_020c62cc(struct O* c, int b);

void func_ov007_020c5ef0(struct O* c)
{
    int i, j;
    func_ov007_020c6af0(c);
    func_ov007_020c6b58(c);
    func_ov007_020c64c4(c, 0);
    func_ov007_020c62cc(c, 1);
    for (i = 0; i < c->f8; i++) {
        struct Inner* in = c->f3c[i];
        for (j = 0; j < in->f4; j++) {
            struct Obj* o = in->f0[j];
            o->f18 = 0;
            o->f14 = o->f18;
            o->f10 = o->f14;
            o->fc = o->f10;
            o->f8 = o->fc;
            o->f4 = o->f8;
        }
    }
    *(int*)((char*)c + 0x90) = 0;
    *(int*)((char*)c + 0x8c) = *(int*)((char*)c + 0x90);
    *(int*)((char*)c + 0x88) = *(int*)((char*)c + 0x8c);
}
