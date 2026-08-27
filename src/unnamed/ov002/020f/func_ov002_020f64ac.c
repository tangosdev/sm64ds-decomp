extern int _ZN9Animation7AdvanceEv(char*);
extern int _ZN9Animation8FinishedEv(char*);
extern void func_ov002_020f6514(char*, char*, int);
void func_ov002_020f64ac(char* c, char* r4){
  _ZN9Animation7AdvanceEv(c+0x50);
  if(!_ZN9Animation8FinishedEv(c+0x50)) return;
  signed char r2 = *(signed char*)(r4 + (*(unsigned char*)(c+0x82) << 2) + 3);
  if(r2 < 0) return;
  func_ov002_020f6514(c, r4, r2 & 0xff);
}
