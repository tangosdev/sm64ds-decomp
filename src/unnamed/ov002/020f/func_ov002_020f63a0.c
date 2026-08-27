struct Obj { char pad[0xd8]; void *p; /* 0xd8 */ };

extern void func_ov002_020f5f60(void *c);
extern void _ZN6Memory16operator_delete2EPv(void *p);

int func_ov002_020f63a0(struct Obj *thiz)
{
    func_ov002_020f5f60(thiz->p);
    if (thiz->p != 0) {
        _ZN6Memory16operator_delete2EPv(thiz->p);
        thiz->p = 0;
    }
    return 1;
}
