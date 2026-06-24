extern int func_ov100_02145014(void);
extern int _ZN6Player12GetTalkStateEv(char* p);
extern void func_ov100_02145e10(char* c, char* a);
extern int func_ov100_02145e74(char* c, char* a);
extern void func_ov100_02145f68(char* c, void* p, char* a2);
extern signed char data_ov100_02148390[];
extern int data_ov100_02148964;

int func_ov100_02145b9c(char* r5, char* r4){
  func_ov100_02145014();
  if (_ZN6Player12GetTalkStateEv(r4) == -1) {
    if (data_ov100_02148390[*(int*)(r5 + 8) * 6] == 0x50) {
      func_ov100_02145e10(r5, r4);
    } else if (func_ov100_02145e74(r5, r4) == 0) {
      func_ov100_02145f68(r5, &data_ov100_02148964, r4);
    }
  }
  return 1;
}
