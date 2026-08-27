//cpp
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
