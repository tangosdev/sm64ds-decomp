//cpp
extern "C" {
extern void *_ZN5Actor10FindWithIDEj(unsigned int);
extern int _ZN6Player9StartTalkER9ActorBaseb(void*,void*,char);
extern int _ZN7Message11PrepareTalkEv(void);
extern int func_ov085_02129524(void*,int);
void func_ov085_02129470(char *c){
    if((*(int*)(c+0xf4) & 0x8000000)==0) return;
    void *o=_ZN5Actor10FindWithIDEj(*(unsigned int*)(c+0xf8));
    if(o==0) return;
    int eq = (*(unsigned short*)((char*)o+0xc)==0xbf) ? 1 : 0;
    if(!eq) return;
    *(void**)(c+0x1f8)=o;
    if(_ZN6Player9StartTalkER9ActorBaseb(*(void**)(c+0x1f8), c, 0)==0) return;
    _ZN7Message11PrepareTalkEv();
    func_ov085_02129524(c, 1);
}
}
