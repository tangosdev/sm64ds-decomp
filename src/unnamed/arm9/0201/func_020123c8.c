extern void func_020485f0(int a0, int a1);
extern void func_02048528(int a0, int a1);
extern void func_02048d80(int a0, int a1);

void func_020123c8(int a0, int type, int code, int arg)
{
    if (type == 3 && code >= 0x100 && code <= 0x103) {
        func_020485f0(a0, arg);
        return;
    }
    if (type == 3 && code == 0x18d) {
        func_02048528(a0, arg);
        return;
    }
    if (type == 3 && code == 0x81) {
        func_020485f0(a0, arg);
        return;
    }
    func_02048d80(a0, arg);
}
