extern int func_0203da3c(void);
extern void Deallocate(void* ptr);
extern void LoadTextNarcs(void);
extern void LoadArchive(int);
extern void func_ov075_0211a194(void* c, void* arg);
extern void* data_0209b2e8;
extern int data_ov075_0211d780[];
int func_ov075_0211977c(char* c){
  int r = func_0203da3c();
  if(r != 0) return r;
  Deallocate(data_0209b2e8);
  data_0209b2e8 = 0;
  LoadTextNarcs();
  LoadArchive(0);
  LoadArchive(1);
  func_ov075_0211a194(c, data_ov075_0211d780);
}
