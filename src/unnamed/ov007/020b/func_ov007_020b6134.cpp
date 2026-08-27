//cpp
extern "C" {
extern void func_ov007_020aec94(int);
extern char* data_ov007_0210342c;
void func_ov007_020b6134(int arg){
  char* G=data_ov007_0210342c;
  switch(*(short*)(*(int*)(G+8))){
  case 0:
    func_ov007_020aec94(arg+3);
    break;
  case 3:
    *(int*)(G+0x58)=arg;
    break;
  case 2:
    *(int*)(G+0x5c)=arg;
    break;
  case 7:
    {
      int v=*(int*)(G+0x58);
      if(v==-1){ *(int*)(G+0x58)=arg; return; }
      if(arg==v) *(int*)(G+0x58)=-1;
      else *(int*)(G+0x5c)=arg;
    }
    break;
  case 9:
    if(*(int*)(G+0x58)==-1) *(int*)(G+0x58)=arg;
    break;
  }
}
}
