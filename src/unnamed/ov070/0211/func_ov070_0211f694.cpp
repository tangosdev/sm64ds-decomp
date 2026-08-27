//cpp
extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisp, void *bca, int a, int fix, unsigned int j);
extern int data_ov070_02123508[];
int func_ov070_0211f694(char *c) {
    *(int*)(c+0x3d8) = 0;
    if (*(int*)(c+0x3dc) == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)data_ov070_02123508[1], 0x40000000, 0x1000, 0);
    }
    return 1;
}
}
