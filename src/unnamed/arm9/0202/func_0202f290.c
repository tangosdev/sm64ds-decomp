struct Obj { char pad[0x14]; int f; };
extern void func_0202ee94(struct Obj *o);
extern void func_0202efa0(struct Obj *o);

void func_0202f290(struct Obj *o)
{
    if (o->f == 2)
        func_0202ee94(o);
    else
        func_0202efa0(o);
}
