extern int func_020618b8(void);
extern unsigned short data_020a0f1c[];
int func_0203d9f4(void){
    if ((data_020a0f1c[0] & 0x8000) == 0) return 0;
    return func_020618b8();
}
