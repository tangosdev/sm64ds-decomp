extern void func_ov007_020af34c(void);
extern void* data_ov007_0210342c;
void func_ov007_020b1604(void){
  char* r2 = (char*)data_ov007_0210342c;
  int r1 = *(int*)(r2+0x10);
  if(r1==9){
    *(int*)(r2+0x14)=0;
    *(int*)((char*)data_ov007_0210342c+0x10)=0;
    func_ov007_020af34c();
    return;
  }
  if(r1==8){
    *(short*)(*(char**)(r2+4)+2)=6;
  } else {
    *(short*)(*(char**)(r2+8)+2)=7;
  }
}
