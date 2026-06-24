extern int func_020124c4(int a, int b, int c, int d);
extern void func_ov075_02115134(char* c);
extern int func_0203da9c(void);
extern int func_ov075_02115098(char* c, int arg1);
extern int func_0203da3c(void);
extern void LoadTextNarcs(void);
extern int LoadArchive(int idx);
extern void func_02030aa4(int arg);
extern void func_ov075_0211a194(char* c, void* p);
extern int data_ov075_0211d918;

void func_ov075_02119640(char* r4){
  *(int*)(r4 + 0x278) = func_020124c4(*(int*)(r4 + 0x278), 2, 0x11d, 0);
  if (*(unsigned char*)(r4 + 0x282) == 0) {
    func_ov075_02115134(*(char**)(r4 + 0x50));
    *(unsigned char*)(r4 + 0x282) = (unsigned char)func_ov075_02115098(*(char**)(r4 + 0x50), func_0203da9c());
  }
  if (func_0203da3c() != 0) return;
  LoadTextNarcs();
  LoadArchive(0);
  LoadArchive(1);
  func_02030aa4(1);
  func_ov075_0211a194(r4, &data_ov075_0211d918);
}
