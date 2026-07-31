// @symbol func_ov002_020bf40c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
struct RaycastGround { char pad[0x50]; };
extern unsigned char NumStars(void);
extern void AddVec3(struct Vector3* a, struct Vector3* b, struct Vector3* c);
extern void _ZN13RaycastGroundC1Ev(struct RaycastGround* r);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround* r, struct Vector3* p, char* a);
extern int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround* r);
extern void _ZN13RaycastGroundD1Ev(struct RaycastGround* r);
extern void* data_0209f318;
int func_ov002_020bf40c(char* c){
  struct Vector3 pos;
  struct Vector3 off;
  struct RaycastGround rg;
  if(NumStars() < 0x50 || *(int*)(c+8) != 0){
    AddVec3((struct Vector3*)(c+0x5c), &data_ov002_0210a590, (struct Vector3*)(c+0x5c));
    off.x = data_ov002_0210a590.x;
    off.y = data_ov002_0210a590.y;
    off.z = data_ov002_0210a590.z;
    func_0200d858(data_0209f318, &off);
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
