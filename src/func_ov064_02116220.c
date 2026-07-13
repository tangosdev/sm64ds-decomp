//cpp
struct ActorV {
  virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
  virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
  virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
  virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
  virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
  virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
  virtual void m90();
};
extern "C" {
typedef struct { int x, y, z; } Vector3;
typedef struct Actor Actor;
typedef struct BCA_File BCA_File;
struct RaycastGround { char buf[0x50]; };
void _ZN13RaycastGroundC1Ev(RaycastGround* self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3* p, Actor* a);
int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
int func_02037e38(unsigned int* p);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, BCA_File* f, int a, int b, unsigned int e);
void _ZN13RaycastGroundD1Ev(RaycastGround* self);

void func_ov064_02116220(char* c){
  RaycastGround rg;
  _ZN13RaycastGroundC1Ev(&rg);
  Vector3 v;
  int y = *(int*)(c+0x60);
  int yoff = *(int*)(c+0x3ec);
  int z = *(int*)(c+0x64);
  int x = *(int*)(c+0x5c);
  int sum = y + yoff;
  int yv = sum + 0x96000;
  v.x = x;
  v.y = yv;
  v.z = z;

  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, (Actor*)c);
  if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
    *(int*)(c+0x3f4) = *(int*)(rg.buf + 0x44);
    if (*(int*)(c+0x60) <= *(int*)(rg.buf + 0x44) + 0x14000) {
      int r = func_02037e38((unsigned int*)(rg.buf + 0x14));
      if (r == 4 || r == 5 || r == 0x13) {
        *(int*)(c+0x398) = 5;
      } else if (r == 1) {
        *(int*)(c+0x398) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x110,
            *(BCA_File**)(*(int*)(*(int*)(c+0x330) + 4) + 4), 0, 0x1000, 0);
        *(unsigned int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1u;
        ((ActorV*)c)->m90();
        *(int*)(c+0x3a8) = *(int*)(c+0x5c);
        *(int*)(c+0x3ac) = *(int*)(c+0x60);
        *(int*)(c+0x3b0) = *(int*)(c+0x64);
        *(int*)(c+0x3ac) = *(int*)(rg.buf + 0x44) + 0x5000;
      }
    }
  }
  _ZN13RaycastGroundD1Ev(&rg);
}
}
