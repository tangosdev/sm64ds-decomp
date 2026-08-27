//cpp
extern "C" {
extern void *data_ov004_020beb68;
int func_ov004_020ae1cc(void);
int func_ov004_020ae1f0(void);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);
void func_ov004_020ae2c8(void);
}
void func_ov004_020ae2c8(void) {
    if (data_ov004_020beb68 == 0) return;
    int r = func_ov004_020ae1cc();
    if (r == 0) {
        _ZN5Sound22StopLoadedMusic_Layer1Ej(1);
        return;
    }
    if (r != 1) return;
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(func_ov004_020ae1f0());
}
