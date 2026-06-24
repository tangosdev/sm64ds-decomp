extern int data_ov075_0211d304[];
extern void func_ov075_02115bc8(void);
extern void func_ov075_02115bac(void);
extern int data_02092680[];
extern int data_0208e4b8[];
extern void func_0207328c(void*, int, int, void*);
extern void _ZN9ActorBaseD1Ev(void*);
extern void* data_020a0eac;
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
int* func_ov075_02115b28(int* t){
  t[0]=(int)data_ov075_0211d304;
  func_0207328c((char*)t+0x1b4, 4, 0x2c, (void*)func_ov075_02115bc8);
  func_0207328c((char*)t+0x70, 9, 0x24, (void*)func_ov075_02115bac);
  t[0]=(int)data_02092680;
  t[0]=(int)data_0208e4b8;
  _ZN9ActorBaseD1Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
  return t;
}
