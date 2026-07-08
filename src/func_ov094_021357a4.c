typedef struct { int x, y, z; } V3;
extern V3 data_ov094_02136a1c;
extern signed char LEVEL_ID;
extern int data_ov094_02136b70;
extern int _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* self, V3* v);
extern int _ZN5Actor10FindWithIDEj(unsigned id);
extern int func_ov002_020df840(void* a, void* b, void* d);
extern int func_ov094_02136188(void* c, void* p);
void func_ov094_021357a4(char* c){
  int st[3];
  V3 tmp;
  tmp.x = data_ov094_02136a1c.x;
  tmp.y = data_ov094_02136a1c.y;
  tmp.z = data_ov094_02136a1c.z;
  _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c+0x110, &tmp);
  if(*(int*)(c+0x134) == 0) return;
  int a = _ZN5Actor10FindWithIDEj(*(int*)(c+0x134));
  if(a == 0) return;
  int t = *(unsigned short*)(a+0xc);
  unsigned eq = (t == 0xbf);
  if(!eq) return;
  *(int*)(c+0x3cc) = a;
  st[0] = 0;
  st[1] = 0;
  st[2] = 0;
  st[1] = 0x1838000;
  if(LEVEL_ID == 0x16) st[1] = 0x1194000;
  *(int*)(c+0xa4) = 0;
  *(int*)(c+0xa8) = 0;
  *(int*)(c+0xac) = 0;
  if(func_ov002_020df840((void*)*(int*)(c+0x3cc), c, st) != 1){
    *(int*)(c+0x3cc) = 0;
    return;
  }
  func_ov094_02136188(c, &data_ov094_02136b70);
}
