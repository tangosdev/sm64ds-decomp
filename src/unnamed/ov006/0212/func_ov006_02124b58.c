extern void func_ov006_020c0c80(void* c);
extern void func_ov006_020c0d68(void* c);
void func_ov006_02124b58(char* c) {
    if (*(int*)(c + 0x51b8) == 7) {
        func_ov006_020c0c80(c + 0x4f38);
        *(int*)(c + 0x51b8) = 9;
    } else {
        func_ov006_020c0d68(c + 0x4f38);
        *(int*)(c + 0x51b8) = 0xa;
    }
}
