//cpp
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void*);
extern void _ZN5Enemy9SpawnCoinEv(void*);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void*);
int func_ov002_020ae454(char* c, void* a){
  if(*(unsigned short*)(c+0x102)==0 || _ZNK12WithMeshClsn10IsOnGroundEv(a)!=0 || _ZNK12WithMeshClsn8IsOnWallEv(a)!=0){
    _ZN5Enemy9SpawnCoinEv(c);
    _ZN5Actor24KillAndTrackInDeathTableEv(c);
    *(int*)(c+0x10c)=0;
    return 1;
  }
  return 0;
}
}
