typedef struct { int x,y; } Vec2_Fix12;
extern void Vec2_Sub(int* o, int* a, int* b);
extern void func_0203d680(Vec2_Fix12* out, const Vec2_Fix12* in, int scale);
void func_ov006_02109530(int* out, int* a, int scale){
  Vec2_Fix12 v1;
  Vec2_Fix12 v2;
  out[6]=a[0];
  out[7]=a[1];
  Vec2_Sub((int*)&v1, out+6, out);
  func_0203d680(&v2, &v1, scale);
  out[4]=v2.x;
  out[5]=v2.y;
  if(out[4]<0) out[4]=-out[4];
  if(out[5]<0) out[5]=-out[5];
}
