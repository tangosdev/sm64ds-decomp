void Math_Function_0203b0fc(int *p, int target, int scale, int max)
{
    int delta;
    int d;
    if (*p == target) return;
    d = target - *p;
    delta = (int)(((long long)d * scale + 0x800) >> 12);
    if (delta > max) {
        delta = max;
    } else if (delta < -max) {
        delta = -max;
    }
    *p += delta;
}
