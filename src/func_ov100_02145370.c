struct Vec3 { int x, y, z; };
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void* Vec3_RotateYAndTranslate(void* out, void* base, short ang, struct Vec3* v);
extern unsigned char data_0209f250[];
extern char* data_0209f394[];
extern int data_020a0ebc;
void* func_ov100_02145370(char* c){
  struct Vec3 v;
  char* r5 = data_0209f394[data_0209f250[0]];
  Vec3_Sub(&v, (struct Vec3*)(r5+0x5c), (struct Vec3*)(c+0x5c));
  Vec3_RotateYAndTranslate(c+0x80, &data_020a0ebc, (short)(-(*(short*)(c+0x8e))), &v);
  return r5;
}
