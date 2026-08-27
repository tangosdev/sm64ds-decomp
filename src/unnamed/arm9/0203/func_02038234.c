typedef void (*PFN)(int self, int arg, int b);
extern int func_02038298(int a, int b);
extern int func_02037f54(int a);
extern PFN func_020393bc(int x);
extern int func_020393b4(int x);
extern int data_020a0c80[];

void func_02038234(int a, int b)
{
    int idx;
    int entry;
    PFN fn;
    if (func_02038298(a, b) == 0) return;
    idx = func_02037f54(a);
    entry = data_020a0c80[idx];
    fn = func_020393bc(entry);
    if (fn == 0) return;
    fn(entry, func_020393b4(entry), b);
}
