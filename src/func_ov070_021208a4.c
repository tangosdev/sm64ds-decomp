extern int _ZN9Animation7AdvanceEv();
extern int _ZN8dActor_c9UpdatePosEP5dCc_c();
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer();
extern int _ZNK10dBgCh_Actr13JustHitGroundEv();
extern int DecIfAbove0_Byte();
extern int _ZN8dActor_c8PoofDustEv();
extern int _ZN7fBase_c18MarkForDestructionEv();
int func_ov070_021208a4(char *c){
 *(short*)(c+0x8c)=*(short*)(c+0x8c)-0x1000;
 _ZN9Animation7AdvanceEv((char*)c+0x124);
 _ZN8dActor_c9UpdatePosEP5dCc_c(c,(char*)c+0x1d8);
 dBgCh_Actr_UpdateDiscreteNoLava_veneer((char*)c+0x218);
 if(_ZNK10dBgCh_Actr13JustHitGroundEv((char*)c+0x218)==0){
   if(DecIfAbove0_Byte((char*)c+0x430)!=0) goto end;
 }
 _ZN8dActor_c8PoofDustEv(c);
 _ZN7fBase_c18MarkForDestructionEv(c);
end:
 return 1;
}
