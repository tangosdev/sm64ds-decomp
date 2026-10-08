//cpp
extern "C" {
extern void func_ov007_020b3edc(int);
extern void func_ov007_020bc53c(int);
extern int data_ov007_020d7c5c[];
}
extern "C" void func_ov007_020bea44(unsigned char* r4){
  func_ov007_020b3edc(3);
  func_ov007_020b3edc(*(int*)((char*)data_ov007_020d7c5c + r4[1]*8 + r4[0]*4));
  func_ov007_020b3edc(0xe);
  int p=*(int*)(r4+4);
  if(p){
    func_ov007_020bc53c(p);
    *(int*)(r4+4)=0;
  }
  extern void func_ov007_020c3d1c(void*);
  func_ov007_020c3d1c(r4);
}
