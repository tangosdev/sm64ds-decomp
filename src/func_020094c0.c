extern void func_02009e70(void *obj);

void func_020094c0(void *self)
{
    *(unsigned int *)((char *)self + 0x118) = *(unsigned int *)((char *)self + 0x110);
    *(unsigned int *)((int *)(((int)self + 0x154))) |= 4;
    func_02009e70(self);
}
