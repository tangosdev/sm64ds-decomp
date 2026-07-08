//cpp
struct State;
extern State _ZN6Player15ST_SWING_PLAYERE;
extern State _ZN6Player7ST_WALKE;

extern "C" {
void func_ov002_020bdb50(char* c, unsigned int p2);
void func_ov002_020bda48(char* c);
int _ZN6Player7IsStateERNS_5StateE(char* c, State& s);
void func_ov002_020d9c70(char* c);
void func_ov002_020da95c(char* c);
void _ZN6Player11ChangeStateERNS_5StateE(char* c, State& s);
void func_02012790(int n);
void func_ov002_020e6350(char* c);

void _ZN6Player18SetNewHatCharacterEjjb(char* c, unsigned int p1, unsigned int p2, bool p3)
{
  unsigned int old = *(unsigned int*)(c + 8);
  if (p1 == old) return;
  *(unsigned char*)(c + 0x6dc) = (unsigned char)old;
  *(unsigned char*)(c + 0x6dd) = (unsigned char)p1;
  *(unsigned short*)((char*)(((int)c + 0x700) & 0xFFFFFFFFFFFFFFFF) + 0x3c) = 1;
  if (p1 != *(unsigned char*)(c + 0x6d9)) {
    *(unsigned short*)((char*)(((int)c + 0x73c) & 0xFFFFFFFFFFFFFFFF)) |= 0x8000;
  }
  func_ov002_020bdb50(c, p2);
  func_ov002_020bda48(c);
  if (_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player15ST_SWING_PLAYERE)) {
    func_ov002_020d9c70(c);
    func_ov002_020da95c(c);
    _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player7ST_WALKE);
  }
  if (p3 == 0) {
    if (p1 != *(unsigned char*)(c + 0x6d9)) func_02012790(0xb);
    else func_02012790(0xc);
  }
  *(unsigned int*)(c + 8) = *(unsigned char*)(c + 0x6dd);
  func_ov002_020e6350(c);
  *(unsigned int*)(c + 8) = *(unsigned char*)(c + 0x6dc);
}
}
