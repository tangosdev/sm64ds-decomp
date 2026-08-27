typedef long long s64;

void MulVec3Mat3x3(const int *in, const int *m, int *out)
{
    int x = in[0];
    int y = in[1];
    int z = in[2];

    out[0] = (int)(((s64)x * m[0] + (s64)y * m[3] + (s64)z * m[6]) >> 12);
    out[1] = (int)(((s64)x * m[1] + (s64)y * m[4] + (s64)z * m[7]) >> 12);
    out[2] = (int)(((s64)x * m[2] + (s64)y * m[5] + (s64)z * m[8]) >> 12);
}
