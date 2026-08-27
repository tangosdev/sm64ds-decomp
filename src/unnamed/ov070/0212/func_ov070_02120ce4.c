extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj();
extern int data_ov070_0212360c[];
int func_ov070_02120ce4(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov070_0212360c[1], 0x40000000, 0x1000, 0);
    *(unsigned char*)(c+0x430) = 0x3c;
    *(int*)(c+0x420) = 0;
    return 1;
}
