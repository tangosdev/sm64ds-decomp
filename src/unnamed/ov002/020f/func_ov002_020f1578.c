extern int func_ov002_020dd8b8(void* c);

void func_ov002_020f1578(char* c, char* arg){
    int b = (int)(*(unsigned short*)(arg + 0xc) == 0xbf);
    if (b == 0) return;
    if (func_ov002_020dd8b8(arg) != 0)
        *(unsigned char*)(c + 0x32c) = 1;
}
