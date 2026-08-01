extern short data_ov063_0211e7e0[];
void func_ov063_0211adfc(char *p);
void func_ov063_0211a810(char *p, int a);
void func_ov063_0211a76c(char *p, int a, int b);
void func_ov063_0211a6f0(char *p);

int func_ov063_0211a634(char *thiz, int arg)
{
    unsigned int idx;
    int *pf;
    int rv;
    func_ov063_0211adfc(thiz);
    if (*(unsigned short*)(thiz + 0x100) == 0)
        func_ov063_0211a810(thiz, 0);
    idx = *(unsigned short*)(thiz + 0x100);
    if (idx < 0x20) {
        func_ov063_0211a76c(thiz, 0,
            ((arg * data_ov063_0211e7e0[idx]) << 12) / 5000);
        rv = 0;
        goto done;
    }
    pf = (int*)(((int)thiz + 0x19c));
    *pf &= ~1;
    func_ov063_0211a6f0(thiz);
    *(unsigned char*)(thiz + 0x5cc) = 1;
    return 1;
done:
    return rv;
}
