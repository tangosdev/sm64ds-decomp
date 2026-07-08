typedef short s16;
typedef long long s64;

extern s16 SINE_TABLE[];

void func_0203cc28(int *p, int angle)
{
    int a = (unsigned short)angle >> 4;
    int c = SINE_TABLE[a * 2];
    int s = SINE_TABLE[a * 2 + 1];
    int x = p[0];
    int z = p[1];
    p[0] = (int)(((s64)s * x + 0x800) >> 12) - (int)(((s64)c * z + 0x800) >> 12);
    p[1] = (int)(((s64)c * x + 0x800) >> 12) + (int)(((s64)s * z + 0x800) >> 12);
}
