typedef struct UnkB {
    int f00, f04, f08, f0c, f10, f14, f18, f1c, f20;
} UnkB;

typedef struct UnkC {
    int f00, f04, f08;
} UnkC;

typedef struct UnkA {
    UnkB *b;
    UnkC *c;
} UnkA;

extern void func_ov007_020c4388(UnkC *c, int arg);
extern void func_ov007_020c421c(UnkC *c);

void func_ov007_020b4464(UnkA *a, int arg) {
    int changed;
    if (a->b->f20 == 0) {
        return;
    }
    changed = 0;
    if (a->c->f04 != a->b->f08 || a->c->f08 != a->b->f0c) {
        changed = 1;
    }
    a->c->f04 = a->b->f08;
    a->c->f08 = a->b->f0c;
    if (changed) {
        func_ov007_020c4388(a->c, arg);
    }
    func_ov007_020c421c(a->c);
}
