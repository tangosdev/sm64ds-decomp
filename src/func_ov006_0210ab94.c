extern unsigned char DecIfAbove0_Byte(unsigned char* p);
void func_ov004_020b0a54(void* c);

void func_ov006_0210ab94(char* c){
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x503e)) == 0) {
        *(int*)(c + 0x5000) = 7;
        func_ov004_020b0a54((void*)0x12);
        *(int*)(c + 0x5004) = 0;
    }
    if (*(unsigned char*)(c + 0x503e) >= 0x3c) return;
    {
        int* a = (int*)(((int)c + 0x4ff4) & 0xFFFFFFFFFFFFFFFF);
        int* b = (int*)(((int)c + 0x4ff8) & 0xFFFFFFFFFFFFFFFF);
        int* d = (int*)(((int)c + 0x4ffc) & 0xFFFFFFFFFFFFFFFF);
        *a = *a + 0x10000;
        *b = *b - 0x10000;
        *d = *d + 0x10000;
    }
}
