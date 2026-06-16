struct Vector3 { int x,y,z; };
extern int func_ov102_02149684(struct Vector3* out, void* b);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int,unsigned int,struct Vector3*,void*,int,int);
extern int _ZN5Actor24KillAndTrackInDeathTableEv(void*);
void func_ov102_02149428(char* this){
  struct Vector3 pos;
  func_ov102_02149684(&pos, this);
  _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x114, 0, &pos, 0, *(signed char*)(this+0xcc), -1);
  _ZN5Actor24KillAndTrackInDeathTableEv(this);
}
