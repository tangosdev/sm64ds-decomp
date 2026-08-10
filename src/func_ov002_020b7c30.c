extern int _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern int _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void*, void*, unsigned int);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern int func_ov002_020b6fcc(void*);
extern int _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void*, int, int, int, int);
extern int _ZN8SaveData13PlayerLoseCapEv(void);
extern int _ZN9ActorBase18MarkForDestructionEv(void*);
extern int data_02092138;

int func_ov002_020b7c30(void* c) {
  if (*(int*)((char*)c + 0x9c) != 0) {
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (char*)c + 0x110);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (char*)c + 0x144, 0);
    *(unsigned*)((char*)c + 0x12c) |= 0x8000;
    if (_ZNK12WithMeshClsn10IsOnGroundEv((char*)c + 0x144)) {
      *(int*)((char*)c + 0x98) = 0;
      func_ov002_020b6fcc(c);
      _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(c, 0x32000, 0x32000, 0x1000000, 0x1000000);
    }
  }
  if (data_02092138 > *(int*)((char*)c + 0x60)) {
    _ZN8SaveData13PlayerLoseCapEv();
    _ZN9ActorBase18MarkForDestructionEv(c);
  }
  return 1;
}
