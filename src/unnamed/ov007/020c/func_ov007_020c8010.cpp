//cpp
extern "C" {
void SubVec3(int,int,void*);
int LenVec3(void*);
void func_ov007_020c8010(char* r4,int* r1,int* r2){
  *(int**)(r4)=r1;
  *(int**)(r4+4)=r2;
  if(r1[0]==0) return;
  if(r2[0]==0) return;
  SubVec3(*(int*)(*(int**)(r4)),*(int*)(*(int**)(r4+4)),r4+0xc);
  *(int*)(r4+8)=LenVec3(r4+0xc);
}
}
