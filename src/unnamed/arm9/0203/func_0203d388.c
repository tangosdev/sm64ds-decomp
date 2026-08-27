typedef short s16;
typedef long long s64;

extern s16 data_02082214[];

void func_0203d388(int *p, int angle)
{
    int a = (unsigned short)angle >> 4;
    int c = data_02082214[a * 2];
    int s = data_02082214[a * 2 + 1];
    int x = p[0];
    int z = p[1];
    p[0] = (int)(((s64)s * x + 0x800) >> 12) - (int)(((s64)c * z + 0x800) >> 12);
    p[1] = (int)(((s64)c * x + 0x800) >> 12) + (int)(((s64)s * z + 0x800) >> 12);
}
