extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj();
extern int data_ov073_02122f88[];
int func_ov071_0211fbf4(char* c){
    *(int*)((char*)c+0x9c)=-0x2000;
    *(int*)((char*)c+0xa0)=-0x3c000;
    *(int*)((char*)c+0x98)=0x4000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, data_ov073_02122f88[1], 0, 0x1000, 0);
    *(int*)((char*)c+0x130)=0x1000;
    *(int*)((char*)c+0x39c)=5;
    return 1;
}
