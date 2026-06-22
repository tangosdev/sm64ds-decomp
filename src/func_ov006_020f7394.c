extern void func_ov006_020f7064(void *c);
extern void func_ov004_020b66d4(void *o);
extern char *func_020beb68;
extern int func_020bc7d4;

void func_ov006_020f7394(char *c)
{
    char *o;
    int v;
    func_ov006_020f7064(c);
    *(int *)(c + 0x53d4) = 0;
    o = func_020beb68;
    v = 0;
    if (o != 0) v = *(int *)(o + 0xa8);
    if (v >= 5) v = 5;
    func_ov004_020b66d4(o);
    func_020bc7d4 = 1;
    *(unsigned char *)(c + 0x5409) = (unsigned char)v;
}
