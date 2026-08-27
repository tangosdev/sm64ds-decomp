//cpp
extern "C" {
extern unsigned char IsPlayerWarping(int a0);
extern int _ZN6Player7IsStateERNS_5StateE(void *c, void *s);
extern void func_02020388(int handle);
extern char data_ov002_02110154[];
void func_ov002_020c2e78(char* c){
  unsigned char w = IsPlayerWarping(*(unsigned char*)(c+0x6d8));
  if(w==0) return;
  if(!_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110154)){
    func_02020388(*(unsigned char*)(c+0x6d8));
    return;
  }
  *(unsigned char*)(c+0x6e8) = w;
  unsigned short *p = (unsigned short*)(((int)c + 0x6ce));
  *p |= 8;
}
}
