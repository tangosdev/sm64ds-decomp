typedef struct Mat3 { int m[9]; } Mat3;

extern void Matrix3x3_LoadIdentity(Mat3 *m);
extern void func_ov007_020c3abc(Mat3 *out, int ax, int ay, int az);
extern void func_0205532c(int *a, int *v);

void func_ov007_020c42f8(char *self);
void func_ov007_020c42f8(char *self) {
    int vec[3];
    Mat3 mtx;
    int a, b;
    vec[0] = *(int *)(self + 0x14) + *(int *)(self + 0x28);
    vec[1] = *(int *)(self + 0x18) + *(int *)(self + 0x2c);
    vec[2] = *(int *)(self + 0x1c) + *(int *)(self + 0x30);
    Matrix3x3_LoadIdentity(&mtx);
    func_ov007_020c3abc(&mtx, *(unsigned short *)(self + 0x34), *(unsigned short *)(self + 0x36), *(unsigned short *)(self + 0x38));
    func_0205532c((int *)&mtx, vec);
    a = *(int *)(self + 0x3c);
    b = *(int *)(self + 0x40);
    *(volatile int *)0x400046c = a;
    *(volatile int *)0x400046c = b;
    *(volatile int *)0x400046c = 0x1000;
}
