// NONMATCHING: different op / idiom (div=5). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_02034414(unsigned short n);
extern int LoadFile(int handle);
extern void func_ov075_02116030(void *c, int v);
extern void func_ov075_02115e8c(void *c, int a, int b, int d, int e);
extern unsigned char data_0209d454;
extern int data_ov075_0211d71c;
extern int data_ov075_0211d810[];

void func_ov075_02117d80(char *c)
{
    int fh;
    int v0, v1;
    unsigned char cond;

    data_0209d454 = data_0209d454 & ~1;
    cond = *(unsigned short *)(c + 0xc) == 6;
    if (cond)
    {
        func_02034414(0x19);
    }
    else
    {
        data_0209d454 = data_0209d454 & ~4;
        fh = LoadFile(0x9802);
        func_ov075_02116030(&data_ov075_0211d71c, fh);
        func_02034414(0x16);
        v0 = data_ov075_0211d810[4];
        v1 = data_ov075_0211d810[5];
        data_0209d454 = data_0209d454 | 4;
        *(int *)(c + 0x64) = v0;
        *(int *)(c + 0x68) = v1;
    }
    data_0209d454 = data_0209d454 | 1;
    *(unsigned char *)(c + 0x280) = 2;
    *(unsigned char *)(c + 0x281) = 1;
    func_ov075_02115e8c(c + 0x70, 8, 0, 0x50, 0xa8);
    func_ov075_02115e8c(c + 0x94, 8, 0, 0xb0, 0xa8);
}
