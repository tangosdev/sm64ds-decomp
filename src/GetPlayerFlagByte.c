extern unsigned char data_020a1166[];
unsigned char GetPlayerFlagByte(int i, int p) {
    return *(unsigned char*)((char*)data_020a1166 + i * 0x24 + p);
}
