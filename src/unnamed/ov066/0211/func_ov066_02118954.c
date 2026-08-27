typedef int s32;
extern s32 func_ov066_021168b0(char* c);
extern void func_ov066_02119454(char* c, int* p);
extern int data_ov066_0211ae10;
extern int data_ov066_0211b03c;
s32 func_ov066_02118954(char* c) {
    s32 r = func_ov066_021168b0(c);
    if (r == 0) {
        return 1;
    }
    *(char*)(c + 0x4d9) = 0;
    *(char*)&data_ov066_0211ae10 = 0;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
