extern void SetBg2Offset(int a, int b);

void func_ov006_020d5c60(char* p) {
    int zero = 0;
    *(int*)(p + 0x62dc) = zero;
    *(unsigned char*)(p + 0x62f4) = zero;
    *(unsigned short*)(p + 0x62f2) = zero;
    SetBg2Offset(zero, zero);
}
