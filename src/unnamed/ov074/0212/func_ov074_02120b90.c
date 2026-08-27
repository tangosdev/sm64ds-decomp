extern unsigned short data_ov074_02122dfc[];

void func_ov074_02120b90(void *c) {
    int idx = *(unsigned char*)((char*)c + 0x604);
    int r3 = *(int*)((char*)c + 0x5f0);
    unsigned short r1 = data_ov074_02122dfc[idx];
    *(short*)((char*)c + 0x5f8) = r3 * r1;
}
