extern unsigned char data_ov066_0211ae04;
extern int data_ov066_0211b06c;
extern int data_ov066_0211b08c;
extern int data_ov066_0211b0bc;
extern int data_ov066_0211b0ec;
extern int data_ov066_0211afec;
extern int data_ov066_0211b01c;
extern int data_ov066_0211b04c;
extern int func_ov066_02119454(void *c, void *p);

int func_ov066_021168ec(void *c) {
    unsigned char m;
    if (*(int*)((char*)c + 0x9c) != 0) return 0;
    m = data_ov066_0211ae04;
    if (m == 3) {
        if (*(void**)((char*)c + 0x48c) != (void*)&data_ov066_0211b06c) {
            func_ov066_02119454(c, (void*)&data_ov066_0211b06c);
            return 3;
        }
    }
    if ((unsigned char)(m + 0xfc) <= 5
        && *(void**)((char*)c + 0x48c) == (void*)&data_ov066_0211b06c) {
        if (m == 4) { func_ov066_02119454(c, (void*)&data_ov066_0211b08c); return data_ov066_0211ae04; }
        if (m == 5) { func_ov066_02119454(c, (void*)&data_ov066_0211b0bc); return data_ov066_0211ae04; }
        if (m == 6) { func_ov066_02119454(c, (void*)&data_ov066_0211b0ec); return data_ov066_0211ae04; }
        if (m == 7) { func_ov066_02119454(c, (void*)&data_ov066_0211afec); return data_ov066_0211ae04; }
        if (m == 8) { func_ov066_02119454(c, (void*)&data_ov066_0211b01c); return data_ov066_0211ae04; }
        if (m == 9) { func_ov066_02119454(c, (void*)&data_ov066_0211b04c); return data_ov066_0211ae04; }
    }
    return 0;
}
