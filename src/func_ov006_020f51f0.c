void _ZN13dScMgMemory_c15SetupDifficultyEv(char* c) {
    *(unsigned char*)(c + 0x533c) = 0;
    *(unsigned short*)(c + 0x532a) = 4;
    int x = *(int*)(c + 0xb4);
    if (x >= 0xa) {
        *(unsigned char*)(c + 0x533c) = 2;
        *(unsigned short*)(c + 0x532a) = 6;
        return;
    }
    if (x >= 5) {
        *(unsigned char*)(c + 0x533c) = 1;
        *(unsigned short*)(c + 0x532a) = 5;
    }
}
