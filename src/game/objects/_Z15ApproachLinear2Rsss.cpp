//cpp
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
