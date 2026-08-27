extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov084_02130e0c[];
void func_ov084_0212f2dc(char *c){
    *(unsigned char*)(c + 0x45c) = 0;
    if(*(int*)(c + 0x464) <= 0x4b0000) return;
    *(int*)(c + 0x458) = 8;
    *(int*)(c + 0x474) = 0;
    *(int*)(c + 0x470) = *(int*)(c + 0x474);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, (void*)data_ov084_02130e0c[1], 0, 0x1000, 0);
}
