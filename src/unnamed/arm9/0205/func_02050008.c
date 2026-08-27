struct Obj1 { char pad[0x2c]; void *p; void *q; };

void func_0204f15c(void *p);

void func_02050008(struct Obj1 *o) {
    if (o->p == 0) {
        return;
    }
    func_0204f15c(o->p);
    o->p = 0;
    o->q = 0;
}
