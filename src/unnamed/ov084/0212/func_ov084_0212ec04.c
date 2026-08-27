extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int a, int b, unsigned int u);
extern void* data_ov084_02130e14[];
void func_ov084_0212ec04(char* c, int arg) {
    *(int*)(c + 0x458) = 2;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130e14[1], 0x40000000, 0x1000, 0);
    *(unsigned int*)(c + 0x168) = ((unsigned int)(arg << 0x10)) >> 4;
}
