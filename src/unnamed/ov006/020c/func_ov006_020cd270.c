#pragma opt_common_subs off

extern void func_0203cd80(int *v, int a);
extern void func_0203ccd4(int *v, int a);
extern int data_ov006_02140594;
extern char *data_ov006_02140550;
extern void func_ov006_020cafb4(char *p);
extern void func_ov006_020c8a30(void);

void func_ov006_020cd270(void)
{
    int v[3];
    int w[3];

    v[0] = 0;
    v[1] = 0;
    v[2] = 0xfffff008;
    func_0203cd80(v, 0x2000);
    func_0203ccd4(v, 0);
    *(volatile int *)0x040004c8 =
        (((short)v[0] >> 3) & 0x3ff) |
        ((((short)v[1] >> 3) & 0x3ff) << 10) |
        ((((short)v[2] >> 3) & 0x3ff) << 20);

    w[0] = 0;
    w[1] = 0;
    w[2] = 0xfffff008;
    func_0203cd80(w, -0x2000);
    *(volatile int *)0x040004c8 =
        (((short)w[0] >> 3) & 0x3ff) |
        ((((short)w[1] >> 3) & 0x3ff) << 10) |
        ((((short)w[2] >> 3) & 0x3ff) << 20) | 0x40000000;

    {
        int i;
        for (i = 0; i < data_ov006_02140594; i++)
            func_ov006_020cafb4(data_ov006_02140550 + i * 0xd0);
    }

    func_ov006_020c8a30();
}
