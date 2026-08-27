extern int _ZN9Animation8FinishedEv();
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
extern int data_ov075_0211d3ec[];
int func_ov075_021141b8(char* c){
    int r=_ZN9Animation8FinishedEv((char*)c+0x50);
    if(!r) return r;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c, data_ov075_0211d3ec[1], 4, 0, 0x1000, 0);
    *(int*)((char*)c+0x114)=0;
    return 0;
}
