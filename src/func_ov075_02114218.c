extern int _ZN9Animation8FinishedEv();
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
extern int data_ov075_0211d3a4[];
int func_ov075_02114218(char* c){
    int r=_ZN9Animation8FinishedEv((char*)c+0x50);
    if(!r) return r;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c, data_ov075_0211d3a4[1], 4, 0x40000000, 0x1000, 0);
    *(int*)((char*)c+0x114)=3;
    return 3;
}
