struct State { int a; int b; };
extern int _ZN6Player7IsStateERNS_5StateE(void *c, struct State *s);
extern struct State data_ov002_0211022c;

int _ZN6Player12Unk_020c9e5cEh(void *c, unsigned char h){
  if(!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211022c)) return 0;
  if(*(unsigned char*)((char*)c+0x70a) != h) return 0;
  switch(h){
  case 0:
  case 3:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==1) return 1; }
    break;
  case 0x11:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==0x18) return 1; }
    /* fallthrough */
  case 1:
  case 2:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==2 || (unsigned char)(s+0xec)<=1) return 1; }
    break;
  case 4:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==3) return 1; }
    break;
  case 5:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if((unsigned char)(s+0xfc)<=1) return 1; }
    break;
  case 6:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==6) return 1; }
    break;
  case 7:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if((unsigned char)(s+0xf9)<=1) return 1; }
    break;
  case 8:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==9 || (unsigned char)(s+0xec)<=1) return 1; }
    break;
  case 9:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if((unsigned char)(s+0xf5)<=2) return 1; }
    break;
  case 0xa:
  case 0xc:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if((unsigned char)(s+0xf2)<=1) return 1; }
    break;
  case 0xb:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==0x10) return 1; }
    break;
  case 0xd:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==0x11) return 1; }
    break;
  case 0xe:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if(s==0x12) return 1; }
    break;
  case 0xf:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if((unsigned char)(s+0xea)<=1) return 1; }
    break;
  case 0x10:
    { unsigned char s = *(unsigned char*)((char*)c+0x6e3);
      if((unsigned char)(s+0xe8)<=1) return 1; }
    break;
  }
  return 0;
}
