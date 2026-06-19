struct Vector3 { int x,y,z; };
extern char* func_02050cdc(void);
extern int func_02048720(struct Vector3* v, unsigned int a, unsigned int b);
extern void Player_PlaySoundEffect(int x, unsigned int a, unsigned int b);
extern void func_02048908(int x, struct Vector3* v);
extern int func_02048a1c(struct Vector3* v, unsigned int a, unsigned int b);
extern void func_02048d80(int* g, struct Vector3* v);
extern int data_0209b4a4[];
void _ZN5Sound4PlayEjjRK7Vector3(unsigned int j1, unsigned int j2, struct Vector3* v){
  char* s = func_02050cdc();
  int t = *(unsigned char*)(s+5);
  if(t==9 || t==2){
    int r = func_02048720(v, j1, j2);
    if(r==0) return;
    Player_PlaySoundEffect(r, j1, j2);
    func_02048908(r, v);
    return;
  }
  if(func_02048a1c(v, j1, j2)==0) return;
  Player_PlaySoundEffect((int)data_0209b4a4, j1, j2);
  func_02048d80(data_0209b4a4, v);
}
