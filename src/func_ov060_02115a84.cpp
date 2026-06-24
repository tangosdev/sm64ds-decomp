//cpp
extern "C" {
struct V3 { int x,y,z; };
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void* p);
extern void _ZN5Actor13LandingDustAtER7Vector3b(void* a, struct V3* v, int b);
extern void func_02012694(int id, void* p);
void func_ov060_02115a84(char* c, char* arg){
  if(_ZNK12WithMeshClsn13JustHitGroundEv(c+0x14c)==0) return;
  *(unsigned short*)arg = *(unsigned short*)arg + 1;
  if(*(unsigned short*)arg >= 4) return;
  struct V3 v;
  v.x = *(int*)(c+0x5c);
  v.y = *(int*)(c+0x60);
  v.z = *(int*)(c+0x64);
  _ZN5Actor13LandingDustAtER7Vector3b(c, &v, 0);
  func_02012694(0xbd, c+0x74);
}
}
