extern void func_ov002_020f6514(void *p, void *tbl, unsigned char v);
extern int data_ov002_0210bc88[];

int func_ov002_020f6c34(char *c, unsigned char *src)
{
    unsigned char v = *src;
    func_ov002_020f6514(*(void**)(c + 0xe0), data_ov002_0210bc88, v);
    return 1;
}
