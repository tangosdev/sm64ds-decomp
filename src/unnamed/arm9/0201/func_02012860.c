extern void func_0204f82c(void* a, int b, int c);
extern void func_0204f7fc(void* a, int b, int c);
extern void func_0204f7cc(void* a, int b, int c);

void func_02012860(void* a, int flags, int c, int d, int e)
{
    if (flags & 1) func_0204f82c(a, 0xffff, c);
    if (flags & 2) func_0204f7fc(a, 0xffff, d);
    if (flags & 4) func_0204f7cc(a, 0xffff, e - 0x40);
}
