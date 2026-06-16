//cpp
extern "C" {
extern int func_0203aad0(void*);
extern int _ZN21ExtendingMeshCollider9SetScaleYE5Fix12IiE(void*,int);
int func_ov045_021119c0(char* c){
  int v=func_0203aad0(c+0x158);
  if(*(unsigned char*)(c+0xd4)){
    _ZN21ExtendingMeshCollider9SetScaleYE5Fix12IiE(c+0x158, v+8);
    if(func_0203aad0(c+0x158)>0x1000) *(char*)(c+0xd4)=0;
  } else {
    _ZN21ExtendingMeshCollider9SetScaleYE5Fix12IiE(c+0x158, v-8);
    if(func_0203aad0(c+0x158)<0x800) *(char*)(c+0xd4)=1;
  }
  return 1;
}
}
