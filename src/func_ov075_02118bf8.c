extern void* data_020a0eac;
extern int _ZN4Heap10SetDefaultEv(void* h);
extern int LoadArchive(int idx);
extern void UnloadArchive(int i);
extern void func_0202deac(int levelID);
extern signed char data_ov075_0211c6ec[];
extern unsigned char data_0209f2d8;
void func_ov075_02118bf8(int arg){
  void* prev = (void*)_ZN4Heap10SetDefaultEv(data_020a0eac);
  LoadArchive(7);
  int n = arg + 2;
  unsigned int i;
  for(i=2; i<=5; i++){
    if(i==(unsigned)n) continue;
    UnloadArchive(i);
  }
  LoadArchive(n);
  _ZN4Heap10SetDefaultEv(prev);
  data_0209f2d8 = 1;
  func_0202deac(data_ov075_0211c6ec[arg]);
}
