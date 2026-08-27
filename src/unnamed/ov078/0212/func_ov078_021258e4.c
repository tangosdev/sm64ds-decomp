extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
typedef struct { int a, b; } P2;
extern P2 data_ov078_02126ef8;
int func_ov078_021258e4(int *t)
{
    *(int*)((char*)t+0x9c) = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char*)t+0x2cc, ((P2*)&data_ov078_02126ef8)->b, 8, 0x40000000, 0x1000, 0);
    if(*(unsigned char*)((char*)t+0x506)==0){
        *(unsigned char*)((char*)t+0x506)=1;
        *(int*)((char*)t+0xb0)=0x10000002;
    }
    return 1;
}
