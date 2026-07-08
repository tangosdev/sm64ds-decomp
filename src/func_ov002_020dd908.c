// NONMATCHING: different op / idiom (div=8). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vec3 { int x, y, z; };
extern int Vec3_Dist(const struct Vec3* a, const struct Vec3* b);
extern void func_ov002_020dd8f4(char* p);
extern unsigned char CURRENT_GAMEMODE;
extern unsigned char NUM_PLAYERS;
extern char* PLAYER_ARR[];
void func_ov002_020dd908(char* sb){
  int i;
  struct Vec3 v;
  if((int)(CURRENT_GAMEMODE == 1) == 0) return;
  for(i = 0; i < NUM_PLAYERS; i++){
    char* o = PLAYER_ARR[i];
    int* p;
    if(o == 0) continue;
    if(*(int*)(o+0x37c) == 0) continue;
    p = (int*)(o+0x5c);
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
    v.y = *(int*)(sb+0x60);
    if(Vec3_Dist((struct Vec3*)(sb+0x5c), &v) >= 0x12c000) continue;
    func_ov002_020dd8f4(o);
  }
}
