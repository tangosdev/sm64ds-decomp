/* func_ov006_020e6e3c at 0x020e6e3c - thunk: func_02012718(a, b + 0x80000) */

extern int func_02012718(int a, int b);

int func_ov006_020e6e3c(int a, int b)
{
    return func_02012718(a, b + 0x80000);
}
