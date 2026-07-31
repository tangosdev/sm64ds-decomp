extern int _ZN9Animation7AdvanceEv();
extern int _ZN5Actor9UpdatePosEP12CylinderClsn();
extern int WithMeshClsn_UpdateDiscreteNoLava_veneer();
extern int _ZNK12WithMeshClsn13JustHitGroundEv();
extern int DecIfAbove0_Byte();
extern int _ZN5Actor8PoofDustEv();
extern int _ZN9ActorBase18MarkForDestructionEv();
int func_ov070_021213cc(char *c){
 *(short*)(c+0x8c)=*(short*)(c+0x8c)-0x1000;
 _ZN9Animation7AdvanceEv((char*)c+0x124);
 _ZN5Actor9UpdatePosEP12CylinderClsn(c,(char*)c+0x160);
 WithMeshClsn_UpdateDiscreteNoLava_veneer((char*)c+0x1a0);
 if(_ZNK12WithMeshClsn13JustHitGroundEv((char*)c+0x1a0)==0){
   if(DecIfAbove0_Byte((char*)c+0x3ac)!=0) goto end;
 }
 _ZN5Actor8PoofDustEv(c);
 _ZN9ActorBase18MarkForDestructionEv(c);
end:
 return 1;
}
