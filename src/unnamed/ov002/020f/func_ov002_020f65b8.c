struct Obj { char pad[0x7c]; void *seq; /* 0x7c */ };

extern void _ZN15TextureSequence6UpdateER15ModelComponents(void *thiz, void *mc);

void func_ov002_020f65b8(struct Obj *o)
{
    if (o->seq == 0)
        return;
    _ZN15TextureSequence6UpdateER15ModelComponents(o->seq, (char *)o + 8);
}
