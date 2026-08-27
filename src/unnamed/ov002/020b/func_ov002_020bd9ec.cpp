//cpp
extern "C" {
extern int _ZN5Sound8EndMusicEjj(unsigned char);
extern int _ZN5Sound8SetMusicEjj(unsigned char, unsigned int);
void func_ov002_020bd9ec(char* c, unsigned int r4){
  unsigned int r2=*(unsigned int*)(c+0x67c);
  if(r2==r4){ *(unsigned int*)(c+0x678)=r4; return; }
  if((*(unsigned int*)(c+0x678)|r2)!=0){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
  }
  *(unsigned int*)(c+0x678)=r4;
  *(unsigned int*)(c+0x680)=r4;
  _ZN5Sound8SetMusicEjj(*(unsigned char*)(c+0x6d8),r4);
}
}
