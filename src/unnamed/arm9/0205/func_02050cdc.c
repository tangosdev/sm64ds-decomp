struct Obj;
struct Elem;

extern int *func_02050c14(int a);
extern struct Obj *func_020509b0(unsigned int i);
extern struct Elem *func_020523e0(struct Obj *obj, int idx);

void *func_02050cdc(int a, int idx)
{
    int *p;
    struct Obj *o;
    struct Elem *e;

    p = func_02050c14(a);
    if (p == 0)
        return 0;
    o = func_020509b0((unsigned int)*p);
    if (o == 0)
        return 0;
    e = func_020523e0(o, idx);
    if (e == 0)
        return 0;
    return (char *)e + 4;
}
