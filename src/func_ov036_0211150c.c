extern short data_ov036_02113b18;
extern short data_ov036_02113b1c;
extern int data_ov036_02113b2c;
extern int func_ov002_020b676c(int *self, void *arg, int val);

int func_ov036_0211150c(int *self)
{
    short v;
    v = data_ov036_02113b18;
    if ((self[2] & 0xff) == 1) {
        v = data_ov036_02113b1c;
    }
    return func_ov002_020b676c(self, &data_ov036_02113b2c, v);
}
