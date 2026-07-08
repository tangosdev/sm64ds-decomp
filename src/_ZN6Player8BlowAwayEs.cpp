//cpp
extern "C" {
extern int _ZN6Player7ST_BONKE[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
void _ZN6Player8BlowAwayEs(char* c, short v){
  if(*(unsigned char*)(c+0x6f9)) return;
  if(*(unsigned char*)(c+0x6fd)) return;
  *(short*)(c+0x94)=v;
  *(short*)(c+0x8e)=(short)(v+0x8000);
  _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player7ST_BONKE);
}
}
