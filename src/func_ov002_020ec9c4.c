typedef short s16;
extern void _Z15ApproachLinear2Rsss(s16* p, short a, short b);
extern int func_ov002_020d5f98(void* c, unsigned char* a, unsigned char* b);
extern int func_ov002_020d6048(void* c);
extern void _ZN9ActorBase18MarkForDestructionEv(void* c);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* c, void* cc);
extern void WithMeshClsn_UpdateContinuous_Veneer(void* p);
extern s16 data_ov002_021000a8[];
void func_ov002_020ec9c4(char* c){
  if (*(unsigned char*)(c + 0x41d) != 0){
    _Z15ApproachLinear2Rsss((s16*)(c + 0x8c), 0, 0x400);
    _Z15ApproachLinear2Rsss((s16*)(c + 0x90), 0, 0x400);
    if (*(s16*)(c + 0x8c) == 0){
      if (*(s16*)(c + 0x8e) == 0)
        *(unsigned char*)(c + 0x41d) = 0;
    }
  } else {
    unsigned char a, b;
    if (func_ov002_020d5f98(*(void**)(c + 0x38c), &a, &b)){
      unsigned char ip;
      a = a * 2;
      ip = a;
      *(s16*)(c + 0x8c) = b * data_ov002_021000a8[ip];
      *(s16*)(c + 0x90) = b * data_ov002_021000a8[ip + 1];
      *(unsigned char*)(c + 0x41d) = 1;
    }
  }
  if (!func_ov002_020d6048(*(void**)(c + 0x38c)))
    _ZN9ActorBase18MarkForDestructionEv(c);
  _ZN12CylinderClsn5ClearEv(c + 0x110);
  _ZN12CylinderClsn6UpdateEv(c + 0x110);
  if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144))
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
  WithMeshClsn_UpdateContinuous_Veneer(c + 0x144);
}
