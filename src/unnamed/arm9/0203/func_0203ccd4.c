extern short data_02082214[];

void func_0203ccd4(int *m, short angle)
{
    unsigned int i = (unsigned short)angle >> 4;
    short s = data_02082214[i * 2];
    short c = data_02082214[i * 2 + 1];
    int x = m[0];
    int z = m[2];
    int nx = (int)((((long long)c * x) + 0x800) >> 12) + (int)((((long long)s * z) + 0x800) >> 12);
    int nz = (int)((((long long)c * z) + 0x800) >> 12) - (int)((((long long)s * x) + 0x800) >> 12);
    m[0] = nx;
    m[2] = nz;
}
