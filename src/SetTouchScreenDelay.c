extern unsigned char NUM_PLAYERS;
extern unsigned char INPUT_ARR[];

void SetTouchScreenDelay(void){
    unsigned char* p = INPUT_ARR;
    int i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        p[0x12] = 5;
        p += 0x18;
    }
}
