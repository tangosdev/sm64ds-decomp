extern int func_ov007_020b8ec0(int x);
extern int func_ov007_020c94a0(int a, unsigned int b);
extern void func_ov007_020b8b80(void *a, int b, int c);
extern void func_ov007_020b8d48(void *t, int v);
extern void func_ov007_020b8cfc(void *a, int b, int n);

extern int data_ov007_02103464;
extern unsigned char data_ov007_020d7a34[];

void func_ov007_020b8a78(void *t, int idx)
{
    int r;

    if (idx < 0)
        goto neg;

    r = func_ov007_020b8ec0(0x1f63);
    r = func_ov007_020c94a0(0, (unsigned int)r);
    func_ov007_020b8b80(t, r, 1);

    data_ov007_02103464 = data_ov007_02103464 + 4;
    func_ov007_020b8d48(t, data_ov007_020d7a34[idx]);
    return;
neg:
    func_ov007_020b8cfc(t, 0x47, 5);
}
