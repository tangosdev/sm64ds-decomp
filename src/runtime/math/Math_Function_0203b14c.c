int Math_Function_0203b14c(int *ptr, int target, int rate, int limit, int step)
{
    int cur = *ptr;
    int v;
    int d;
    if (cur != target) {
        int diff = target - cur;
        v = (int)(((long long)diff * rate + 0x800) >> 12);
        if (v >= step || v <= -step) {
            if (v > limit) v = limit;
            if (v < -limit) v = -limit;
            *ptr += v;
        } else if (v > 0) {
            if (v < step) {
                *ptr = cur + step;
                if (*ptr > target) *ptr = target;
            }
        } else {
            if (v > -step) {
                int ns = -step;
                *ptr = cur + ns;
                if (*ptr < target) *ptr = target;
            }
        }
    }
    d = target - *ptr;
    if (d < 0) d = -d;
    return d;
}
