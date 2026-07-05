extern void _Z15ApproachLinear2Rsss(short* a, short b, short c);
struct Vec3 { int x,y,z; };
extern void AddVec3(struct Vec3* a, struct Vec3* b, struct Vec3* c);
void func_ov006_0212287c(unsigned char* o){
  _Z15ApproachLinear2Rsss((short*)(o+0x76), 0, 1);
  short* p = (short*)(((int)o + 0x74) & 0xFFFFFFFFFFFFFFFFULL);
  *p = *p + 0x400;
  AddVec3((struct Vec3*)(o+0x5c), (struct Vec3*)(o+0x68), (struct Vec3*)(o+0x5c));
}
