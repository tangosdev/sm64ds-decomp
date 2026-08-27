extern int func_ov078_02123864();
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
extern int data_ov078_02126f20[];
int func_ov078_02123a3c(char* c){
    *(int*)(c+0x4fc)=2;
    *(int*)(c+0x98)=0;
    *(short*)(c+0x100)=0x32;
    func_ov078_02123864(c);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x2cc, data_ov078_02126f20[1], 0, 0, 0x1000, 0);
    return 1;
}
