extern int data_ov004_020beb68;
extern int func_ov004_020ae1cc(void);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
extern int func_ov004_020ae1f0(void);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);
extern unsigned short data_0209b308;

void func_ov004_020ae330(void){
    if(data_ov004_020beb68==0) return;
    if(func_ov004_020ae1cc()==0){
        _ZN5Sound22StopLoadedMusic_Layer1Ej(1);
        if(data_0209b308 != 0x16f) return;
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(func_ov004_020ae1f0());
        return;
    }
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(func_ov004_020ae1f0());
}
