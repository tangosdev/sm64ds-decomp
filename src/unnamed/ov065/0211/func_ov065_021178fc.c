extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj();
extern int* data_ov065_0211d6a0;
int func_ov065_021178fc(void* c) {
    *(int*)((char*)c + 0xb0) = 3;
    *(unsigned char*)((char*)c + 0x43c) = 0;
    *(int*)((char*)c + 0x3c0) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x364, (&data_ov065_0211d6a0)[1], 0, 0x1000, 0);
    return 1;
}
