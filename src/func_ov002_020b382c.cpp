//cpp
struct Arg { int pad[2]; int f8; };

struct Vt;

struct Obj {
    Vt *vt;
};

struct Vt {
    void (*slots[33])(Obj *);
};

extern "C" void func_ov002_020b382c(Obj *self, Arg *a)
{
    if (a->f8 == 3) return;
    unsigned short c = *(unsigned short *)((char *)self + 0xc);
    int b = (c == 0x11) ? 1 : 0;
    if (b != 0) {
        if (a->f8 != 2) return;
        self->vt->slots[0x7c / 4](self);
    } else {
        self->vt->slots[0x7c / 4](self);
    }
}
