/* Sound_PlayBank2Panned at 0x020e6e3c - thunk: func_02012718(a, b + 0x80000) */

extern int func_02012718(int a, int b);

int Sound_PlayBank2Panned(int a, int b)
{
    return func_02012718(a, b + 0x80000);
}
