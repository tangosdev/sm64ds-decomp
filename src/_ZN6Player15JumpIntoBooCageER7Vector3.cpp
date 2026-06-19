//cpp
extern "C" {
struct Vector3 { int x,y,z; };
extern int _ZN6Player17SetNoControlStateEhih(void*,unsigned char,int,unsigned char);
extern short Vec3_HorzAngle(const struct Vector3*, const struct Vector3*);
int _ZN6Player15JumpIntoBooCageER7Vector3(void* c, struct Vector3* v){
  if(*(unsigned char*)((char*)c+0x6de)){
    if(*(int*)((char*)c+0xa8) < 0) return 0;
  }
  if(!_ZN6Player17SetNoControlStateEhih(c,9,-1,0)) return 0;
  *(int*)((char*)c+0x744)=v->x;
  *(int*)((char*)c+0x748)=v->y;
  *(int*)((char*)c+0x74c)=v->z;
  *(short*)((char*)c+0x8e)=Vec3_HorzAngle(v,(struct Vector3*)((char*)c+0x5c))+0x8000;
  return 1;
}
}
