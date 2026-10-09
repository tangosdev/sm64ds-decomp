//cpp
extern "C" int __aeabi_idiv(int a, int b);

void UpdateAngle(short &angle, short target, int divisor, short maxStep) {
    short step = (short)(__aeabi_idiv((short)(target - angle), divisor));
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
