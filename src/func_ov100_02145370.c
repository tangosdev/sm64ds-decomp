struct Vec3 { int x, y, z; };
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void* Vec3_RotateYAndTranslate(void* out, void* base, short ang, struct Vec3* v);
extern unsigned char CURR_PLAYER_ID[];
extern char* PLAYER_ARR[];
extern int _ZN7Vector38IDENTITYE;
void* func_ov100_02145370(char* c){
  struct Vec3 v;
  char* r5 = PLAYER_ARR[CURR_PLAYER_ID[0]];
  Vec3_Sub(&v, (struct Vec3*)(r5+0x5c), (struct Vec3*)(c+0x5c));
  Vec3_RotateYAndTranslate(c+0x80, &_ZN7Vector38IDENTITYE, (short)(-(*(short*)(c+0x8e))), &v);
  return r5;
}
