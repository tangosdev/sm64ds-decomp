struct Obj0 { char pad[0xc]; int flag : 1; };

void func_0204fde8(struct Obj0 *o);

void func_0204ff9c(struct Obj0 *o) {
    if (o->flag) {
        func_0204fde8(o);
    }
}
