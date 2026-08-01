extern int data_ov007_02103260;
extern short data_ov007_02104c28;
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
int func_ov007_020cc118(int a, unsigned int b) {
    if (data_ov007_02103260 >= 0) {
        return 0;
    }
    data_ov007_02103260 = a;
    data_ov007_02104c28 = (short)b;
    _ZN5Sound22StopLoadedMusic_Layer1Ej(b);
    return 1;
}
