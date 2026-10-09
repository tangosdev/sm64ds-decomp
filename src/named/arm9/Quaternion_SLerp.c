typedef long long s64;
void Quaternion_SLerp(int *q0, int *q1, int t, int *out) {
    int u = 0x1000 - t;
    out[0] = (int)(((s64)q0[0] * u + 0x800) >> 12) + (int)(((s64)q1[0] * t + 0x800) >> 12);
    out[1] = (int)(((s64)q0[1] * u + 0x800) >> 12) + (int)(((s64)q1[1] * t + 0x800) >> 12);
    out[2] = (int)(((s64)q0[2] * u + 0x800) >> 12) + (int)(((s64)q1[2] * t + 0x800) >> 12);
    out[3] = (int)(((s64)q0[3] * u + 0x800) >> 12) + (int)(((s64)q1[3] * t + 0x800) >> 12);
}
