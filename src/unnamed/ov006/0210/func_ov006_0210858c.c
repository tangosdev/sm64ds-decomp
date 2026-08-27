struct Obj { char pad[8]; void *a; /* 0x8 */ void *b; /* 0xc */ };

extern void Ov004_Deallocate(void *p);

int func_ov006_0210858c(struct Obj *o)
{
    if (o->a)
        Ov004_Deallocate(o->a);
    if (o->b)
        Ov004_Deallocate(o->b);
    return 1;
}
