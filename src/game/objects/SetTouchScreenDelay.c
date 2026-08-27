extern unsigned char data_0209f21c;
extern unsigned char data_0209f498[];

void SetTouchScreenDelay(void){
    unsigned char* p = data_0209f498;
    int i;
    for (i = 0; i < data_0209f21c; i++) {
        p[0x12] = 5;
        p += 0x18;
    }
}
