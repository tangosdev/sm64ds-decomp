void func_ov006_020fc1b4(char* base, int val) {
    int i;
    for (i = 0; i < 2; i++) {
        if (*(unsigned char*)(base + 0x4eb7)) {
            *(unsigned char*)(base + 0x4eb3) = 3;
            *(unsigned char*)(base + 0x4eba) = 0;
            *(unsigned char*)(base + 0x4eb4) = (unsigned char)val;
        }
        base += 0x1c;
    }
}
