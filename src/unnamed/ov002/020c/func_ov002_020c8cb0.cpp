//cpp
extern "C" {
extern int _ZN6Player12FinishedAnimEv(void*);
extern void func_ov002_020ca108(char* c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern signed char data_02092110;
int func_ov002_020c8cb0(char* c){
  if (_ZN6Player12FinishedAnimEv(c)) {
    if (data_02092110 >= 0)
      *(unsigned char*)(c+0x70b) = 1;
    func_ov002_020ca108(c);
    *(unsigned char*)(c+0x743) = 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
  }
  return 0;
}
}
