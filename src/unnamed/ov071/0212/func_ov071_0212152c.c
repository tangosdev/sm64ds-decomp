extern void _Z14ApproachLinearRsss(short *, short, short);
extern void func_ov071_02120a48(void *);
extern void func_ov071_02120b14(void *);
int func_ov071_0212152c(void *c)
{
    _Z14ApproachLinearRsss((short *)((char *)c + 0x8c), 0, 0x320);
    *(short *)((char *)c + 0x8e) += *(int *)((char *)c + 0x98);
    func_ov071_02120a48(c);
    func_ov071_02120b14(c);
    return 1;
}
