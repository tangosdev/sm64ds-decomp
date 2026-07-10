//cpp
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN5ActorD1Ev(void*);
extern int RotatingPlatformRr_SpawnInfo[];
extern int data_ov056_02112158[];
int _ZN13SnowmanBreathD1Ev(char* c){
  *(int**)c = RotatingPlatformRr_SpawnInfo;
  func_0207328c(c+0xd4, 0x32, 0x60, data_ov056_02112158);
  _ZN5ActorD1Ev(c);
  return (int)c;
}
}
