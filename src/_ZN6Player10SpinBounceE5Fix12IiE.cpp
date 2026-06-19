//cpp
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void*,void*);
extern int _ZN6Player9GetHealthEv(void*);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int,void*);
extern int data_ov002_021105a4[];
extern int data_ov002_02110454[];
void _ZN6Player10SpinBounceE5Fix12IiE(char* c, int f){
  if(_ZN6Player7IsStateERNS_5StateE(c, data_ov002_021105a4)) return;
  if(_ZN6Player9GetHealthEv(c)==0) return;
  *(int*)(c+0xa8) = f;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0x28,(char*)c+0x74);
  _ZN5Sound9PlayBank0EjRK7Vector3(0xb6,(char*)c+0x74);
  *(char*)(c+0x6e6)=1;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110454);
}
}
