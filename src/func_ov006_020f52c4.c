extern void func_ov006_020f4f94(void *c);
extern void func_ov004_020b66d4(void *o);
extern char *func_020beb68;
extern int func_020bc7d4;

void func_ov006_020f52c4(char *c)
{
    char *o;
    int v;
    func_ov006_020f4f94(c);
    *(int *)(c + 0x5314) = 0;
    o = func_020beb68;
    v = 0;
    if (o != 0) v = *(int *)(o + 0xa8);
    if (v >= 3) v = 3;
    func_ov004_020b66d4(o);
    func_020bc7d4 = 1;
    *(unsigned char *)(c + 0x533b) = (unsigned char)v;
}
