extern int func_ov074_02121a20(void *c, int idx);
extern int ApproachAngle(short *cur, int target, int divisor, int band, int maxStep);
extern void func_ov074_02121a4c(void *c, int idx);
extern void func_ov074_021216f4(void *c);
extern int _ZN9Animation8FinishedEv(void *self);

int func_ov074_021206c8(char *c) {
    if (func_ov074_02121a20(c, 8) != 0) {
        if (ApproachAngle((short *)(c + 0x5f6), 0, 0x1e, 0x20, 1) == 0) {
            if ((unsigned short)(*(int *)(c + 0x268) >> 12) == 0) {
                func_ov074_02121a4c(c, 9);
            }
            goto ret0;
        }
        func_ov074_021216f4(c);
        goto ret0;
    }
    if (func_ov074_02121a20(c, 0xa) != 0) {
        func_ov074_02121a4c(c, 9);
        goto ret0;
    }
    if (func_ov074_02121a20(c, 9) != 0) {
        if (_ZN9Animation8FinishedEv(c + 0x260) != 0) {
            return 1;
        }
        goto ret0;
    }
    return 1;
ret0:
    return 0;
}
