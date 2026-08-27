struct S { char *f0; };

extern struct S *data_ov007_02104ba0;

extern void func_ov007_020c5c14(char *self, char *out);
extern void func_ov007_020c64c4(char *c, int b);
extern void func_ov007_020c62cc(char *a, int b);
extern void func_ov007_020c55bc(char *c, int a, int b);
extern void func_ov007_020c56bc(char *c, int a, int b, int d, int e, int f);

void func_ov007_020bbc08(int mode, int a1, int a2)
{
    char *p;

    switch (mode) {
    case 0:
    case 1:
        if (mode == 1) {
            func_ov007_020c5c14(data_ov007_02104ba0->f0, data_ov007_02104ba0->f0 + 0x70);
        }
        p = data_ov007_02104ba0->f0;
        func_ov007_020c64c4(p, 0);
        func_ov007_020c62cc(p, 1);
        func_ov007_020c55bc(data_ov007_02104ba0->f0, 0x1000, 0x1000);
        *(int *)(data_ov007_02104ba0->f0 + 0x8c) = 0;
        *(int *)(data_ov007_02104ba0->f0 + 0x88) = *(int *)(data_ov007_02104ba0->f0 + 0x8c);
        break;

    case 2:
        func_ov007_020c56bc(data_ov007_02104ba0->f0, 1, a1, a2 / 2, a2 / 2, 8);
        break;

    case 3:
        func_ov007_020c56bc(data_ov007_02104ba0->f0, 2, a1, a2, a2, 6);
        break;
    }
}
