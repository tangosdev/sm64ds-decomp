//cpp
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void m3(void *p);
    short f4;
    short f6;
    void **table;
};

extern "C" void func_02044334(Obj *self)
{
    short *ctr;

    if (self->table == 0)
        return;
    if (self->f4 == 0)
        return;
    ctr = (short *)(((int)self + 6));
    *ctr = *ctr - 1;
    if (self->f6 < 0)
        return;
    do {
        self->m3(self->table[self->f6]);
        *ctr = *ctr - 1;
    } while (self->f6 >= 0);
}