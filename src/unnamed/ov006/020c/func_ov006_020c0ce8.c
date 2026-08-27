typedef int Fix12;
extern int _ZN9Animation8FinishedEv();
extern int func_ov006_020c1764();
extern int _ZN5Sound12PlayBank2_2DEj();
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*,void*,int,int,Fix12,unsigned short);
void func_ov006_020c0ce8(char *c){
    if(!_ZN9Animation8FinishedEv(c+0x6c)) return;
    if(*(int*)(c+0x7c)==*(int*)(c+0x25c)){
        func_ov006_020c1764(c);
        return;
    }
    _ZN5Sound12PlayBank2_2DEj(0x13c);
    *(short*)(c+0x1a)=0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x1c, *(void**)(c+0x25c), 0, 0x40000000, 0x800, 0);
}
