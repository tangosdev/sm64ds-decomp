#define M(p) ((void*)(int)(((long long)(int)(p)) & ~0LL))
extern void func_02012790(int);
extern void func_ov006_020c271c(void *c);
extern void func_ov006_020c2664(char *c);

struct E { char _[0x14]; };

void func_ov006_0211b80c(char *c){
  if((unsigned char)c[0x5625] < 2) return;
  if(*(unsigned short*)(c+0x5614) != 0){
    *(unsigned short*)M(c + 0x5614) -= 1;
    if(*(short*)(c+0x5614) <= 0) *(unsigned short*)(c+0x5614) = 0;
    return;
  }
  if((*(unsigned short*)(c+0x5610) & 0xff) == (*(unsigned short*)(c+0x5612) & 0xff)){
    *((char*)&((struct E*)c)[*(unsigned short*)(c+0x5610) >> 8] + 0x50f5) = 0;
    *((char*)&((struct E*)c)[*(unsigned short*)(c+0x5612) >> 8] + 0x50f5) = 0;
    func_02012790(0x26);
    { volatile void* p = (void*)(c+0x4f38); func_ov006_020c271c((void*)p); }
  } else {
    *((char*)&((struct E*)c)[*(unsigned short*)(c+0x5610) >> 8] + 0x50f4) = 3;
    *((char*)&((struct E*)c)[*(unsigned short*)(c+0x5612) >> 8] + 0x50f4) = 3;
    func_02012790(0xe);
    (*(unsigned char*)M(c + 0x5626))--;
    func_02012790(0x12f);
    func_ov006_020c2664(c+0x4f38);
  }
  *(unsigned short*)(c+0x5612) = 0;
  *(unsigned short*)(c+0x5610) = *(unsigned short*)(c+0x5612);
  c[0x5625] = 0;
  c[0x5624] = 0;
}
