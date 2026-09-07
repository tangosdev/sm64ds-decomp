extern int _ZN9Animation7AdvanceEv();
extern int _ZN8dActor_c9UpdatePosEP5dCc_c();
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer();
extern int _ZNK10dBgCh_Actr13JustHitGroundEv();
extern int DecIfAbove0_Byte();
extern void func_ov077_02123a1c(char* c);
int func_ov077_02123fcc(char* c){
    *(short*)((char*)c+0x8c)=*(short*)((char*)c+0x8c)-0x1000;
    _ZN9Animation7AdvanceEv((char*)c+0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char*)c+0x1c4);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer((char*)c+0x204);
    if(!_ZNK10dBgCh_Actr13JustHitGroundEv((char*)c+0x204)){
        if(DecIfAbove0_Byte((char*)c+0x41c)) goto end;
    }
    func_ov077_02123a1c(c);
end:
    return 1;
}
