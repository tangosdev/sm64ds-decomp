struct Vec3 { int x, y, z; };
struct RaycastGround { char pad[0x50]; };
extern unsigned char NumStars(void);
extern void AddVec3(struct Vec3* a, struct Vec3* b, struct Vec3* c);
extern void func_0200d858(void* self, const struct Vec3* offset);
extern void _ZN13RaycastGroundC1Ev(struct RaycastGround* r);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround* r, struct Vec3* p, char* a);
extern int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround* r);
extern void _ZN13RaycastGroundD1Ev(struct RaycastGround* r);
extern struct Vec3 data_ov002_0210a590;
extern void* CAMERA;
int func_ov002_020bf40c(char* c){
  struct Vec3 pos;
  struct Vec3 off;
  struct RaycastGround rg;
  if(NumStars() < 0x50 || *(int*)(c+8) != 0){
    AddVec3((struct Vec3*)(c+0x5c), &data_ov002_0210a590, (struct Vec3*)(c+0x5c));
    off.x = data_ov002_0210a590.x;
    off.y = data_ov002_0210a590.y;
    off.z = data_ov002_0210a590.z;
    func_0200d858(CAMERA, &off);
    _ZN13RaycastGroundC1Ev(&rg);
    {
      int py = *(int*)(c+0x60);
      int pz = *(int*)(c+0x64);
      pos.x = *(int*)(c+0x5c);
      pos.y = py + 0xa0000;
      pos.z = pz;
    }
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, c);
    if(_ZN13RaycastGround10DetectClsnEv(&rg)){
      *(int*)(c+0x644) = *(int*)((char*)&rg + 0x44);
    }
    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
  }
  return 0;
}
