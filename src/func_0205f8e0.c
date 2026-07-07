typedef enum { FALSE, TRUE } Bool;

extern int func_0205fb1c(int a, int b);

void func_0205f8e0(int *out0, int *out1)
{
    short tmp;
    Bool b8;
    Bool b4;

    if (func_0205fb1c(0, (int)&tmp) != 0) {
        return;
    }
    if (out0 != 0) {
        b8 = (Bool)((tmp & 8) != 0);
        *out0 = (int)b8;
    }
    if (out1 != 0) {
        b4 = (Bool)((tmp & 4) != 0);
        *out1 = (int)b4;
    }
}