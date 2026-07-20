int __aeabi_idiv(int a, int b);

int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep)
{
    int diff;
    short c;
    c = *cur;
    diff = (int)(((long long)(short)(target - c)) & 0xFFFFFFFFFFFFFFFFLL);
    if (c != target) {
        int step = (short)__aeabi_idiv(diff, divisor);
        if (step > maxStep || step < -maxStep) {
            if (step > band) step = band;
            else if (step < -band) step = (short)-band;
            *cur = *cur + step;
        } else if (diff >= 0) {
            *cur = *cur + maxStep;
            if ((short)(target - *cur) <= 0) *cur = target;
        } else {
            *cur = *cur - maxStep;
            if ((short)(target - *cur) >= 0) *cur = target;
        }
    }
    return target - *cur;
}
