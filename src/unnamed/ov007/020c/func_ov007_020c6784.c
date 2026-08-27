extern int func_ov007_020c3ce4(void);
extern void func_ov007_020c6724(void *self, int flag);
extern void *func_ov007_020c2b38(int count, int *src1, int *src2, int arg3);
extern void *func_ov007_020c2acc(int count, unsigned char *a, unsigned char *b, int x);
extern void func_ov007_020c6e68(void *self, int i);
extern int func_ov007_020c5db0(void *c);

struct C {
    char p8[8];
    int f8;       // 0x8
    char p[8];
    int f14;      // 0x14
    char p2[0x20];
    void **f38;   // 0x38
};

void func_ov007_020c6784(struct C *self, int count, int *p2, int *p3,
                         int *p4, int *p5, int mode)
{
    int i;
    func_ov007_020c3ce4();
    func_ov007_020c6724(self, 1);
    for (i = 0; i < count; i++) {
        switch (mode) {
        case 0:
            self->f38[i] = func_ov007_020c2b38(p2[i], (int*)p3[i], (int*)p4[i], p5[i]);
            break;
        case 1:
            self->f38[i] = func_ov007_020c2acc(p2[i], (unsigned char*)p3[i], (unsigned char*)p4[i], p5[i]);
            break;
        }
        func_ov007_020c6e68(self, i);
    }
    self->f8 = count;
    self->f14 = func_ov007_020c5db0(self);
    func_ov007_020c3ce4();
}
