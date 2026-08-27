extern void *func_0205d23c(void *p, int a);
extern void func_0205caa8(void);
extern void func_0205c7c4(void *p, void *fn, int b);
extern void func_0205cb68(void *p, int a, int b0, int b4, int c0, int c4, void *fn1, void *fn2);
extern void func_0205db88(int idx, int a, int b);

extern int data_02099e6c;
extern void func_02042668(void);
extern void func_020426f8(void);
extern void func_020426f0(void);

struct Pair { int *f0; int f4; };

int func_020423dc(int *r6)
{
    int r5;
    void *h;
    int r3;
    struct Pair arr[4];

    h = func_0205d23c(&data_02099e6c, 3);
    func_0205caa8();
    func_0205c7c4(h, &func_02042668, -1);
    r5 = *r6;
    r6++;
    for (r3 = 0; r3 < 4; r3++) {
        arr[r3].f0 = r6 + 1;
        arr[r3].f4 = *r6;
        r6 = (int *)((char *)arr[r3].f0 + ((arr[r3].f4 + 0x1f) & ~0x1f));
    }
    func_0205cb68(h, 0, (int)arr[0].f0, arr[0].f4, (int)arr[1].f0, arr[1].f4,
                  &func_020426f8, &func_020426f0);
    func_0205db88(0, (int)arr[2].f0, arr[2].f4);
    func_0205db88(1, (int)arr[3].f0, arr[3].f4);
    return r5;
}
