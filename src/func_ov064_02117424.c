extern void func_ov064_02116ec0(char* c);
extern int data_ov064_0211b834;

void func_ov064_02117424(char* c) {
    *(int*)(c + 0x3fc) = 0;
    *(int*)(c + 0x330) = (int)&data_ov064_0211b834;
    func_ov064_02116ec0(c);
}
