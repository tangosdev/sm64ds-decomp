extern int func_02050c14(unsigned int id);
extern int func_020509b0(unsigned int i);
extern void *func_020523e0(void *obj, int idx);
extern int func_02051a98(int a, int b, int c, int d, void *elem, int r4, int arg5, int arg6);

int func_02051e60(int a, int b, int c, int d, unsigned int arg5, int arg6)
{
    int *p;
    void *elem;
    int r4;
    p = (int*)func_02050c14(arg5);
    if (p == 0) return 0;
    r4 = func_020509b0(p[0]);
    if (r4 == 0) return 0;
    elem = func_020523e0((void*)r4, arg6);
    if (elem == 0) return 0;
    if (d < 0) d = *(unsigned char*)((char*)elem + 8);
    if (c < 0) c = *(unsigned short*)((char*)elem + 4);
    if (b < 0) b = *(unsigned char*)((char*)elem + 9);
    return func_02051a98(a, b, c, d, elem, r4, arg5, arg6);
}
