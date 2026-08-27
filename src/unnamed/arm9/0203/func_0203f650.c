extern void (*data_020a0f90)(short *);

void func_0203f650(int x)
{
    short v[2];
    void (*fn)(short *) = data_020a0f90;
    if (fn == 0)
        return;
    if (x > 0) {
        v[0] = 0;
        v[1] = (short)x;
    } else {
        v[0] = (short)x;
        v[1] = 0;
    }
    fn(v);
}
