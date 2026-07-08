struct E18 { short v; char pad[0x16]; };
extern struct E18 data_0209f4a0[];
extern struct E18 data_0209f49e[];
extern unsigned char data_020a0e40;
extern char* CAMERA;
extern char _ZN6Player7ST_FALLE;
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* s);
int func_ov002_020d2da0(char* c){
  unsigned char idx = data_020a0e40;
  char* p = CAMERA;
  int cond = (int)(data_0209f4a0[idx].v >= 0x200);
  if(cond | (((unsigned short)data_0209f49e[idx].v) & 0xc03)){
    *(int*)(((int)p + 0x154) & 0xFFFFFFFFFFFFFFFFull) &= ~0x2000;
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2e, c+0x74);
    return 1;
  }
  if(*(unsigned char*)(c+0x6de) == 0) return 0;
  *(int*)(((int)p + 0x154) & 0xFFFFFFFFFFFFFFFFull) &= ~0x2000;
  _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_FALLE);
  return 2;
}
