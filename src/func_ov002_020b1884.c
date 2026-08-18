extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(char*);
extern int _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, void*);
extern void GiveCoins(int idx, int amount);
extern int _ZN6Player4HealEi(char*, int);
void func_ov002_020b1884(char* c, char* r4){
  *(short*)(c+0x3a8)=0;
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
  if(*(unsigned char*)(r4+0x706))
    _ZN5Sound9PlayBank3EjRK7Vector3(0x12, c+0x74);
  else
    _ZN5Sound9PlayBank3EjRK7Vector3(0x11, c+0x74);
  GiveCoins(*(unsigned char*)(r4+0x6d8), 1);
  _ZN6Player4HealEi(r4, 0x100);
}
