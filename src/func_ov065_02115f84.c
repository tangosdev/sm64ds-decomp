extern int _ZN5Actor13SmallPoofDustEv(void*);
extern int _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void*,void*,unsigned,int,short);
extern int _ZN5Actor24KillAndTrackInDeathTableEv(void*);
extern int func_02012694(int,void*);
typedef struct { int x,y,z; } Vector3;
int func_ov065_02115f84(char* c){
  Vector3 v;
  _ZN5Actor13SmallPoofDustEv(c);
  v.x=*(int*)(c+0x5c);
  v.y=*(int*)(c+0x60);
  v.z=*(int*)(c+0x64);
  _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c,&v,*(unsigned char*)(c+0x10a)+1,0xa000,0);
  _ZN5Actor24KillAndTrackInDeathTableEv(c);
  return func_02012694(0x11e,c+0x74);
}
