extern int func_ov007_020b2614(int a, int b, int c);
extern void func_ov007_020b131c(void);
extern void func_ov007_020b12e8(void);
extern void* data_ov007_0210342c;
void func_ov007_020b11bc(void){
  char* o=(char*)data_ov007_0210342c;
  char* p=*(char**)(o+8);
  int r4=*(int*)(p+0xc);
  if(r4==0){
    char* q=*(char**)(o+0xc);
    *(short*)(q+2)=0xe;
    func_ov007_020b2614(2,0,8);
    *(int*)((char*)data_ov007_0210342c+0x180)=9;
    func_ov007_020b131c();
  }
  if(r4==1){
    func_ov007_020b12e8();
  }
}
