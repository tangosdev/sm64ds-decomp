extern unsigned short FRAME_COUNTER[];
extern unsigned short data_020a0f30[];
int func_0203d81c(void){
    FRAME_COUNTER[6] |= 0x1000;
    if (data_020a0f30[0] != 0){
        data_020a0f30[0] = 0;
        return 1;
    }
    return 0;
}
