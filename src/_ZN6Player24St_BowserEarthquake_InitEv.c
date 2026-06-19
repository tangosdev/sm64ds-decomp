extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
int _ZN6Player24St_BowserEarthquake_InitEv(char* c){
  if(*(unsigned char*)(c+0x6e3)==0){
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x12,0x40000000,0x1000,0);
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
    *(unsigned short*)(c+0x6a4)=0x30;
  }
  return 1;
}
