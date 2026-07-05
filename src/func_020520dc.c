struct Obj1;
extern void func_02050008(struct Obj1 *o);

void func_020520dc(char *self)
{
    if (*(int *)(self + 0x100) == 0)
        return;

    *(int *)(((long long)(int)(self + 0x100)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;

    if (*(int *)(self + 0x100) != 0)
        return;

    func_02050008((struct Obj1 *)self);
}
