//cpp
extern "C" {
extern int _ZN5Sound8EndMusicEjj(unsigned char);
extern int _ZN5Sound8SetMusicEjj(unsigned char, unsigned int);
void func_ov002_020bd984(char* c, unsigned int r1){
  unsigned int r2=*(unsigned int*)(c+0x67c);
  if(r2==0){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
    *(unsigned int*)(c+0x680)=0;
  } else if(*(unsigned int*)(c+0x680)==r1 && r2!=r1){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
    _ZN5Sound8SetMusicEjj(*(unsigned char*)(c+0x6d8), *(unsigned int*)(c+0x67c));
    *(unsigned int*)(c+0x680)=*(unsigned int*)(c+0x67c);
  }
  *(unsigned int*)(c+0x678)=0;
}
}
