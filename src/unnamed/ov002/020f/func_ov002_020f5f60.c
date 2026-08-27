extern void _ZN6Memory16operator_delete2EPv(void *p);

void func_ov002_020f5f60(char *c)
{
    void *p = *(void**)(c + 0x510);
    if (p == 0)
        return;
    _ZN6Memory16operator_delete2EPv(p);
    *(void**)(c + 0x510) = 0;
}
