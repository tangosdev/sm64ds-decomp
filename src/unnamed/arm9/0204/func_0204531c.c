extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void* self);
extern void func_02047910(int p0, int a, int p2, int c);

void func_0204531c(char* self, int arg)
{
    unsigned int i;
    int a;
    int off;
    int save = *(int*)(self + 0xc);
    a = *(int*)(self + 0x10);

    *(int*)(self + 0xc) = a;
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(self);
    *(int*)(self + 0xc) = save;

    i = 0;
    if (i >= *(unsigned int*)(*(int*)self + 4))
        return;

    off = i;
    do {
        func_02047910(*(int*)(self + 0xc) + off, a, *(int*)(self + 0xc) + off, arg);
        i++;
        off += 0x30;
        a += 0x30;
    } while (i < *(unsigned int*)(*(int*)self + 4));
}
