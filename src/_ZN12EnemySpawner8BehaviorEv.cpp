//cpp
extern "C" {
extern int _ZN5Event6GetBitEj(unsigned int bit);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
  unsigned int id, unsigned int a2, void* pos, void* rot, int a5, int a6);
extern void func_ov102_0214ad14(void* c);
int _ZN12EnemySpawner8BehaviorEv(char* c){
  if (_ZN5Event6GetBitEj(*(unsigned char*)(c+0xdc)) && *(int*)(c+0xd8) == 0) {
    void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
      *(unsigned short*)(c+0xd4), 4, c+0x5c, c+0x92, *(signed char*)(c+0xcc), -1);
    if (a != 0) func_ov102_0214ad14(a);
  }
  *(int*)(c+0xd8) = _ZN5Event6GetBitEj(*(unsigned char*)(c+0xdc));
  return 1;
}
}
