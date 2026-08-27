//cpp
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(char* w);
extern void _ZN12dEnemyBase_c9SpawnCoinEv(char* c);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(char* c);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
  unsigned int a, unsigned int b, int c, int d, int e, const void* f, void* g);
extern void* _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
  unsigned int a, unsigned int b, int c, int d, int e, const void* f);
}
extern "C" int func_ov002_020ae4cc(char* self, char* clsn){
  int v[3];
  int n = *(int*)(self+0x84);
  if (n == 0) {
    int z = *(int*)(self+0x64);
    int y = *(int*)(self+0x60) + 0x50000;
    int x = *(int*)(self+0x5c);
    v[0] = x;
    v[1] = y;
    v[2] = z;
  } else {
    int z = *(int*)(self+0x64);
    int y = *(int*)(self+0x60) + n * 0x50;
    int x = *(int*)(self+0x5c);
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {
    _ZN12dEnemyBase_c9SpawnCoinEv(self);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
    *(int*)(self+0x10c) = 0;
    return 1;
  }
  *(void**)(self+0xf8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    *(unsigned int*)(self+0xf8), 0x13a, *(int*)((char*)v + 0), *(int*)((char*)v + 4), *(int*)((char*)v + 8), 0, 0);
  *(void**)(self+0xfc) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    *(unsigned int*)(self+0xfc), 0x13b, *(int*)((char*)v + 0), *(int*)((char*)v + 4), *(int*)((char*)v + 8), 0);
  return 0;
}
