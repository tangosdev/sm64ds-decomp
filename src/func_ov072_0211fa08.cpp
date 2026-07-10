//cpp
extern "C" {
int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(int, void*, int, void*, int, int);
int _ZN6Player12GetTalkStateEv(void*);
unsigned short DecIfAbove0_Short(unsigned short* p);
int func_ov072_0211fcb0(void*, int);
int func_ov072_0211fa08(char* c){
  int v[3];
  unsigned char *st;
  v[0] = *(int*)(c+0x5c);
  int y = *(int*)(c+0x60);
  v[1] = y;
  v[2] = *(int*)(c+0x64);
  v[1] = y + 0x96000;
  switch(*(unsigned char*)(c+0x3a2)){
  case 0:
    if(_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(int*)(c+0x390), c, 0xb0, v, 0, 0) == 0) break;
    st = (unsigned char*)(((int)c + 0x3a2) & 0xFFFFFFFFFFFFFFFFLL);
    *st = *st + 1;
    break;
  case 1:
    if(_ZN6Player12GetTalkStateEv((void*)*(int*)(c+0x390)) != -1) break;
    st = (unsigned char*)(((int)c + 0x3a2) & 0xFFFFFFFFFFFFFFFFLL);
    *st = *st + 1;
    break;
  case 2:
    if(DecIfAbove0_Short((unsigned short*)(c+0x3a0)) == 0){
      func_ov072_0211fcb0(c, 2);
    }
    break;
  }
  return 1;
}
}
