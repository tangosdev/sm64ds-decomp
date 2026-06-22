void func_02071ba0(int obj)
{
    int p;
    void (*fn)(int, int);
    p = *(int*)obj;
    if (p == 0) return;
    fn = *(void(**)(int, int))(obj + 8);
    if (fn == 0) return;
    fn(p, -1);
}
