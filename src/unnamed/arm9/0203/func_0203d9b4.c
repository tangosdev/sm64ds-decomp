extern unsigned char data_020a0f10;

int func_0203d9b4(void){
    int b = (int)(*(unsigned short*)0x27ffc40 == 2);
    if (b != 0) return 0;
    return *(unsigned char*)&data_020a0f10 == 0;
}
