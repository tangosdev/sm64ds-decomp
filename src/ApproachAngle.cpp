//cpp
/* Step an angle toward a target, and the linear approaches beside it.
 * Definitions run high address first so reverse emission is ROM order.
 *
 * leftovers:
 * - ApproachLinear2(int &) reads and writes the reference through an
 *   unsigned char *. A full-word store widens the byte the ROM keeps.
 * - ApproachAngle and UpdateAngle call __aeabi_idiv. The declaration is
 *   local; the host build has no divider.
 */

#ifndef _MSC_VER
/* The host smoke build has no divider. These two call __aeabi_idiv. */
extern "C" int __aeabi_idiv(int a, int b);

extern "C" int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep)
{
    int diff;
    short c;
    c = *cur;
    diff = (((long long)(short)(target - c)));
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

void UpdateAngle(short &angle, short target, int divisor, short maxStep) {
    short step = (__aeabi_idiv((short)(target - angle), divisor));
    if (step > maxStep) {
        angle = angle + maxStep;
        return;
    }
    if (step < -maxStep) {
        angle = angle - maxStep;
    } else {
        angle = angle + step;
    }
}
#endif

int ApproachLinear2(int &ref, int target, int step)
{
    unsigned char *p = (unsigned char *)&ref;
    int cur;
    int ip;
    if (step != 0) {
        cur = *p;
        if (cur > target)
            step = (short)(-step);
        ip = (short)(cur + step);
        if (step * (ip - target) >= 0) {
            *p = target;
            return 1;
        }
        *p = ip;
    } else if (*p == target) {
        return 1;
    }
    return 0;
}

int ApproachLinear2(short &x, short target, short step)
{
    if (step != 0) {
        if (x > target) step = -step;
        x += step;
        if (step * (x - target) >= 0) {
            x = target;
            return 1;
        }
    } else if (x == target) {
        return 1;
    }
    return 0;
}

int ApproachLinear(int &ref, int target, int step)
{
    if (step != 0) {
        if (ref > target)
            step = -step;
        ref = ref + step;
        if ((long long)step * (ref - target) >= 0) {
            ref = target;
            return 1;
        }
    } else if (ref == target) {
        return 1;
    }
    return 0;
}

bool ApproachLinear(short &x, short target, short step)
{
    if (step != 0) {
        short d = (x - target);
        if (d > 0) step = -step;
        x += step;
        d = (x - target);
        if (step * d >= 0) {
            x = target;
            return true;
        }
    } else if (x == target) {
        return true;
    }
    return false;
}
