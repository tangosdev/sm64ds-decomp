void _ZN14dScMgMemory2_c15SetupDifficultyEv(char* c) {
    *(unsigned char*)(c + 0x540a) = 0;
    *(unsigned short*)(c + 0x53ea) = 8;
    int x = *(int*)(c + 0xb4);
    if (x >= 0xa) {
        *(unsigned char*)(c + 0x540a) = 2;
        *(unsigned short*)(c + 0x53ea) = 0xa;
        return;
    }
    if (x >= 5) {
        *(unsigned char*)(c + 0x540a) = 1;
        *(unsigned short*)(c + 0x53ea) = 9;
    }
}
