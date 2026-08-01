extern int _ZN9Animation7AdvanceEv();
extern int _ZN5Actor9UpdatePosEP12CylinderClsn();
extern int WithMeshClsn_UpdateDiscreteNoLava_veneer();
extern int _ZNK12WithMeshClsn13JustHitGroundEv();
extern int DecIfAbove0_Byte();
extern int _ZN5Actor8PoofDustEv();
extern int _ZN9ActorBase18MarkForDestructionEv();
int func_ov070_021208a4(char *c){
 *(short*)(c+0x8c)=*(short*)(c+0x8c)-0x1000;
 _ZN9Animation7AdvanceEv((char*)c+0x124);
 _ZN5Actor9UpdatePosEP12CylinderClsn(c,(char*)c+0x1d8);
 WithMeshClsn_UpdateDiscreteNoLava_veneer((char*)c+0x218);
 if(_ZNK12WithMeshClsn13JustHitGroundEv((char*)c+0x218)==0){
   if(DecIfAbove0_Byte((char*)c+0x430)!=0) goto end;
 }
 _ZN5Actor8PoofDustEv(c);
 _ZN9ActorBase18MarkForDestructionEv(c);
end:
 return 1;
}
