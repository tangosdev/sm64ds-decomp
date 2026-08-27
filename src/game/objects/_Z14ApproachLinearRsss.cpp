//cpp
int ApproachLinear(short &x, short target, short step)
{
    if (step != 0) {
        short d = (short)(x - target);
        if (d > 0) step = -step;
        x += step;
        d = (short)(x - target);
        if (step * d >= 0) {
            x = target;
            return 1;
        }
    } else if (x == target) {
        return 1;
    }
    return 0;
}
