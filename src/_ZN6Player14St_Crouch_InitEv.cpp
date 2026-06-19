//cpp
struct Vector3 { int x,y,z; };
extern "C" {
extern unsigned char data_020a0e40;
extern unsigned char data_0209f49c[];
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,struct Vector3*);
extern int func_ov002_020d1204(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
int _ZN6Player14St_Crouch_InitEv(char* c){
  if(*(unsigned char*)(c+0x6ed)){
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0x2e,(struct Vector3*)(c+0x74));
  }
  *(short*)(c+0x6a4)=0x1e;
  if(*(unsigned short*)(data_0209f49c + data_020a0e40*0x18) & 2){
    if(func_ov002_020d1204(c)) return 1;
  }
  if(*(unsigned char*)(c+0x6e3)==0){
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x2d,0x40000000,0x1000,0);
    *(short*)(c+0x94)=*(short*)(c+0x8e);
  } else if(*(unsigned char*)(c+0x6e3)==1){
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x2c,0,0x1000,0);
  }
  return 1;
}
}
