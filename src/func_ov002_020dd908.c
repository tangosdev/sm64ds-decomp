struct Vec3 { int x, y, z; };
#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))
extern int Vec3_Dist(const struct Vec3* a, const struct Vec3* b);
extern void func_ov002_020dd8f4(char* p);
extern unsigned char data_0209f2d8;
extern unsigned char data_0209f21c;
extern char* data_0209f394[];
void func_ov002_020dd908(char* sb){
  int i;
  struct Vec3 v;
  if((int)(data_0209f2d8 == 1) == 0) return;
  for(i = 0; i < data_0209f21c; i++){
    char* o = data_0209f394[i];
    int* p;
    if(o == 0) continue;
    if(*(int*)(o+0x37c) == 0) continue;
    p = (int*)AT(o, 0x5c);
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
    v.y = *(int*)(sb+0x60);
    if(Vec3_Dist((struct Vec3*)(sb+0x5c), &v) >= 0x12c000) continue;
    func_ov002_020dd8f4(o);
  }
}
