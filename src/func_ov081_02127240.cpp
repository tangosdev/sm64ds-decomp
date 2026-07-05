//cpp
extern "C" {
extern void func_0201267c(int, void*);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void*);
extern void func_ov081_021265c8(void*);
extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern int func_ov081_02126950(void*, void*);
extern void func_ov081_02126758(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
int func_ov081_02127240(char* c){
  switch(*(unsigned char*)(c+0x3f1)){
  case 0:
    if((((unsigned int)*(int*)(c+0x12c))<<4)>>16 == 7){
      *(int*)(c+0x98)=0x12000;
      *(int*)(c+0xa8)=0x1e000;
      func_0201267c(0x77, c+0x74);
      (*(unsigned char*)(((int)c + 0x3f1) & 0xFFFFFFFFFFFFFFFF))++;
    }
    break;
  case 1:
    if(_ZNK12WithMeshClsn13JustHitGroundEv(c+0x1e4)){
      func_0201267c(0x71, c+0x74);
      func_ov081_021265c8(c);
    }
    break;
  }
  _ZN9Animation7AdvanceEv(c+0x124);
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x1b0);
  func_ov081_02126950(c, c+0x1e4);
  func_ov081_02126758(c);
  _ZN12CylinderClsn5ClearEv(c+0x1b0);
  _ZN12CylinderClsn6UpdateEv(c+0x1b0);
  return 1;
}
}
