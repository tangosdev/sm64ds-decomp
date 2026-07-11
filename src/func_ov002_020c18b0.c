typedef struct { int x, y, z; } Vector3;

extern int func_ov002_020c179c(char *self, int angle);
extern int func_02010844(void *unused, Vector3 *v, short angle);
extern int func_ov002_020bf30c(void *c, int a);
extern int __aeabi_idiv(int a, int b);
extern short data_02082214[];

void func_ov002_020c18b0(char *self, int p1)
{
    if (*(unsigned char *)(self + 0x6de) != 0) return;
    if (*(int *)(self + 0x558) == 0) return;
    {
        int a = func_ov002_020c179c(self, 0);
        int idx = (unsigned short)a >> 4;
        int sinv = data_02082214[idx * 2 + 1];
        int f554 = *(int *)(self + 0x554);
        int f55c = *(int *)(self + 0x55c);
        Vector3 v;
        v.x = f554;
        v.y = sinv;
        v.z = f55c;
        *(short *)(self + 0x8c) = (short)func_02010844(self, &v, *(short *)(self + 0x8e));
        *(short *)(self + 0x90) = (short)func_02010844(self, &v, (short)(*(short *)(self + 0x8e) - 0x4000));
    }
    if (p1 == 0) return;
    {
        int r0v = func_ov002_020bf30c(self, 0x28000);
        int divr = __aeabi_idiv(*(int *)(self + 0x98) << 12, r0v);
        *(short *)(self + 0x8c) = (short)(int)(((long long)*(short *)(self + 0x8c) * divr + 0x800) >> 12);
        *(short *)(self + 0x90) = (short)(int)(((long long)*(short *)(self + 0x90) * divr + 0x800) >> 12);
    }
}
