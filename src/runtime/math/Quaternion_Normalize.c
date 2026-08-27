typedef int s32;
typedef long long s64;
extern s32 func_02053130(s32 r0);

void Quaternion_Normalize(s32 *q)
{
    s64 ss;
    s32 inv;
    ss = (s64)q[0]*q[0];
    ss += (s64)q[1]*q[1];
    ss += (s64)q[2]*q[2];
    ss += (s64)q[3]*q[3];
    inv = func_02053130((s32)(ss >> 12));
    q[0] = (s32)(((s64)inv*q[0] + 0x800) >> 12);
    q[1] = (s32)(((s64)inv*q[1] + 0x800) >> 12);
    q[2] = (s32)(((s64)inv*q[2] + 0x800) >> 12);
    q[3] = (s32)(((s64)inv*q[3] + 0x800) >> 12);
}
