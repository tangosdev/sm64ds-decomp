struct Vector3 { int x, y, z; };
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, void* file, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(char* self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, char* actor, const struct Vector3* pos, int a, unsigned int b, unsigned int c, unsigned int d);
extern void* data_ov085_02130858;
extern struct Vector3 data_ov085_0212f29c;

int _ZN8WallSign13InitResourcesEv(char* r4){
  struct Vector3 v;
  void* file = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_02130858);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(r4 + 0xd4, file, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(r4);
  v.x = data_ov085_0212f29c.x;
  v.y = data_ov085_0212f29c.y;
  v.z = data_ov085_0212f29c.z;
  _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(r4 + 0x320, r4, &v, 0x78000, 0x64000, 0x4800002, 0);
  return 1;
}
