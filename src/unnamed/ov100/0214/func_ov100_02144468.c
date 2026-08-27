extern int func_ov100_02145080(void*);
extern void _ZN8SaveData17SetCharacterIntroEi(int);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int, int);
extern int func_ov100_021453d8(void* c, void* p, int a2);
extern char data_ov100_02148704;
extern char data_ov100_021488b4[];
int func_ov100_02144468(char *c, int p){
  if(!func_ov100_02145080(c)){
    data_ov100_02148704=0;
    if(*(int*)(c+8)!=0xd){
      _ZN8SaveData17SetCharacterIntroEi(*(signed char*)(c+0x100+0x44)-1);
    } else {
      _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0xcb33);
    }
    func_ov100_021453d8(c, data_ov100_021488b4, p);
  }
  return 1;
}
