extern int func_0205c5e4(int this, int a);
extern void func_0205c788(int this, int a);
extern int func_0205d044(int a);
extern void func_0205cfa4(int a);

int func_0205cf5c(int this)
{
    int r;
    r = func_0205c5e4(this, *(int*)(this + 0x10));
    func_0205c788(this, r);
    r = func_0205d044(*(int*)(this + 8));
    if (r != 0) func_0205cfa4(r);
    return *(int*)(this + 0x14) == 0;
}
