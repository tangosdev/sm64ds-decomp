//cpp
// @symbol _ZN6Player18SetNewHatCharacterEjjb
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct State;
extern State data_ov002_02110604;
extern State data_ov002_0211013c;

extern "C" {
void func_ov002_020bdb50(char* c, unsigned int p2);
void func_ov002_020bda48(char* c);
int _ZN6Player7IsStateERNS_5StateE(char* c, State& s);
void func_ov002_020d9c70(char* c);
void func_ov002_020da95c(char* c);
void _ZN6Player11ChangeStateERNS_5StateE(char* c, State& s);
void func_02012790(int n);
void func_ov002_020e6350(char* c);
}

void Player::SetNewHatCharacter(unsigned int p1, unsigned int p2, bool p3)
{
  unsigned int old = param1;
  if (p1 == old) return;
  mPrevCharacter = (unsigned char)old;
  mHatCharacter = (unsigned char)p1;
  *(unsigned short*)((char*)(((int)((char*)this) + 0x700)) + 0x3c) = 1;
  if (p1 != mCharacter) {
    *(unsigned short*)((char*)(((int)((char*)this) + 0x73c))) |= 0x8000;
  }
  func_ov002_020bdb50(((char*)this), p2);
  func_ov002_020bda48(((char*)this));
  if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_02110604)) {
    func_ov002_020d9c70(((char*)this));
    func_ov002_020da95c(((char*)this));
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
  }
  if (p3 == 0) {
    if (p1 != mCharacter) func_02012790(0xb);
    else func_02012790(0xc);
  }
  param1 = mHatCharacter;
  func_ov002_020e6350(((char*)this));
  param1 = mPrevCharacter;
}
