//cpp
// @symbol _ZN6Player10SpinBounceE5Fix12IiE
/* recovered: named members + shared header */
#include "Player.h"
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void*,void*);
extern int _ZN6Player9GetHealthEv(void*);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int,void*);
extern int data_ov002_021105a4[];
extern int data_ov002_02110454[];
void _ZN6Player10SpinBounceE5Fix12IiE(struct Player *self, int f) {
  if(_ZN6Player7IsStateERNS_5StateE(((char*)self), data_ov002_021105a4)) return;
  if(_ZN6Player9GetHealthEv(((char*)self))==0) return;
  self->mVertSpeed = f;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(self->mCharacter,0x28,(char*)((char*)self)+0x74);
  _ZN5Sound9PlayBank0EjRK7Vector3(0xb6,(char*)((char*)self)+0x74);
  self->mStatePhase=1;
  _ZN6Player11ChangeStateERNS_5StateE(((char*)self), data_ov002_02110454);
}
}
