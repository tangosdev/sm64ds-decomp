//cpp
extern "C" {
struct V{int x,y,z;};
extern char* _ZN5Actor22ClosestNonVanishPlayerEv(void);
extern short Vec3_HorzAngle(const void*,const struct V*);
extern short Vec3_VertAngle(const void*,const struct V*);
extern int Vec3_Dist(const void*,const struct V*);
extern void Matrix4x3_FromRotationY(void*,short);
extern void MulVec3Mat4x3(void*,void*,void*);
extern void ApproachAngle(void*,short,int,int,int);
extern int func_ov065_0211691c(void*,void*);
extern int MATRIX_SCRATCH_PAPER[];
extern int data_ov075_0211d650[];
int func_ov065_021165d8(char*c){
  short r4=0;
  char*p=_ZN5Actor22ClosestNonVanishPlayerEv();
  if(p!=0){
    struct V tmp=*(struct V*)(p+0x5c);
    struct V v;
    struct V a; a.x=tmp.x; a.y=tmp.y; a.z=tmp.z;
    *(short*)(c+0x3e0)=Vec3_HorzAngle(c+0x5c,&a);
    struct V b; b.x=tmp.x; b.y=tmp.y; b.z=tmp.z;
    r4=Vec3_VertAngle(c+0x5c,&b);
    if(Vec3_Dist(c+0x5c,&tmp)>=0x1f4000){
      *(int*)(c+0xa4)=0;*(int*)(c+0xa8)=0;*(int*)(c+0xac)=0;
    } else {
      v.z=0; v.z=-0x1000; v.x=0; v.y=0;
      Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER,*(short*)(c+0x8e));
      MulVec3Mat4x3(&v,MATRIX_SCRATCH_PAPER,c+0xa4);
    }
  } else {
    *(short*)(c+0x100)=r4;
  }
  ApproachAngle(c+0x94,*(short*)(c+0x3e0),1,0x500,0x500);
  ApproachAngle(c+0x92,r4,1,0x500,0x500);
  if(*(unsigned short*)(c+0x100)==0)
    func_ov065_0211691c(c,data_ov075_0211d650);
  return 1;
}
}
