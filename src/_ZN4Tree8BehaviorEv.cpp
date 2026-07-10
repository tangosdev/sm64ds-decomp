//cpp
extern "C" {
extern int _ZN12CylinderClsn5ClearEv(void*);
extern int _ZN12CylinderClsn6UpdateEv(void*);
extern char* data_ov002_02110a48[5];
int _ZN4Tree8BehaviorEv(void){
  char** pp = data_ov002_02110a48;
  int i;
  for(i=0;i<5;i++){
    char* p = *pp;
    while(p){
      _ZN12CylinderClsn5ClearEv(p+0xc);
      _ZN12CylinderClsn6UpdateEv(p+0xc);
      p = *(char**)(p+0x48);
    }
    pp++;
  }
  return 1;
}
}
