typedef struct { int x, y, z; } Vec3;
extern void func_02007d0c(void *self, Vec3 *src, int a2, int a3, int a4);

int func_02007548(int p0)
{
    int *base = (int *)((*(int *)(p0 + 0x110) + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    int y0 = base[1] + 0xffecd000;
    int tz = base[2];
    int tx = base[0];
    Vec3 src;
    src.y = 0x197000 + (y0 >> 1);
    src.x = tx;
    src.z = tz;
    func_02007d0c((void *)(p0 + 0x80), &src, 0x800, 0xccc, 0x800);
    return 1;
}
