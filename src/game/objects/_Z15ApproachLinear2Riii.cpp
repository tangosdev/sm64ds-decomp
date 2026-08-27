//cpp
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
            *p = (unsigned char)target;
            return 1;
        }
        *p = (unsigned char)ip;
    } else if (*p == target) {
        return 1;
    }
    return 0;
}
