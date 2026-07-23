extern void func_020731dc(void* a, void* b, void** node);
extern void func_020072c0(void);

extern int data_ov002_0210ddd0[3];
extern int data_ov002_0210dddc[3];
extern int data_ov002_0210dde8[3];
extern int data_ov002_0210ddf4[3];
extern void* data_ov002_0210ddac;
extern void* data_ov002_0210ddb8;
extern void* data_ov002_0210ddc4;
extern void* data_ov002_0210dda0;

void __sinit_ov002_02100f84(void) {
    int k;

    data_ov002_0210ddd0[0] = 0;
    data_ov002_0210ddd0[1] = 0x190000;
    data_ov002_0210ddd0[2] = 0x12c000;
    func_020731dc(data_ov002_0210ddd0, (void*)func_020072c0, &data_ov002_0210ddac);

    {
        int *p = (int *)(((int)data_ov002_0210dddc) & 0xFFFFFFFFFFFFFFFFll);
        k = 0x190000;
        p[0] = 0;
        p[1] = -k;
        p[2] = 0x12c000;
        func_020731dc(p, (void*)func_020072c0, &data_ov002_0210ddb8);
    }
    {
        int *p = (int *)(((int)data_ov002_0210dde8) & 0xFFFFFFFFFFFFFFFFll);
        p[0] = 0x190000;
        p[1] = 0;
        p[2] = 0x12c000;
        func_020731dc(p, (void*)func_020072c0, &data_ov002_0210ddc4);
    }
    {
        int *p = (int *)(((int)data_ov002_0210ddf4) & 0xFFFFFFFFFFFFFFFFll);
        k = 0x190000;
        p[0] = -k;
        p[1] = 0;
        p[2] = 0x12c000;
        func_020731dc(p, (void*)func_020072c0, &data_ov002_0210dda0);
    }
}
